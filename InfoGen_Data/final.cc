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
#include "final.h"
#include <fstream>
#include <yvals_core.h> // STD version header

#include "geochronology/final.h"
#include "mineralogy/final.h"

using namespace std;
using namespace cse;

ObjectBuffer System;
string Final;

enum OutputFormat OFormat = MD;
bool Astrobiology = false;
uint32_t _OUT_PRECISION = 12;

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

void NormalProcess(ISCStream& SystemIn, vector<string> args)
{
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
	if (Astrobiology){composite3();}
}

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const* argv[]) // main function can return "void" in C++20, WHY???
{
	cout << "SpaceEngine 0.990 Detailed information generator\nCopyright (C) StellarDX Astronomy.\n";
	cout << "This is a free software, licenced under GNU General public license v2.\n";
	cout << "Built with " + COMPILER_VERSION << '\n' << '\n';

	if (argc == 1)
	{
		cout << "Usage: \n";
		cout << "\033[1m\t\033[35mInfoGen \033[36m<filename> \033[31m<args...>\n\033[0m\n";

		cout << "Additional arguments: \n\n";
		cout << "\t\033[32m-outformat=<fmt> \033[0m: Specify output format, supports Github Markdown(md).\n";
		cout << "\t\033[32m-encoding=<encod> \033[0m: Specify encoding, use numbers of codepage, e.g. -encoding=0 -> ANSI.\n";
		cout << "\t\033[32m-out <filename> \033[0m: Specify output file name.\n";
		cout << "\t\033[32m-genseed=<seed> \033[0m: Specify seed of random generator, using Hexadecimal(0 - FFFFFFFF), it will randomly generated when argument is missing.\n";
		cout << "\t\033[32m-precision=<int> \033[0m: Specify output precision, default is 12.\n";

		cout << "\t\033[32m-minorplanetsort=<char> \033[0m: Generate a list of minor planets(include dwarf planets) that are exceptional in some way. The following values are valid:\n";
		cout << "\t\t\033[33mdiameter \033[0m- IRAS standard, asteroids with a diameter greater than 120 Km (This is default option when argument is missing or invalid.)\n";
		cout << "\t\t\033[33mmass \033[0m- Asteroids with nominal mass > 1E+18 kg\n";
		cout << "\t\t\033[33mslowrotator \033[0m- slowest-rotating known minor planets with a period of at least 1000 hours\n";
		cout << "\t\t\033[33mfastrotator \033[0m- fastest-rotating minor planets with a period of less than 100 seconds\n";
		cout << "\t\t\033[33mretrograde \033[0m- Minor planets with orbital inclinations greater than 90 deg\n";
		cout << "\t\t\033[33mhighinclined \033[0m- Minor planets with orbital inclinations greater than 10 deg and smaller than 90 deg\n";
		cout << "\t(Only mass and diameter is valid when proccessing mode is switched to mineral generation.)\n";
		cout << "\n";
		cout << "\t\033[32m-astrobiology \033[0m: Calculate ESI and Habitabilities for rocky planets and report will be displayed on the bottom of the file.\n";
		cout << "\n";
		cout << "\t\033[32m-geochronology \033[0m: Generate timeline of the evolutionary history of a life planet, the following options are available.\n";
		cout << "\t\t\033[33m-target <ObjectName> \033[0m: Specify target object.\n";
		cout << "\t\t\033[33m-parent <ObjectName> \033[0m: Specify parent body, it will automatically detected when missing.\n";
		cout << "\n";
		cout << "\t\033[32m-mineralogy \033[0m: Generate mineral distribution of all the rocky objects in this system, the following options are available.\n";
		cout << "\t\t\033[33m-oredict <filename> \033[0m: Specify custom ore dictionary file, program will use default ore dictionary when this argument is missing.\n";
		cout << "\t\t\033[33m-oredictencod=<encod> \033[0m: Specify file encoding of custom ore dictionary, default is 65001(UTF-8).\n";

		return 0x7FFFFFFF;
	}

	vector<string> args;
	for (size_t i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

	// Output format
	if (find(args.begin(), args.end(), "-outformat=html") != args.end()) { OFormat = HTML; }
	else if (find(args.begin(), args.end(), "-outformat=md") != args.end()) { OFormat = MD; }

	// Seed
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 9) == "-genseed=")
		{
			string encodstr = args[i];
			random.seed(stoull(encodstr.substr(9, encodstr.size() - 9), nullptr, 16));
			break;
		}
	}

	// Output precision
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 11) == "-precision=")
		{
			string encodstr = args[i];
			_OUT_PRECISION = stoi(encodstr.substr(11, encodstr.size() - 11));
			break;
		}
	}

	// Parse File

	cout << "Loading File...\n";

	//string FileName = "RS 0-5-21663-1051-3833-8-4038573-225";
	ISCStream SystemIn;
	//try{SystemIn = ParseFile(FileName + ".sc");}
	if (args[1].substr(args[1].size()-3, 3) != ".sc")
	{
		args[1] += ".sc";
	}
	try {SystemIn = ParseFile(args[1]); }
	catch (exception e)
	{
		cout << e.what();
		abort();
	}

	// Processing

	if (find(args.begin(), args.end(), "-geochronology") != args.end()) { geochronology(SystemIn, args); }
	else if (find(args.begin(), args.end(), "-mineralogy") != args.end()) { minerals(SystemIn, args); }
	else
	{
		if (find(args.begin(), args.end(), "-astrobiology") != args.end()) { Astrobiology = true; }
		NormalProcess(SystemIn, args);
	}

	// Transcode

	int encoding = 65001; // Default encoding is UTF-8
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 10) == "-encoding=")
		{
			string encodstr = args[i];
			encoding = stoi(encodstr.substr(10, encodstr.size() - 10));
			break;
		}
	}
	Transcode(Final, encoding);

	// Output file

	string OutputFileName;
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

	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i] == "-out" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid output filename." << '\n';
				abort();
			}
			OutputFileName = args[i + 1];
			break;
		}
	}

	ofstream fout(OutputFileName);
	fout << Final;
	fout.close();

	exit(0);
}
