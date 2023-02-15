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
#include "final.h"
#include <sstream>

using namespace std;
using namespace cse;

// Indices
map<string, vector<size_t>> TypeIndices;
map<string, vector<size_t>> Companions;

void SortSystemType(ObjectBuffer Sys)
{
	// Types
	for (size_t i = 0; i < Sys.size(); i++)
	{
		auto it = TypeIndices.find(Sys[i].Type);
		auto end = TypeIndices.end();
		if (it != end)
		{
			it->second.push_back(i);
		}
		else {TypeIndices.insert(pair(Sys[i].Type, vector<size_t>({i})));}
	}
}

void SortParentBody(ObjectBuffer Sys)
{
	for (size_t i = 0; i < Sys.size(); i++)
	{
		auto it = Companions.find(Sys[i].ParentBody);
		auto end = Companions.end();
		if (it != end)
		{
			it->second.push_back(i);
		}
		else { Companions.insert(pair(Sys[i].ParentBody, vector<size_t>({i}))); }
	}
}

ObjectBuffer::iterator FindSystemBarycenter(ObjectBuffer& Sys)
{
	auto it = Sys.begin();
	auto end = Sys.end();
	while (it != end)
	{
		for (size_t i = 0; i < it->Name.size(); i++)
		{
			if (it->ParentBody == it->Name[i])
			{
				return it;
			}
		}
		++it;
	}
	throw exception("System barycenter is not found.");
}

// GH MarkDown
string SystemInfoTable(SystemInfo Info)
{
	ostringstream os;
	os << "| | |\n|:---|:---|\n";
	string fmtstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	string ifmtstring = "| {} | {} |\n";
	os << vformat(fmtstring, make_format_args("Age (Years)", Info.Age));
	os << vformat(fmtstring, make_format_args("System mass (Kg)", Info.Mass));
	os << vformat(fmtstring, make_format_args("Semi-major axis of outer known planet (m)", Info.DistOutPlanet));
	os << vformat(ifmtstring, make_format_args("Stars", Info.NStars));
	os << vformat(ifmtstring, make_format_args("Known planets", Info.NPlanets));
	os << vformat(ifmtstring, make_format_args("Known dwarf planets", Info.NDPlanets));
	os << vformat(ifmtstring, make_format_args("Known natural satellites", Info.NSatellites));
	os << vformat(ifmtstring, make_format_args("Known minor planets", Info.NMPlanets));
	os << vformat(ifmtstring, make_format_args("Known comets", Info.NComets));
	os << vformat(ifmtstring, make_format_args("Identified rounded satellites", Info.NRSatellites));
	os << vformat(ifmtstring, make_format_args("Spectral type", Info.SpType));

	return os.str();
}

string GHMarkDownOutput()
{
	ObjectBuffer::iterator SystemBarycenter;
	try { SystemBarycenter = FindSystemBarycenter(System); }
	catch (exception e)
	{
		cout << e.what();
		abort();
	}

	ostringstream fout;
	fout << vformat("# {}\n", make_format_args(SystemBarycenter->Name[0]));

	SystemInfo Info = gbuffer_basic(System, TypeIndices);
	fout << "\n## System Information\n" << SystemInfoTable(Info);

	return fout.str();
}

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void composite()
{
	cout << "Loading - Initializing object phase 2...\n";

	SortSystemType(System);
	SortParentBody(System);

	switch (OFormat)
	{
	case HTML:
		cout << "HTML format output is not support just now, maybe support in the future versions.\n";
		exit(0xFFFFFFFF);
		break;
	case MD:
	default:
		Final = GHMarkDownOutput();
		break;
	}
}