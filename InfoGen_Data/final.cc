/***
*SpaceEngine 0.990 Detailed information generator
*
*       Copyright (C) StellarDX Astronomy.
*
*       This program is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 2 of the License, or
*       (at your option) any later version.
*
*       This program is distributed in the hope that it will be useful,
*       but WITHOUT ANY WARRANTY; without even the implied warranty of
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*       GNU General Public License for more details.
*
*       You should have received a copy of the GNU General Public License along
*       with this program; If not, see <https://www.gnu.org/licenses/>.
*
****/

#include "composite.h"
#include "composite1.h"
#include "composite2.h"
#include "composite3.h"
#include "composite4.h"
#include "final.h"
#include <fstream>
#include <yvals_core.h> // STD version header

//#include "geochronology/final.h"
//#include "mineralogy/final.h"

using namespace std;
using namespace cse;

enum OutputFormat OFormat  = MD;
bool Astrobiology          = false;
uint32_t _OUT_PRECISION    = 12;
string LcID                = "1033";
enum LinkCSS LCSS          = Static;
int srcencoding            = 65001;
int outencoding            = 65001;

ObjectBuffer System;
string Final;
map<string, string> LocStrings;
string OutputFileName;
string CSSPath;

string HTMLhead;
string HTMLcontent;
string HTMLMenu;
int CSSEncod = 65001;
enum CopyOption Cpm = Asking;

void Transcode(string& arg, int encoding)
{
	int len = MultiByteToWideChar(CP_ACP, 0, arg.c_str(), -1, NULL, 0);

	wchar_t* wszUtf8 = new wchar_t[len];
	memset(wszUtf8, 0, len);
	MultiByteToWideChar(CP_ACP, 0, arg.c_str(), -1, wszUtf8, len);

	len = WideCharToMultiByte(encoding, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char* szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(encoding, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);

	arg = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;
}

void ParseLocalStrings(string FileName, string LCID, UINT CP)
{
	ISCStream LCIS;
	bool FileExist = true;
	try{ LCIS = ParseFile("./" + LCID + "/" + FileName, CP); }
	catch (const std::exception&)
	{
		cout << "Localization is not specified or file not exist, using default strings.\n";
		FileExist = false;
	}

	if (!FileExist) { return; }

	string InternationalName = sc::GetAs<string>(LCIS, "InternationalName");
	string LocalName = sc::GetAs<string>(LCIS, "LocalName");
	cout << "Enabled localization: " + LocalName + '(' + InternationalName + ')' + '\n';

	auto it = LCIS->find("Translations");
	if (it == LCIS->end())
	{
		cout << "Invalid localization file.\n";
		abort();
	}

	auto Translations = it->SubTable;
	for (size_t i = 0; i < Translations->Catalogue.size(); i++)
	{
		LocStrings.insert({ Translations->Catalogue[i].Key, sc::GetAs<string>(Translations, Translations->Catalogue[i].Key) });
	}
}

void NormalProcess(ISCStream& SystemIn, vector<string> args)
{
	UINT LCodePage = 65001;
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 8) == "-lchset=")
		{
			string lccp = args[i];
			LCodePage = stoul(lccp.substr(8, lccp.size() - 8));
			break;
		}
	}

	cout << "Loading localizations...\n";
	ParseLocalStrings("SystemInfo.cfg", LcID, LCodePage);

	cout << "Loading - Initializing object phase 1...\n";
	auto it = SystemIn->begin();
	auto end = SystemIn->end();
	while (it != end)
	{
		System.push_back(ObjectLoader(it));
		++it;
	}

	cout << vformat("{} Objects loaded.\n", make_format_args(System.size()));

	composite();
	composite1();
	composite2(args);
	if (Astrobiology)
	{
		composite3(args);
		composite4();
	}

	if (OFormat == HTML) { HTMLWrite(&Final, HTMLhead, HTMLMenu, HTMLcontent); }
}

#ifdef _DEBUG
#define GEOCHRO_PATH "Geochronology.exe"
#else
#define GEOCHRO_PATH ".\\InfoGen_Data\\Geochronology.exe"
#endif

