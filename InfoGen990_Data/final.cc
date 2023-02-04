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

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const* argv[]) // main function can return "void" in C++20, WHY???
{
	cout << "Loading File...\n";

	string FileName = "RS 1236-3145-0-0-548";
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

	if (argc == 3 && "-html" == string(argv[3])) { OFormat = HTML; }
	else if (argc == 3 && "-md" == string(argv[3])) { OFormat = MD; }

	composite(argc, argv);
	composite1(argc, argv);

	ofstream fout(FileName + ".md");
	fout << Final;
	fout.close();

	exit(0);
}
