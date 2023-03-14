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

#include "gbuffer_html.h"

#include "composite.h"
#include "final.h"
#include <sstream>

using namespace std;
using namespace cse;

// Default Localization strings
namespace Localization
{
	static string C0_Title1   = "System Information";
	static string C0_Age      = "Age (Years)";
	static string C0_Mass     = "System mass (Kg)";
	static string C0_SMAOP    = "Semi-major axis of outer known planet (m)";
	static string C0_Stars    = "Stars";
	static string C0_Planets  = "Known planets";
	static string C0_DPlanets = "Known dwarf planets";
	static string C0_NSatel   = "Known natural satellites";
	static string C0_MPs      = "Known minor planets";
	static string C0_Comets   = "Known comets";
	static string C0_RSatel   = "Identified rounded satellites";
	static string C0_SpType   = "Spectral type";
}

using namespace Localization;

// Indices
map<string, vector<size_t>> TypeIndices;
map<string, vector<size_t>> Companions;
string SystemBarycenter;

string HTMLhead;
string HTMLcontent;
string HTMLMenu;

// -------------------- Functions -------------------- //

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

void FormatOutputSysInfo(ostream& os, string fmtstring, string ifmtstring, const SystemInfo& Info)
{
	os << vformat(fmtstring, make_format_args(C0_Age, Info.Age));
	os << vformat(fmtstring, make_format_args(C0_Mass, Info.Mass));
	os << vformat(fmtstring, make_format_args(C0_SMAOP, Info.DistOutPlanet));
	os << vformat(ifmtstring, make_format_args(C0_Stars, Info.NStars));
	os << vformat(ifmtstring, make_format_args(C0_Planets, Info.NPlanets));
	os << vformat(ifmtstring, make_format_args(C0_DPlanets, Info.NDPlanets));
	os << vformat(ifmtstring, make_format_args(C0_NSatel, Info.NSatellites));
	os << vformat(ifmtstring, make_format_args(C0_MPs, Info.NMPlanets));
	os << vformat(ifmtstring, make_format_args(C0_Comets, Info.NComets));
	os << vformat(ifmtstring, make_format_args(C0_RSatel, Info.NRSatellites));
	os << vformat(ifmtstring, make_format_args(C0_SpType, Info.SpType));
}

// HTML
string HTMLHeadOutput()
{
	ObjectBuffer::iterator SystemBarycen;
	try { SystemBarycen = FindSystemBarycenter(System); }
	catch (exception e)
	{
		cout << e.what();
		abort();
	}

	ostringstream fout;
	fout << MakeHTMLHead(SystemBarycen->Name[0], CSSPath, CopyCSS);
	SystemBarycenter = SystemBarycen->Name[0];

	return fout.str();
}

string HTMLTitleOutput()
{
	ostringstream fout;
	SystemInfo Info = gbuffer_basic(System, TypeIndices);
	fout << "\t\t\t" << _Html_Tags::_h1_begin << SystemBarycenter << _Html_Tags::_h1_end << '\n';
	fout << "\t\t\t" << "<table border=\"1\">" << '\n';

	string fmtstring = "\t\t\t\t<tr class = \"MainTable\"><td class = \"MainTableHead\">{}</td><td class = \"MainTableData\">{:." + to_string(_OUT_PRECISION) + "g}</td></tr>\n";
	string ifmtstring = "\t\t\t\t<tr class = \"MainTable\"><td class = \"MainTableHead\">{}</td><td class = \"MainTableData\">{}</td></tr>\n";
	fout << vformat("\t\t\t\t<tr class = \"MainTable\"><td class = \"MainTableTitle\" colspan = 2 align=\"center\">{}</td></tr>\n", make_format_args(C0_Title1));
	FormatOutputSysInfo(fout, fmtstring, ifmtstring, Info);

	fout << "\t\t\t" << _Html_Tags::_table_end << '\n';

	return fout.str();
}

// GH MarkDown
string GHMDSystemInfoTable(SystemInfo Info)
{
	ostringstream os;
	os << "| | |\n|:---|:---|\n";
	string fmtstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	string ifmtstring = "| {} | {} |\n";
	FormatOutputSysInfo(os, fmtstring, ifmtstring, Info);
	return os.str();
}

string GHMarkDownOutput()
{
	ObjectBuffer::iterator SystemBarycen;
	try { SystemBarycen = FindSystemBarycenter(System); }
	catch (exception e)
	{
		cout << e.what();
		abort();
	}

	ostringstream fout;
	fout << vformat("# {}\n", make_format_args(SystemBarycen->Name[0]));

	SystemInfo Info = gbuffer_basic(System, TypeIndices);
	fout << "\n ## " + C0_Title1 + "\n" << GHMDSystemInfoTable(Info);
	SystemBarycenter = SystemBarycen->Name[0];

	return fout.str();
}

void GetLcString(string Key, string* Val)
{
	auto it = LocStrings.find(Key);
	if (it == LocStrings.end())
	{
		cout << vformat("String \"{}\"(Default Value = \"{}\") is not found in localization file, using default value.\n", make_format_args(Key, *Val));
		return;
	}
	*Val = it->second;
}

void GetLocaleComp0()
{
	GetLcString("C0_Title",        &C0_Title1);
	GetLcString("C0_Age",          &C0_Age);
	GetLcString("C0_Mass",         &C0_Mass);
	GetLcString("C0_SMAOP",        &C0_SMAOP);
	GetLcString("C0_Stars",        &C0_Stars);
	GetLcString("C0_Planets",      &C0_Planets);
	GetLcString("C0_DwarfPlanets", &C0_DPlanets);
	GetLcString("C0_Satellites",   &C0_NSatel);
	GetLcString("C0_MinorPlanets", &C0_MPs);
	GetLcString("C0_Comets",       &C0_Comets);
	GetLcString("C0_RSatellites",  &C0_RSatel);
	GetLcString("C0_SpecClass",    &C0_SpType);
}

void GetLocaleStar();
void GetLocalePlanet();

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void composite()
{
	GetLocaleComp0();
	GetLocaleStar();
	GetLocalePlanet();
	cout << "Loading - Initializing object phase 2...\n";

	SortSystemType(System);
	SortParentBody(System);

	switch (OFormat)
	{
	case HTML:
		if (!Astrobiology)
		{
			HTMLhead = HTMLHeadOutput();
			HTMLcontent = HTMLTitleOutput();
		}
		else { HTMLHeadOutput(); }
		break;
	case MD:
	default:
		if (!Astrobiology){Final = GHMarkDownOutput();}
		else { GHMarkDownOutput(); }
		break;
	}
}