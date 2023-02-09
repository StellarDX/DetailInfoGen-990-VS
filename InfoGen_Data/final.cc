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
#include "final.h"
#include <fstream>

using namespace std;
using namespace cse;

ObjectBuffer System;
string Final;

enum OutputFormat OFormat = MD;

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

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const* argv[]) // main function can return "void" in C++20, WHY???
{
	cout << "SpaceEngine 0.990 Detailed information generator\nCopyright (C) StellarDX Astronomy.\n";
	cout << "This is a free software, licenced under GNU General public license v2.\n\n";
	if (argc == 1)
	{
		cout << "Usage: \n";
		cout << "InfoGen <filename> <args...>\n\n";

		cout << "Additional arguments: \n";
		cout << "-outformat=<fmt> : Specify output format, supports Github Markdown(md).\n";
		cout << "-encoding=<encod> : Specify encoding, use numbers of codepage, e.g. -encoding=0 -> ANSI.\n";
		cout << "-out <filename> : Specify output file name.\n";

		cout << "-minorplanetsort=<char> : Generate a list of minor planets(include dwarf planets) that are exceptional in some way. The following values are valid:\n";
		cout << "\tdiameter - IRAS standard, asteroids with a diameter greater than 120 Km (This is default option when argument is missing or invalid.)\n";
		cout << "\tmass - Asteroids with nominal mass > 1E+18 kg\n";
		cout << "\tslowrotator - slowest-rotating known minor planets with a period of at least 1000 hours\n";
		cout << "\tfastrotator - fastest-rotating minor planets with a period of less than 100 seconds\n";
		cout << "\tretrograde - Minor planets with orbital inclinations greater than 90 deg\n";
		cout << "\thighinclined - Minor planets with orbital inclinations greater than 10 deg and smaller than 90 deg\n";
		return 0x7FFFFFFF;
	}
	vector<string> args;
	for (size_t i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

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

	cout << "Loading - Initializing object phase 1...\n";

	auto it = SystemIn->begin();
	auto end = SystemIn->end();
	while (it != end)
	{
		System.push_back(ObjectLoader(it));
		++it;
	}

	cout << vformat("{} Objects loaded.\n", make_format_args(System.size()));

	if (find(args.begin(), args.end(), "-outformat=html") != args.end()) { OFormat = HTML; }
	else if (find(args.begin(), args.end(), "-outformat=md") != args.end()) { OFormat = MD; }

	composite(argc, argv);
	composite1(argc, argv);
	composite2(argc, argv);

	int encoding = 65001; // Default encoding is UTF-8
	for (size_t i = 0; i < args.size(); i++)
	{
		if (string(argv[i]).substr(0, 10) == "-encoding=")
		{
			string encodstr = args[i];
			encoding = stoi(encodstr.substr(10, encodstr.size() - 10));
			break;
		}
	}
	Transcode(Final, encoding);

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
		if (args[i] == "-out" && i < args.size() - 1)
		{
			OutputFileName = args[i + 1];
			break;
		}
	}

	ofstream fout(OutputFileName);
	fout << Final;
	fout.close();

	exit(0);
}
