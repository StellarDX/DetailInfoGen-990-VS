#include "composite.h"
#include "final.h"

#include "../composite.h"
#include "../composite2.h"
#include "../final.h"

#include <fstream>
#include <CSE/Random.h>

using namespace std;
using namespace cse;

vector<string> OreDict;

void ParseOreDict(string Filename, vector<string>* Dest, int encod = 65001)
{
	cout << "Loading ore dictionary...\n";
	ifstream fin(Filename);
	if (!fin) { abort(); }
	string buf;
	while (getline(fin, buf, '\n'))
	{
		// Skip comment
		auto buf2 = buf;
		buf.clear();
		for (size_t i = 0; i < buf2.size(); i++)
		{
			if (buf2[i] == '/' && buf2.substr(i, 2) == "//"){break;}
			buf += buf2[i];
		}

		// Skip white space
		buf.erase(0, buf.find_first_not_of(" \t\n\r"));
		buf.erase(buf.find_last_not_of(" \t\n\r") + 1);

		Transcode(buf, encod);

		if (!buf.empty()) { OreDict.push_back(buf); }
	}
	cout << vformat("{} Minerals loaded.\n", make_format_args(OreDict.size()));
}

void LoadSystem(ISCStream& SystemIn)
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
}

void minerals(ISCStream& SystemIn, vector<string> args)
{
	bool CustomOreDict = false;
	string OreDictPath = "InfoGen_Data/minerals/OreDict.tbl";

	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i] == "-oredict" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid ore dictionary." << '\n';
				abort();
			}
			OreDictPath = args[i + 1];
			CustomOreDict = true;
			break;
		}
	}

	int encoding = 65001; // Default encoding is UTF-8
	for (size_t i = 0; i < args.size() && CustomOreDict; i++)
	{
		if (args[i].substr(0, 14) == "-oredictencod=")
		{
			string encodstr = args[i];
			encoding = stoi(encodstr.substr(14, encodstr.size() - 14));
			break;
		}
	}

	string MPSStr = "diameter";
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 17) == "-minorplanetsort=")
		{
			string encodstr = args[i];
			MPSStr = encodstr.substr(17, encodstr.size() - 17);
			break;
		}
	}

	if (MPSStr == "diameter") { MinorPlanetSortArg = MPS_Diam; }
	else if (MPSStr == "mass") { MinorPlanetSortArg = MPS_Mass; } // Only mass and radius valid in this scope
	
	ParseOreDict(OreDictPath, &OreDict, encoding);
	LoadSystem(SystemIn);
	SortSystemType(System);

	composite0min();
}