void GeochronologyCall(vector<string> args)
{
	// Construct data buffer
	cout << "Constructing data buffer...\n";
	auto DataBufferHInst = LoadLibraryA(_DATABUFFER_PATH);
	if (!DataBufferHInst)
	{
		cout << "Data buffer couldn't be constructed, program will exit.\n";
		exit(3);
	}
	cout << "Data buffer address: " << DataBufferHInst << '\n';

	// send data
	cout << "Exporting data...\n";
	void (*SendDataFPtr)(string _Buf, int _OFmt, UINT _ICP, UINT _OCP, string _OFName, string _LCID)
		= (void (*)(string, int, UINT, UINT, string, string))GetProcAddress(DataBufferHInst, "SendBasicParams");

	SendDataFPtr(Final, OFormat, srcencoding, outencoding, OutputFileName, LcID);

	if (OFormat == HTML)
	{
		void (*SendHTMLFPtr)(int _LCSSM, string _CSSPath, int _CPMethod)
			= (void (*)(int, string, int))GetProcAddress(DataBufferHInst, "SendHTMLParams");
		SendHTMLFPtr(LCSS, CSSPath, Cpm);
	}

	cout << "DONE.\n";

	cout << "Starting Geochronology process...\n";

	string CmdLine;
	args[0] = GEOCHRO_PATH;
	for (auto i : args)
	{
		CmdLine += "\"" + i + "\"" + ' ';
	}
	char CmdStr[500];
	strcpy_s(CmdStr, CmdLine.c_str());

	STARTUPINFOA GeoChronoInfo;
	PROCESS_INFORMATION GeoChronoPInf;
	memset(&GeoChronoInfo, 0, sizeof(STARTUPINFO));
	GeoChronoInfo.cb = sizeof(STARTUPINFO);
	memset(&GeoChronoPInf, 0, sizeof(PROCESS_INFORMATION));

	bool GeochronoReturn = CreateProcessA(nullptr, CmdStr, nullptr, nullptr, false, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &GeoChronoInfo, &GeoChronoPInf);
	if (!GeochronoReturn) 
	{
		cout << "Geochronology process start failed.\n";
	}

	cout << "Waiting for process...\n";
	WaitForSingleObject(GeoChronoPInf.hProcess, 0xFFFFFFFF);

	cout << "Geochronology exited, receiving result.\n";
	char DBuf[0x7FFF], OFname[200], LocID[5];
	void (*LoadDataFPtr)(char* _DBuf, UINT * _OFmt, UINT * _Scp, UINT * _Ocp, char* _OFName, char* _LCID)
		= (void (*)(char*, UINT*, UINT*, UINT*, char*, char*))GetProcAddress(DataBufferHInst, "LoadBasicParams");
	LoadDataFPtr(DBuf, (UINT*)(&OFormat), (UINT*)&srcencoding, (UINT*)&outencoding, OFname, LocID);

	Final = DBuf;
	OutputFileName = OFname;
	LcID = LocID;

	cout << "DONE.\n";
	
	CloseHandle(GeoChronoPInf.hProcess);
	CloseHandle(GeoChronoPInf.hThread);
	FreeLibrary(DataBufferHInst);
}

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const* argv[]) // main function can return "void" in C++20, WHY???
{
	SetDllDirectoryA("./InfoGen_Data/SharedObjects");

	cout << "SpaceEngine 0.990 Detailed information generator\nCopyright (C) StellarDX Astronomy.\n";
	cout << "This is a free software, see the source for copying condition. \n";
	//cout << "There is no warranty; not even for merchantability or fitness for a particular purpose.\n";
	cout << "Built with " + COMPILER_VERSION << '\n' << '\n';

	if (argc == 1)
	{
		cout << "Usage: \n";
		cout << "\033[1m\t\033[35mInfoGen \033[36m<filename> \033[31m<args...>\n\033[0m\n";

		cout << "Additional arguments: \n\n";
		cout << "\t\033[32m-fmt=<fmt> \033[0m: Specify output format, supports Github Markdown(md) and HTML(html).\n";
		cout << "\t\033[32m-srccp=<encod> \033[0m: Specify Source file encoding, use numbers of codepage, e.g. -srccp=0 -> ANSI.\n";
		cout << "\t\033[32m-outcp=<encod> \033[0m: Specify output file encoding.\n";
		cout << "\t\033[32m-out <filename> \033[0m: Specify output file name.\n";
		cout << "\t\033[32m-seed=<seed> \033[0m: Specify seed of random generator, using Hexadecimal(0 - FFFFFFFF), it will randomly generated when argument is missing.\n";
		cout << "\t\033[32m-prec=<int> \033[0m: Specify output precision, default is 12.\n";
		cout << "\t\033[32m-lnkcss <path> \033[0m: Linking css file, only available for html mode.\n";

		cout << "\t\033[32m-lcssmode=<mode>(-cp<encod>) \033[0m: CSS link mode, only available for html mode. The following values are available:\n";
		cout << "\t\t\033[33mstatic \033[0m- (Default value) link css with absolute path, maybe cause problems when files are moved.\n";
		cout << "\t\t\033[33mcopy \033[0m- copy the css file to output path and link it with relative path. add \"-skip\" or \"-replace\" at the end to disable asking when file is already exist.\n";
		cout << "\t\t\033[33minline \033[0m- merge html and css into a single file. Most stable but the output file maybe very big.\n";
		cout << "\t\tAddition: Custom encoding, aka. the \"-cp...\" after this argument, of css files is supported when using inline method.\n";

		cout << "\t\033[32m-cpcss \033[0m: [\033[31mArgument for early versions, replaced by \"lcssmode\"\033[0m] Copy css file to destination directory, only available for html mode.\n";

		cout << "\t\033[32m-mpsort=<char> \033[0m: Generate a list of minor planets(include dwarf planets) that are exceptional in some way. The following values are valid:\n";
		cout << "\t\t\033[33mdiameter \033[0m- IRAS standard, asteroids with a diameter greater than 120 Km (This is default option when argument is missing or invalid.)\n";
		cout << "\t\t\033[33mmass \033[0m- Asteroids with nominal mass > 1E+18 kg\n";
		cout << "\t\t\033[33mslowrotator \033[0m- slowest-rotating known minor planets with a period of at least 1000 hours\n";
		cout << "\t\t\033[33mfastrotator \033[0m- fastest-rotating minor planets with a period of less than 100 seconds\n";
		cout << "\t\t\033[33mretrograde \033[0m- Minor planets with orbital inclinations greater than 90 deg\n";
		cout << "\t\t\033[33mhighinclined \033[0m- Minor planets with orbital inclinations greater than 10 deg and smaller than 90 deg\n";
		cout << "\t(Only mass and diameter is valid when proccessing mode is switched to mineral generation.)\n";
		cout << "\n";
		cout << "\t\033[32m-astrobiology \033[0m: Calculate ESI and Habitabilities for rocky planets.\n";
		cout << "\n";
		cout << "\t\033[32m-geochronology \033[0m: Generate timeline of the evolutionary history of a life planet, the following options are available.\n";
		cout << "\t\t\033[33m-target <ObjectName> \033[0m: Specify target object. (This argument is required under geochronology mode.)\n";
		cout << "\t\t\033[33m-parent <ObjectName> \033[0m: Specify parent body, it will automatically detected when missing.\n";
		cout << "\t\t\033[33m-evmdl <File> \033[0m: Specify custom evolution model.\n";
		cout << "\t\t\033[33m-evmcp=<encod> \033[0m: Specify file encoding of custom evolution model.\n";
		cout << "\n";
		cout << "\t\033[32m-mineralogy \033[0m: Generate mineral distribution of all the rocky objects in this system, the following options are available.\n";
		cout << "\t\t\033[33m-oredict <filename> \033[0m: Specify custom ore dictionary file, program will use default ore dictionary when this argument is missing.\n";
		cout << "\t\t\033[33m-oredictencod=<encod> \033[0m: Specify file encoding of custom ore dictionary, default is 65001(UTF-8).\n";
		cout << "\n";

		cout << "Localizations: \n\n";
		cout << "\t\033[32m-lcid=<ID> \033[0m: Locale ID, e.g. -lcid=1033 -> en_us.\n";
		cout << "\t\033[32m-lchset=<encod> \033[0m: Encoding of localization file.\n";

		return 0x7FFFFFFF;
	}

	vector<string> args;
	for (size_t i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

	// Output format
	if (find(args.begin(), args.end(), "-fmt=html") != args.end()) { OFormat = HTML; }
	else if (find(args.begin(), args.end(), "-fmt=md") != args.end()) { OFormat = MD; }

	// Set Output Name
	if (args[1].substr(args[1].size() - 3, 3) != ".sc")
	{
		args[1] += ".sc";
	}
	switch (OFormat)
	{
	case HTML:
		OutputFileName = args[1].substr(0, args[1].size() - 3) + ".html";
		break;
	case MD:
	default:
		OutputFileName = args[1].substr(0, args[1].size() - 3) + ".md";
		break;
	}

	//int outencoding = 65001; // Default encoding is UTF-8
	for (size_t i = 0; i < args.size(); i++)
	{
		// Seed
		if (args[i].substr(0, 6) == "-seed=")
		{
			string encodstr = args[i];
			random.seed(stoull(encodstr.substr(6, encodstr.size() - 6), nullptr, 16));
		}

		// Localization
		else if (args[i].substr(0, 6) == "-lcid=")
		{
			string lcstr = args[i];
			LcID = lcstr.substr(6, lcstr.size() - 6);
		}

		// Output precision
		else if (args[i].substr(0, 6) == "-prec=")
		{
			string encodstr = args[i];
			_OUT_PRECISION = stoi(encodstr.substr(6, encodstr.size() - 6));
			if (_OUT_PRECISION > 32)
			{
				cout << "Precision is too high to output.\n";
				abort();
			}
		}

		else if (args[i] == "-out" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid output filename." << '\n';
				abort();
			}
			OutputFileName = args[i + 1];
		}

		// Linking CSS
		else if (OFormat == HTML && args[i] == "-lnkcss" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid css filename." << '\n';
				abort();
			}
			CSSPath = args[i + 1];
		}

		else if (OFormat == HTML && args[i].substr(0, 10) == "-lcssmode=")
		{
			if (args[i] == "-lcssmode=static") { LCSS = Static; }
			else if (args[i] == "-lcssmode=copy") { LCSS = Copy; }
			else if (args[i] == "-lcssmode=copy-skip")
			{
				LCSS = Copy;
				Cpm = Skip;
			}
			else if (args[i] == "-lcssmode=copy-replace")
			{
				LCSS = Copy;
				Cpm = Replace;
			}
			else if (args[i].substr(0, 16) == "-lcssmode=inline")
			{
				LCSS = Inline;
				string cpstr = args[i].substr(16, cpstr.size() - 16);
				if (cpstr.empty() || cpstr.substr(0, 3) != "-cp") {}
				else
				{
					CSSEncod = stoi(cpstr.substr(3, cpstr.size() - 3));
				}
			}
			else
			{
				cout << "Invalid css mode." << '\n';
				abort();
			}
		}

		else if (args[i].substr(0, 7) == "-srccp=")
		{
			string encodstr = args[i];
			srcencoding = stoi(encodstr.substr(7, encodstr.size() - 7));
		}

		else if (args[i].substr(0, 7) == "-outcp=")
		{
			string encodstr = args[i];
			outencoding = stoi(encodstr.substr(7, encodstr.size() - 7));
		}
	}

	if (OFormat == HTML && find(args.begin(), args.end(), "-cpcss") != args.end()) { LCSS = Copy; } // Capability of earlier version.

	// Processing

	if (OFormat == HTML)
	{
		Final += string(_Html_Tags::_DOCTYPE) + '\n';
		Final += string(_Html_Tags::_html_begin) + '\n';
	}
	if (find(args.begin(), args.end(), "-geochronology") != args.end()) { GeochronologyCall(args); }
	//else if (find(args.begin(), args.end(), "-mineralogy") != args.end()) { minerals(SystemIn, args); }
	else
	{
		// Parse File
		cout << "Loading File...\n";
		ISCStream SystemIn;
		try { SystemIn = ParseFile(args[1], srcencoding); }
		catch (exception e)
		{
			cout << e.what();
			abort();
		}
		if (find(args.begin(), args.end(), "-astrobiology") != args.end()) { Astrobiology = true; }
		NormalProcess(SystemIn, args);
	}
	if (OFormat == HTML) {Final += string(_Html_Tags::_html_end) + '\n';}

	// Transcode

	Transcode(Final, outencoding);

	// Output file

	ofstream fout(OutputFileName);
	fout << Final;
	fout.close();

	cout << "File is output at " + OutputFileName << '\n';

	exit(0);
}
