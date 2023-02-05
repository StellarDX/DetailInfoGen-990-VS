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
	vector<string> args;
	for (size_t i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}
	cout << "Loading File...\n";

	string FileName = "RS 1236-1389-4-2925-2839";
	ISCStream SystemIn;
	try{SystemIn = ParseFile(FileName + ".sc");}
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

	if (find(args.begin(), args.end(), "-html") != args.end()) { OFormat = HTML; }
	else if (find(args.begin(), args.end(), "-md") != args.end()) { OFormat = MD; }

	composite(argc, argv);
	composite1(argc, argv);

	int encoding = 65001; // Default encoding is UTF-8
	for (size_t i = 0; i < argc; i++)
	{
		if (string(argv[i]).substr(0, 10) == "-encoding=")
		{
			string encodstr = string(argv[i]);
			encoding = stoi(encodstr.substr(11, encodstr.size() - 11));
		}
	}
	Transcode(Final, encoding);

	ofstream fout(FileName + ".md");
	fout << Final;
	fout.close();

	exit(0);
}
