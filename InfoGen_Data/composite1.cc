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

#include "partition/partition.h"
#include "gbuffer_star.h"
#include "gbuffer_planet.h"
#include "gbuffer_atmosphere.h"
#include "gbuffer_water.h"
#include "gbuffer_html.h"
#include "composite.h"
#include "composite1.h"

using namespace std;
using namespace cse;

// Default localization strings

namespace Localization
{
	static string C1_Title2          = "Objects";
	static string C1_Orbit_Title     = "Orbital characteristics";
	static string C1_Orbit_Epoch     = "Epoch";
	static string C1_Orbit_RefPlane  = "Reference System";
	static string C1_Orbit_Prim      = "Primary";
	static string C1_Orbit_Comp      = "Companion";
	static string C1_Orbit_ApH       = "Aphelion (m)";
	static string C1_Orbit_PeriH     = "Perihelion (m)";
	static string C1_Orbit_ApG       = "Apogee (m)";
	static string C1_Orbit_PeriG     = "Perigee (m)";
	static string C1_Orbit_Apoapsis  = "Apoapsis (m)";
	static string C1_Orbit_Periapsis = "Periapsis (m)";
	static string C1_Orbit_a         = "Semi-major axis (a) (m)";
	static string C1_Orbit_e         = "Eccentricity (e)";
	static string C1_Orbit_P1        = "Orbital period (sidereal) (P) (s)";
	static string C1_Orbit_P2        = "Orbital period (sidereal) (P) (s)";
	static string C1_Orbit_P3        = "Orbital period (sidereal) (P) (s)";
	static string C1_Orbit_sp        = "Orbital period (synodic) (s)";
	static string C1_Orbit_M0        = "Mean anomaly";
	static string C1_Orbit_i         = "Inclination (i)";
	static string C1_Orbit_ome       = "Longitude of ascending node (Ω)";
	static string C1_Orbit_w1        = "Argument of periastron (ω) (secondary)";
	static string C1_Orbit_w2        = "Argument of perihelion (ω)";
	static string C1_Orbit_w3        = "Argument of perigee (ω)";
	static string C1_Orbit_w4        = "Argument of periapsis (ω)";
	static string C1_Physical        = "Physical characteristics";
	static string C1_Bar_Type        = "System Barycenter";
	static string C1_Star_MagBol     = "Bolometric magnitude";
	static string C1_Star_Class      = "Spectral classification";
	static string C1_Star_EqRadius   = "Equatorial radius (m)";
	static string C1_Star_EqCircum   = "Equatorial circumference (m)";
	static string C1_Star_Oblate     = "Flattening";
	static string C1_Star_SurfArea   = "Surface area (m^2)";
	static string C1_Star_Volume     = "Volume (m^3)";
	static string C1_Star_Mass       = "Mass (Kg)";
	static string C1_Star_Density    = "Average density (Kg/m^3)";
	static string C1_Star_SurfGrav   = "Equatorial surface gravity (m/s^2)";
	static string C1_Star_EscVel     = "Escape velocity (from the surface) (m/s)";
	static string C1_Star_Teff       = "Temperature (°K)";
	static string C1_Star_Lum        = "Luminosity (W)";
	static string C1_Star_Obliq      = "Obliquity";
	static string C1_Star_RP         = "RotationPeriod (s)";
	static string C1_BH_SRadius      = "Schwarzschild Radius (m)";
	static string C1_BH_KerrSpin     = "Angular momentum";
	static string C1_BH_KerrCharge   = "Electric charge";
	static string C1_Planet_MRad     = "Mean radius (m)";
	static string C1_Planet_ERad     = "Equatorial radius (m)";
	static string C1_Planet_PRad     = "Polar radius (m)";
	static string C1_Planet_Oblate   = "Flattening";
	static string C1_Planet_ECircum  = "Equatorial Circumference (m)";
	static string C1_Planet_MCircum  = "Meridional Circumference (m)";
	static string C1_Planet_SurfArea = "Surface area (m^2)";
	static string C1_Planet_Volume   = "Volume (m^3)";
	static string C1_Planet_Mass     = "Mass (Kg)";
	static string C1_Planet_Density  = "Mean density (Kg/m^3)";
	static string C1_Planet_SurfGrav = "Surface gravity (m/s^2)";
	static string C1_Planet_EscVel   = "Escape velocity (m/s)";
	static string C1_Planet_SynPer   = "Synodic rotation period (s)";
	static string C1_Planet_SidPer   = "Sidereal rotation period (s)";
	static string C1_Planet_Obliq    = "Axial tilt";
	static string C1_Planet_AlbedoB  = "Albedo (Bond)";
	static string C1_Planet_AlbedoG  = "Albedo (geometric)";
	static string C1_Planet_Temp     = "Surface temperature (Approximate value, only for reference)";
	static string C1_Planet_TempMin  = "Min";
	static string C1_Planet_TempMean = "Mean";
	static string C1_Planet_TempMax  = "Max";
	static string C1_Planet_Atm      = "Atmosphere";
	static string C1_Planet_Pressure = "Surface pressure (Pa)";
	static string C1_Planet_AtmComp  = "Composition by volume";
	static string C1_Planet_Ocean    = "Ocean";
	static string C1_Planet_OceanDep = "Depth (m)";
	static string C1_Planet_OcComp   = "Composition by volume";
	static string C1_SubSys_Title1   = "Compositions";
	static string C1_SubSys_Title2   = "Sub-satellites";
	static string C1_SubSys_Title3   = "Satellites";
	static string C1_SubSys_Name     = "Name";
	static string C1_SubSys_Diam     = "Diameter(m)";
	static string C1_SubSys_Mass     = "Mass(Kg)";
	static string C1_SubSys_a        = "Semi-Major Axis(m)";
	static string C1_SubSys_P        = "Orbital Period(s)";
	static string C1_SubSys_i        = "Inclination";
	static string C1_SubSys_e        = "Eccentricity";
}

using namespace Localization;

// Internal variables and Buffers

shared_ptr<SystemStruct> SystemStructure;
stack<SystemStruct::Elem> ParentBodyStack;
shared_ptr<SystemStruct> CurrentSubSystem;
float64 SynodicPeriodBuffer;

// -------------------- Functions -------------------- //

void HTMLPushMenu(string ObjName, string ObjType)
{
	ostringstream os;
	string NSpace;
	for (size_t i = 0; i < ParentBodyStack.size(); i++)
	{
		NSpace += "&nbsp&nbsp";
	}
	if (ObjType.empty()) { os << vformat("\t\t\t{1}<a href = \"#{0}\">{0}</a>", make_format_args(ObjName, NSpace)); }
	else { os << vformat("\t\t\t{2}<a href = \"#{0}\">{0}</a> - {1}", make_format_args(ObjName, ObjType, NSpace)); }
	os << _Html_Tags::_html_endl << '\n';
	HTMLMenu += os.str();
}

void __BFS_Build(vector<shared_ptr<SystemStruct::Elem>>& Src, shared_ptr<SystemStruct>& Dst)
{
	queue<SystemStruct::Elem*> Buffer;
	for (size_t i = 0; i < Dst->Catalog[0].SubSystem->Catalog.size(); i++)
	{
		Buffer.push(&Dst->Catalog[0].SubSystem->Catalog[i]);
	}

	while (!Buffer.empty())
	{
		SystemStruct::Elem* Current = Buffer.front();
		Buffer.pop();

		auto it = Src.begin();
		auto end = Src.end();
		while (it != end)
		{
			if (Current->Pointer->Name == (*it)->Pointer->Name && Current->Pointer->ParentBody == (*it)->Pointer->ParentBody)
			{
				break;
			}
			++it;
		}
		if (it == end) { continue; }

		Current->SubSystem = (*it)->SubSystem;
		for (size_t i = 0; i < Current->SubSystem->Catalog.size(); i++)
		{
			Buffer.push(&Current->SubSystem->Catalog[i]);
		}
	}
}

shared_ptr<SystemStruct> BuildTree(map<string, vector<size_t>> CompanionBuffer)
{
	// Convert parent body name string to index
	map<size_t, vector<size_t>> ParentIndices;
	auto it = CompanionBuffer.begin();
	auto end = CompanionBuffer.end();
	while (it != end)
	{
		size_t ParentIdx;
		bool Found = false;
		for (size_t i = 0; i < System.size() && !Found; i++)
		{
			for (size_t j = 0; j < System[i].Name.size() && !Found; j++)
			{
				if (System[i].Name[j] == it->first)
				{
					ParentIdx = i;
					Found = true;
				}
			}
		}
		if (!Found)
		{
			throw exception(("ParentBody \"" + it->first + "\" is nof found.").c_str());
		}

		if (ParentIndices.find(ParentIdx) != ParentIndices.end())
		{
			auto it2 = ParentIndices.find(ParentIdx);
			for (size_t i = 0; i < it->second.size(); i++)
			{
				it2->second.push_back(it->second[i]);
			}
		}
		else { ParentIndices.insert(pair(ParentIdx, it->second)); }
		++it;
	}

	vector<shared_ptr<SystemStruct::Elem>> Buffer;

	auto it2 = ParentIndices.begin();
	auto end2 = ParentIndices.end();

	while (it2 != end2)
	{
		SystemStruct::Elem Parent;
		Parent.Pointer = make_shared<Object>(System[it2->first]);
		SystemStruct SubSystem;
		for (size_t i = 0; i < it2->second.size(); i++)
		{
			bool Barycenter = false;
			for (size_t j = 0; j < System[it2->second[i]].Name.size(); j++)
			{
				if (System[it2->second[i]].Name[j] == System[it2->second[i]].ParentBody) { Barycenter = true; }
			}
			if (Barycenter) { continue; }
			SubSystem.Catalog.push_back(SystemStruct::Elem{ .Pointer = make_shared<Object>(System[it2->second[i]]) });
		}
		Parent.SubSystem = make_shared<SystemStruct>(SubSystem);
		Buffer.push_back(make_shared<SystemStruct::Elem>(Parent));
		++it2;
	}

	shared_ptr<SystemStruct> SysTree;

	shared_ptr<SystemStruct::Elem> BarycenterObj = nullptr;
	auto it3 = Buffer.begin();
	auto end3 = Buffer.end();
	while (it3 != end3 && BarycenterObj == nullptr)
	{
		for (size_t j = 0; j < (*it3)->Pointer->Name.size() && BarycenterObj == nullptr; j++)
		{
			if ((*it3)->Pointer->ParentBody == (*it3)->Pointer->Name[j])
			{
				BarycenterObj = *it3;
			}
		}
		if (BarycenterObj == nullptr)
		{
			++it3;
		}
	}
	if (BarycenterObj == nullptr)
	{
		throw exception("System barycenter is not found.");
	}

	Buffer.erase(it3);
	SysTree = make_shared<SystemStruct>(SystemStruct{ .Catalog = {*BarycenterObj} });
	__BFS_Build(Buffer, SysTree);

	return SysTree;
}

string HTMLGenOrbit(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "<td class = \"OrbitTableHead\" colspan = 1 >{}</td><td class = \"OrbitTableData\" colspan = {}>{:." + to_string(_OUT_PRECISION) + "g}</td>";
	string ifmtstring = "<td class = \"OrbitTableHead\" colspan = 1 >{}</td><td class = \"OrbitTableData\" colspan = {}>{}</td>";

	os << "\t\t\t\t<tr class = \"OrbitTable\"><td colspan = 6 class = \"OrbitTableSectionHeader\" align=\"center\">" + C1_Orbit_Title + "</td></tr>\n";

	// Epoch and RefPlane
	os << "\t\t\t\t<tr class = \"OrbitTable\">";
	if (isinf(Obj->Orbit.Epoch)) { Obj->Orbit.Epoch = J2000; }
	os << vformat(fmtstring, make_format_args(C1_Orbit_Epoch, 2, Obj->Orbit.Epoch));
	os << vformat(ifmtstring, make_format_args(C1_Orbit_RefPlane, 2, Obj->Orbit.RefPlane));
	os << "</tr>\n";

	// Detect binary
	if (Obj->Orbit.Binary)
	{
		os << "\t\t\t\t<tr class = \"OrbitTable\">";
		os << vformat(ifmtstring, make_format_args(C1_Orbit_Prim, 2, Obj->ParentBody));
		os << vformat(ifmtstring, make_format_args(C1_Orbit_Comp, 2, Obj->Name[0]));
		os << "</tr>\n";
	}

	// Semi-major axis and apsis
	os << "\t\t\t\t<tr class = \"OrbitTable\">";
	if (Obj->Orbit.Eccentricity == 0)
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_a, 5, Obj->Orbit.SemiMajorAxis()));
	}
	else if (Obj->Orbit.Eccentricity < 1)
	{
		if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
		{
			os << vformat(fmtstring, make_format_args(C1_Orbit_ApH, 1, Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist));
			os << vformat(fmtstring, make_format_args(C1_Orbit_PeriH, 1, Obj->Orbit.PericenterDist));
		}
		else if (ParentBodyStack.top().Pointer->Type == "Planet")
		{
			os << vformat(fmtstring, make_format_args(C1_Orbit_ApG, 1, Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist));
			os << vformat(fmtstring, make_format_args(C1_Orbit_PeriG, 1, Obj->Orbit.PericenterDist));
		}
		else
		{
			os << vformat(fmtstring, make_format_args(C1_Orbit_Apoapsis, 1, Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist));
			os << vformat(fmtstring, make_format_args(C1_Orbit_Periapsis, 1, Obj->Orbit.PericenterDist));
		}
		os << vformat(fmtstring, make_format_args(C1_Orbit_a, 1, Obj->Orbit.SemiMajorAxis()));
	}
	if (Obj->Orbit.Eccentricity >= 1)
	{
		if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
		{
			os << vformat(fmtstring, make_format_args(C1_Orbit_PeriH, 5, Obj->Orbit.PericenterDist));
		}
		else if (ParentBodyStack.top().Pointer->Type == "Planet")
		{
			os << vformat(fmtstring, make_format_args(C1_Orbit_PeriG, 5, Obj->Orbit.PericenterDist));
		}
		else
		{
			os << vformat(fmtstring, make_format_args(C1_Orbit_Periapsis, 5, Obj->Orbit.PericenterDist));
		}
	}
	os << "</tr>\n";

	// Calculate Synodic month when object is moon.
	os << "\t\t\t\t<tr class = \"OrbitTable\">";
	if (ParentBodyStack.top().Pointer->Type == "Planet")
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_P3, 2, Obj->Orbit.Period));
		auto OrbitParent = ParentBodyStack.top().Pointer->Orbit;
		if ((Obj->Orbit.Inclination > 90 && Obj->Orbit.Inclination < 270) ||
			(Obj->Orbit.Inclination < -90 && Obj->Orbit.Inclination > -270)) // Check retrograde
		{
			SynodicPeriodBuffer = (Obj->Orbit.Period * OrbitParent.Period) / (OrbitParent.Period + Obj->Orbit.Period);
			os << vformat(fmtstring, make_format_args(C1_Orbit_sp, 2, SynodicPeriodBuffer));
			SynodicPeriodBuffer *= -1;
		}
		else
		{
			SynodicPeriodBuffer = (Obj->Orbit.Period * OrbitParent.Period) / (OrbitParent.Period - Obj->Orbit.Period);
			os << vformat(fmtstring, make_format_args(C1_Orbit_sp, 2, SynodicPeriodBuffer));
		}
		if (Astrobiology)
		{
			Obj->Orbit.Period = OrbitParent.Period;
			Obj->Rotation.Obliquity = Obj->Orbit.Inclination;
		}
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"OrbitTable\">";
		os << vformat(fmtstring, make_format_args(C1_Orbit_e, 2, Obj->Orbit.Eccentricity));
		os << vformat(fmtstring, make_format_args(C1_Orbit_i, 2, Obj->Orbit.Inclination));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Star" ||
		(ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_P2, 1, Obj->Orbit.Period));
		os << vformat(fmtstring, make_format_args(C1_Orbit_e, 1, Obj->Orbit.Eccentricity));
		os << vformat(fmtstring, make_format_args(C1_Orbit_i, 1, Obj->Orbit.Inclination));
	}
	else 
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_P1, 1, Obj->Orbit.Period));
		os << vformat(fmtstring, make_format_args(C1_Orbit_e, 1, Obj->Orbit.Eccentricity));
		os << vformat(fmtstring, make_format_args(C1_Orbit_i, 1, Obj->Orbit.Inclination));
	}
	os << "</tr>\n";

	// Plane of orbit
	os << "\t\t\t\t<tr class = \"OrbitTable\">";
	os << vformat(fmtstring, make_format_args(C1_Orbit_M0, 1, Obj->Orbit.MeanAnomaly));
	os << vformat(fmtstring, make_format_args(C1_Orbit_ome, 1, Obj->Orbit.AscendingNode));
	if (Obj->Orbit.Binary)
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w1, 1, Obj->Orbit.ArgOfPericenter));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w2, 1, Obj->Orbit.ArgOfPericenter));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Planet")
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w3, 1, Obj->Orbit.ArgOfPericenter));
	}
	else
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w4, 1, Obj->Orbit.ArgOfPericenter));
	}
	os << "</tr>\n";

	return os.str();
}

string GHMarkDownGenOrbit(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	string ifmtstring = "| {} | {} |\n";
	os << '\n';
	os << " * " + C1_Orbit_Title + "\n";
	os << "| | |\n|:---|---:|\n";
	if (!isinf(Obj->Orbit.Epoch)) { os << vformat(fmtstring, make_format_args(C1_Orbit_Epoch, Obj->Orbit.Epoch)); }
	else { os << vformat(fmtstring, make_format_args(C1_Orbit_Epoch, float64(J2000))); }
	os << vformat(ifmtstring, make_format_args(C1_Orbit_RefPlane, Obj->Orbit.RefPlane));
	if (Obj->Orbit.Binary)
	{
		os << vformat(ifmtstring, make_format_args(C1_Orbit_Prim, Obj->ParentBody));
		os << vformat(ifmtstring, make_format_args(C1_Orbit_Comp, Obj->Name[0]));
	}
	if (Obj->Orbit.Eccentricity != 0)
	{
		if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
		{
			if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args(C1_Orbit_ApH, Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist)); }
			os << vformat(fmtstring, make_format_args(C1_Orbit_PeriH, Obj->Orbit.PericenterDist));
		}
		else if (ParentBodyStack.top().Pointer->Type == "Planet")
		{
			if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args(C1_Orbit_ApG, Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist)); }
			os << vformat(fmtstring, make_format_args(C1_Orbit_PeriG, Obj->Orbit.PericenterDist));
		}
		else
		{
			if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args(C1_Orbit_Apoapsis, Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist)); }
			os << vformat(fmtstring, make_format_args(C1_Orbit_Periapsis, Obj->Orbit.PericenterDist));
		}
	}
	if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args(C1_Orbit_a, Obj->Orbit.SemiMajorAxis())); }
	os << vformat(fmtstring, make_format_args(C1_Orbit_e, Obj->Orbit.Eccentricity));
	
	// Calculate Synodic month when object is moon.
	if (ParentBodyStack.top().Pointer->Type == "Planet")
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_P3, Obj->Orbit.Period));
		auto OrbitParent = ParentBodyStack.top().Pointer->Orbit;
		if ((Obj->Orbit.Inclination > 90 && Obj->Orbit.Inclination < 270) ||
			(Obj->Orbit.Inclination < -90 && Obj->Orbit.Inclination > -270)) // Check retrograde
		{
			SynodicPeriodBuffer = (Obj->Orbit.Period * OrbitParent.Period) / (OrbitParent.Period + Obj->Orbit.Period);
			os << vformat(fmtstring, make_format_args(C1_Orbit_sp, SynodicPeriodBuffer));
			SynodicPeriodBuffer *= -1;
		}
		else
		{
			SynodicPeriodBuffer = (Obj->Orbit.Period * OrbitParent.Period) / (OrbitParent.Period - Obj->Orbit.Period);
			os << vformat(fmtstring, make_format_args(C1_Orbit_sp, SynodicPeriodBuffer));
		}
		if (Astrobiology)
		{
			Obj->Orbit.Period = OrbitParent.Period;
			Obj->Rotation.Obliquity = Obj->Orbit.Inclination;
		}
	}
	else if (ParentBodyStack.top().Pointer->Type == "Star" ||
		(ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_P2, Obj->Orbit.Period));
	}
	else { os << vformat(fmtstring, make_format_args(C1_Orbit_P1, Obj->Orbit.Period)); }

	os << vformat(fmtstring, make_format_args(C1_Orbit_M0, Obj->Orbit.MeanAnomaly));
	os << vformat(fmtstring, make_format_args(C1_Orbit_i, Obj->Orbit.Inclination));
	os << vformat(fmtstring, make_format_args(C1_Orbit_ome, Obj->Orbit.AscendingNode));
	if (Obj->Orbit.Binary)
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w1, Obj->Orbit.ArgOfPericenter));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w2, Obj->Orbit.ArgOfPericenter));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Planet")
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w3, Obj->Orbit.ArgOfPericenter));
	}
	else
	{
		os << vformat(fmtstring, make_format_args(C1_Orbit_w4, Obj->Orbit.ArgOfPericenter));
	}

	return os.str();
}

string HTMLGenSubSys(shared_ptr<Object> Obj)
{
	ostringstream os;
	if (CurrentSubSystem != nullptr)
	{
		if (Obj->Type == "Barycenter") { os << "\t\t\t\t<tr class = \"SubSysTable\"><td colspan = 6 class = \"SubSysTableSectionHeader\" align=\"center\">" + C1_SubSys_Title1 + "</td></tr>\n"; }
		else if (Obj->Type == "Moon") { os << "\t\t\t\t<tr class = \"SubSysTable\"><td colspan = 6 class = \"SubSysTableSectionHeader\" align=\"center\">" + C1_SubSys_Title2 + "</td></tr>\n"; }
		else { os << "\t\t\t\t<tr class = \"SubSysTable\"><td colspan = 6 class = \"SubSysTableSectionHeader\" align=\"center\">" + C1_SubSys_Title3 + "</td></tr>\n"; }
		os << "\t\t\t\t<tr class = \"SubSysTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"SubSysTableSubHead\">" + C1_SubSys_Name + "</td><td class = \"SubSysTableSubHead\">" + C1_SubSys_Diam + "</td><td class = \"SubSysTableSubHead\">" + C1_SubSys_Mass +
			"</td><td class = \"SubSysTableSubHead\">" + C1_SubSys_a + "</td><td class = \"SubSysTableSubHead\">" + C1_SubSys_P + "</td><td class = \"SubSysTableSubHead\">" + C1_SubSys_i + "</td><td class = \"SubSysTableSubHead\">" + C1_SubSys_e + "</td></tr>\n";
		string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
		for (size_t i = 0; i < CurrentSubSystem->Catalog.size(); i++)
		{
			string NameString = CurrentSubSystem->Catalog[i].Pointer->Name[0];
			if (CurrentSubSystem->Catalog[i].Pointer->Type == "Star" || CurrentSubSystem->Catalog[i].Pointer->Type == "Planet" ||
				CurrentSubSystem->Catalog[i].Pointer->Type == "DwarfPlanet" || CurrentSubSystem->Catalog[i].Pointer->Type == "Moon")
			{
				NameString = vformat("<a href = \"#{0}\">{0}</a>", make_format_args(CurrentSubSystem->Catalog[i].Pointer->Name[0]));
			}
			if (CurrentSubSystem->Catalog[i].Pointer->Type == "Asteroid" || CurrentSubSystem->Catalog[i].Pointer->Type == "Comet")
			{
				continue;
			}
			string sfmtstr;
			if
			(
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.y == CurrentSubSystem->Catalog[i].Pointer->Dimensions.x &&
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.z == CurrentSubSystem->Catalog[i].Pointer->Dimensions.x
			)
			{
				sfmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"SubSysTableData\" colspan = 1>{}</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
				os << vformat(sfmtstr, make_format_args
				(
					NameString,
					CurrentSubSystem->Catalog[i].Pointer->Radius() * 2.,
					CurrentSubSystem->Catalog[i].Pointer->Mass,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.SemiMajorAxis(),
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Period,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Inclination,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Eccentricity
				));
			}
			else if
			(
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.y != CurrentSubSystem->Catalog[i].Pointer->Dimensions.x &&
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.z == CurrentSubSystem->Catalog[i].Pointer->Dimensions.x
			)
			{
				sfmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"SubSysTableData\" colspan = 1>{}</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + " (" + PrecStr + " × " + PrecStr + ")</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
				os << vformat(sfmtstr, make_format_args
				(
					NameString,
					CurrentSubSystem->Catalog[i].Pointer->Radius() * 2.,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.x,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.y,
					CurrentSubSystem->Catalog[i].Pointer->Mass,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.SemiMajorAxis(),
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Period,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Inclination,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Eccentricity
				));
			}
			else
			{
				sfmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"SubSysTableData\" colspan = 1>{}</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + " (" + PrecStr + " × " + PrecStr + " × " + PrecStr + ")</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td><td class = \"SubSysTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
				os << vformat(sfmtstr, make_format_args
				(
					NameString,
					CurrentSubSystem->Catalog[i].Pointer->Radius() * 2.,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.x,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.y,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.z,
					CurrentSubSystem->Catalog[i].Pointer->Mass,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.SemiMajorAxis(),
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Period,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Inclination,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Eccentricity
				));
			}
		}
		os << "\t\t\t\t</table></td></tr>\n";
	}

	return os.str();
}

string GHMarkDownGenSubSys(shared_ptr<Object> Obj)
{
	ostringstream os;
	if (CurrentSubSystem != nullptr)
	{
		if (Obj->Type == "Barycenter") { os << " * " + C1_SubSys_Title1 + "\n"; }
		else if (Obj->Type == "Moon") { os << " * " + C1_SubSys_Title2 + "\n"; }
		else { os << " * " + C1_SubSys_Title3 + "\n"; }
		os << "| " + C1_SubSys_Name + " | " + C1_SubSys_Diam + " | " + C1_SubSys_Mass + 
			" | " + C1_SubSys_a + " | " + C1_SubSys_P + " | " + C1_SubSys_i + " | " + C1_SubSys_e + 
			" |\n|:---|:---|:---|:---|:---|:---|:---|\n";
		string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
		for (size_t i = 0; i < CurrentSubSystem->Catalog.size(); i++)
		{
			if (CurrentSubSystem->Catalog[i].Pointer->Type == "Asteroid" || CurrentSubSystem->Catalog[i].Pointer->Type == "Comet")
			{
				continue;
			}
			string sfmtstr;
			if
			(
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.y == CurrentSubSystem->Catalog[i].Pointer->Dimensions.x &&
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.z == CurrentSubSystem->Catalog[i].Pointer->Dimensions.x
			)
			{
				sfmtstr = "| {} | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " |\n";
				os << vformat(sfmtstr, make_format_args
				(
					CurrentSubSystem->Catalog[i].Pointer->Name[0],
					CurrentSubSystem->Catalog[i].Pointer->Radius() * 2.,
					CurrentSubSystem->Catalog[i].Pointer->Mass,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.SemiMajorAxis(),
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Period,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Inclination,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Eccentricity
				));
			}
			else if
			(
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.y != CurrentSubSystem->Catalog[i].Pointer->Dimensions.x &&
				CurrentSubSystem->Catalog[i].Pointer->Dimensions.z == CurrentSubSystem->Catalog[i].Pointer->Dimensions.x
			)
			{
				sfmtstr = "| {} | " + PrecStr + " (" + PrecStr + " × " + PrecStr + ") | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " |\n";
				os << vformat(sfmtstr, make_format_args
				(
					CurrentSubSystem->Catalog[i].Pointer->Name[0],
					CurrentSubSystem->Catalog[i].Pointer->Radius() * 2.,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.x,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.y,
					CurrentSubSystem->Catalog[i].Pointer->Mass,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.SemiMajorAxis(),
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Period,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Inclination,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Eccentricity
				));
			}
			else
			{
				sfmtstr = "| {} | " + PrecStr + " (" + PrecStr + " × " + PrecStr + " × " + PrecStr + ") | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " | " + PrecStr + " |\n";
				os << vformat(sfmtstr, make_format_args
				(
					CurrentSubSystem->Catalog[i].Pointer->Name[0],
					CurrentSubSystem->Catalog[i].Pointer->Radius() * 2.,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.x,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.y,
					CurrentSubSystem->Catalog[i].Pointer->Dimensions.z,
					CurrentSubSystem->Catalog[i].Pointer->Mass,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.SemiMajorAxis(),
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Period,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Inclination,
					CurrentSubSystem->Catalog[i].Pointer->Orbit.Eccentricity
				));
			}
		}
	}

	return os.str();
}

string HTMLProcBar(shared_ptr<Object> Obj)
{
	ostringstream os;
	bool IsStarOrPlanet = false;
	for (size_t i = 0; i < CurrentSubSystem->Catalog.size(); i++)
	{
		if 
		(
			CurrentSubSystem->Catalog[i].Pointer->Type == "Star" || 
			CurrentSubSystem->Catalog[i].Pointer->Type == "Planet" || 
			CurrentSubSystem->Catalog[i].Pointer->Type == "DwarfPlanet" || 
			CurrentSubSystem->Catalog[i].Pointer->Type == "Moon"
		)
		{
			IsStarOrPlanet = true;
			break;
		}
	}
	if (!IsStarOrPlanet) { return ""; } // Except asteroid barycenters

	bool IsMainBarycenter = false;
	for (size_t i = 0; i < Obj->Name.size(); i++)
	{
		if (Obj->Name[i] == Obj->ParentBody)
		{
			IsMainBarycenter = true;
			break;
		}
	}
	if (IsMainBarycenter) { return ""; } // Return tmpty string if it is the main barycenter.

	if (!Astrobiology) { HTMLPushMenu(Obj->Name[0], C1_Bar_Type); }

	os << "\t\t\t" << _Html_Tags::_table_begin << '\n';
	os << vformat("\t\t\t\t<tr class = \"BarycenterTable\"><td colspan = 4 class = \"BarycenterTableTop\"><a name = \"{0}\">{0}</a></td><td colspan = 2 class = \"BarycenterTableTopEnd\" align=\"right\">{1}</td></tr>\n", make_format_args(Obj->Name[0], C1_Bar_Type));
	if (Obj->Orbit.RefPlane != NO_DATA_STRING && Obj->Orbit.RefPlane != "Static" && Obj->Orbit.RefPlane != "Fixed")
	{
		os << HTMLGenOrbit(Obj);
	}
	if (Obj->Class != "Sun") // Except star barycenters
	{
		os << HTMLGenSubSys(Obj);
	}
	os << "\t\t\t" << _Html_Tags::_table_end << "\n\t\t\t" << _Html_Tags::_html_endl << '\n';

	return os.str();
}

string GHMarkDownProcBar(shared_ptr<Object> Obj)
{
	ostringstream os;
	bool IsStarOrPlanet = false;
	for (size_t i = 0; i < CurrentSubSystem->Catalog.size(); i++)
	{
		if 
		(
			CurrentSubSystem->Catalog[i].Pointer->Type == "Star" || 
			CurrentSubSystem->Catalog[i].Pointer->Type == "Planet" || 
			CurrentSubSystem->Catalog[i].Pointer->Type == "DwarfPlanet" || 
			CurrentSubSystem->Catalog[i].Pointer->Type == "Moon"
		)
		{
			IsStarOrPlanet = true;
			break;
		}
	}
	if (!IsStarOrPlanet) { return ""; } // Except asteroid barycenters

	bool IsMainBarycenter = false;
	for (size_t i = 0; i < Obj->Name.size(); i++)
	{
		if (Obj->Name[i] == Obj->ParentBody)
		{
			IsMainBarycenter = true;
			break;
		}
	}
	if (IsMainBarycenter) { return ""; } // Return tmpty string if it is the main barycenter.

	os << vformat("\n### {} - {}\n", make_format_args(Obj->Name[0], C1_Bar_Type));
	if (Obj->Orbit.RefPlane != NO_DATA_STRING && Obj->Orbit.RefPlane != "Static" && Obj->Orbit.RefPlane != "Fixed")
	{
		os << GHMarkDownGenOrbit(Obj);
	}
	if (Obj->Class != "Sun") // Except star barycenters
	{
		os << GHMarkDownGenSubSys(Obj);
	}

	return os.str();
}

string HTMLProcStar(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "<td class = \"StarTableHead\" colspan = 1 >{}</td><td class = \"StarTableData\" colspan = {}>{:." + to_string(_OUT_PRECISION) + "g}</td>";
	string ifmtstring = "<td class = \"StarTableHead\" colspan = 1 >{}</td><td class = \"StarTableData\" colspan = {}>{}</td>";
	
	if (!Astrobiology) { HTMLPushMenu(Obj->Name[0], GenStarType(Obj)); }

	os << "\t\t\t" << _Html_Tags::_table_begin << "\n";
	os << vformat("\t\t\t\t<tr class = \"StarTable\"><td colspan = 4 class = \"StarTableTop\"><a name = \"{0}\">{0}</a></td><td colspan = 2 class = \"StarTableTopEnd\" align=\"right\">{1}</td></tr>\n", make_format_args(Obj->Name[0], GenStarType(Obj)));

	if (Obj->Orbit.RefPlane != NO_DATA_STRING && Obj->Orbit.RefPlane != "Static" && Obj->Orbit.RefPlane != "Fixed")
	{
		os << HTMLGenOrbit(Obj);
	}

	if (IsBlackHole(Obj->SpecClass)) // Pecular process for black holes
	{
		BlackHoleParams Par = BlackHolePar(Obj);
		os << "\t\t\t\t<tr class = \"StarTable\"><td colspan = 6 class = \"StarTableSectionHeader\" align=\"center\">" + C1_Physical + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_Mass, 2, Par.Mass));
		os << vformat(fmtstring, make_format_args(C1_BH_SRadius, 2, Par.SchwarzschildRadius));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_BH_KerrSpin, 2, Par.Spin));
		os << vformat(fmtstring, make_format_args(C1_BH_KerrCharge, 2, Par.Charge));
		os << "</tr>\n";
	}
	else
	{
		StarParams Params = gbuffer_star(Obj);
		os << "\t\t\t\t<tr class = \"StarTable\"><td colspan = 6 class = \"StarTableSectionHeader\" align=\"center\">" + C1_Physical + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_Lum, 2, Params.Luminosity));
		os << vformat(fmtstring, make_format_args(C1_Star_MagBol, 2, Params.AbsMagnBol));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_EqRadius, 2, Params.EqRadius));
		os << vformat(fmtstring, make_format_args(C1_Star_EqCircum, 2, Params.EqCircum));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_Oblate, 5, Params.Flattening));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_SurfArea, 2, Params.SurfArea));
		os << vformat(fmtstring, make_format_args(C1_Star_Volume, 2, Params.Volume));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_Mass, 2, Params.Mass));
		os << vformat(fmtstring, make_format_args(C1_Star_Density, 2, Params.AvgDensity));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(fmtstring, make_format_args(C1_Star_SurfGrav, 2, Params.EqSurfGrav));
		os << vformat(fmtstring, make_format_args(C1_Star_EscVel, 2, Params.EscapeVelocity));
		os << "</tr>\n";
		os << "\t\t\t\t<tr class = \"StarTable\">";
		os << vformat(ifmtstring, make_format_args(C1_Star_Class, 2, Params.SpType));
		os << vformat(fmtstring, make_format_args(C1_Star_Teff, 2, Params.Temperature));
		os << "</tr>\n";
		if (!Obj->TidalLocked)
		{
			os << "\t\t\t\t<tr class = \"StarTable\">";
			os << vformat(fmtstring, make_format_args(C1_Star_Obliq, 2, Params.Obliquity));
			os << vformat(fmtstring, make_format_args(C1_Star_RP, 2, Params.RotationPeriod));
			os << "</tr>\n";
		}
	}
	os << "\t\t\t" << _Html_Tags::_table_end << "\n\t\t\t" << _Html_Tags::_html_endl << "\n";

	return os.str();
}

string GHMarkDownProcStar(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	string ifmtstring = "| {} | {} |\n";
	os << vformat("\n### {} - {}\n", make_format_args(Obj->Name[0], GenStarType(Obj)));

	if (Obj->Orbit.RefPlane != NO_DATA_STRING && Obj->Orbit.RefPlane != "Static" && Obj->Orbit.RefPlane != "Fixed")
	{
		os << GHMarkDownGenOrbit(Obj);
	}

	if (IsBlackHole(Obj->SpecClass)) // Pecular process for black holes
	{
		BlackHoleParams Par = BlackHolePar(Obj);
		os << " * " + C1_Physical + "\n";
		os << "| | |\n|:---|---:|\n";
		os << vformat(fmtstring, make_format_args(C1_Star_Mass, Par.Mass));
		os << vformat(fmtstring, make_format_args(C1_BH_SRadius, Par.SchwarzschildRadius));
		os << vformat(fmtstring, make_format_args(C1_BH_KerrSpin, Par.Spin));
		os << vformat(fmtstring, make_format_args(C1_BH_KerrCharge, Par.Charge));
	}
	else
	{
		StarParams Params = gbuffer_star(Obj);
		os << " * " + C1_Physical + "\n";
		os << "| | |\n|:---|---:|\n";
		os << vformat(fmtstring, make_format_args(C1_Star_MagBol, Params.AbsMagnBol));
		os << vformat(ifmtstring, make_format_args(C1_Star_Class, Params.SpType));
		os << vformat(fmtstring, make_format_args(C1_Star_EqRadius, Params.EqRadius));
		os << vformat(fmtstring, make_format_args(C1_Star_EqCircum, Params.EqCircum));
		os << vformat(fmtstring, make_format_args(C1_Star_Oblate, Params.Flattening));
		os << vformat(fmtstring, make_format_args(C1_Star_SurfArea, Params.SurfArea));
		os << vformat(fmtstring, make_format_args(C1_Star_Volume, Params.Volume));
		os << vformat(fmtstring, make_format_args(C1_Star_Mass, Params.Mass));
		os << vformat(fmtstring, make_format_args(C1_Star_Density, Params.AvgDensity));
		os << vformat(fmtstring, make_format_args(C1_Star_SurfGrav, Params.EqSurfGrav));
		os << vformat(fmtstring, make_format_args(C1_Star_EscVel, Params.EscapeVelocity));
		os << vformat(fmtstring, make_format_args(C1_Star_Teff, Params.Temperature));
		os << vformat(fmtstring, make_format_args(C1_Star_Lum, Params.Luminosity));
		if (!Obj->TidalLocked)
		{
			os << vformat(fmtstring, make_format_args(C1_Star_Obliq, Params.Obliquity));
			os << vformat(fmtstring, make_format_args(C1_Star_RP, Params.RotationPeriod));
		}
	}

	return os.str();
}

#define IGNORE_GAS_GIANT_GREENHOUSE 1 // If open, greenhouse effect on gas giants will ignored

string HTMLProcPlanet(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "\t\t\t\t<tr class = \"PlanetTable\"><td class = \"PlanetTableHead\" colspan = 1 >{}</td><td class = \"PlanetTableData\" colspan = {}>{:." + to_string(_OUT_PRECISION) + "g}</td></tr>\n";
	string fmtstring2 = "<td class = \"PlanetTableHead\" colspan = 1 >{}</td><td class = \"PlanetTableData\" colspan = {}>{:." + to_string(_OUT_PRECISION) + "g}</td>";
	string ifmtstring = "\t\t\t\t<tr class = \"PlanetTable\"><td class = \"PlanetTableHead\" colspan = 1 >{}</td><td class = \"PlanetTableData\" colspan = {}>{}</td></tr>\n";
	string OrbitString = HTMLGenOrbit(Obj);
	PlanetParams Par = gbuffer_planet(Obj);
	os << "\t\t\t" << _Html_Tags::_table_begin << "\n";
	os << vformat("\t\t\t\t<tr class = \"PlanetTable\"><td colspan = 4 class = \"PlanetTableTop\"><a name = \"{0}\">{0}</a></td><td colspan = 2 class = \"PlanetTableTopEnd\" align=\"right\">{1}</td></tr>\n", make_format_args(Obj->Name[0], GenPlanetType(Obj)));

	if (!Astrobiology) { HTMLPushMenu(Obj->Name[0], GenPlanetType(Obj)); }

	if (Obj->Orbit.RefPlane != NO_DATA_STRING && Obj->Orbit.RefPlane != "Static" && Obj->Orbit.RefPlane != "Fixed")
	{
		os << OrbitString;
	}

	// Physical data
	os << "\t\t\t\t<tr class = \"PlanetTable\"><td colspan = 6 class = \"PlanetTableSectionHeader\" align=\"center\">" + C1_Physical + "</td></tr>\n";
	
	if (Par.Flattening != 0)
	{
		os << "\t\t\t\t<tr class = \"PlanetTable\">";
		os << vformat(fmtstring2, make_format_args(C1_Planet_MRad, 1, Par.MeanRadius));
		os << vformat(fmtstring2, make_format_args(C1_Planet_ERad, 1, Par.EqRadius));
		os << vformat(fmtstring2, make_format_args(C1_Planet_PRad, 1, Par.PolarRadius));
		os << "</tr>\n";
	}
	else { os << vformat(fmtstring, make_format_args(C1_Planet_MRad, 5, Par.MeanRadius)); }
	os << vformat(fmtstring, make_format_args(C1_Planet_Oblate, 5, Par.Flattening));

	os << "\t\t\t\t<tr class = \"PlanetTable\">";
	os << vformat(fmtstring2, make_format_args(C1_Planet_ECircum, 2, Par.EqCircum));
	os << vformat(fmtstring2, make_format_args(C1_Planet_MCircum, 2, Par.MeriCircum));
	os << "</tr>\n";
	os << vformat(fmtstring, make_format_args(C1_Planet_SurfArea, 5, Par.SurfArea));

	os << vformat(fmtstring, make_format_args(C1_Planet_Volume, 5, Par.Volume));
	os << vformat(fmtstring, make_format_args(C1_Planet_Mass, 5, Par.Mass));
	os << vformat(fmtstring, make_format_args(C1_Planet_Density, 5, Par.MeanDensity));
	os << vformat(fmtstring, make_format_args(C1_Planet_SurfGrav, 5, Par.SurfGrav));
	os << vformat(fmtstring, make_format_args(C1_Planet_EscVel, 5, Par.EscapeVel));

	if (ParentBodyStack.top().Pointer->Type == "Planet" && Obj->TidalLocked)
	{
		Par.SynodicPeriod = SynodicPeriodBuffer;
	}
	os << "\t\t\t\t<tr class = \"PlanetTable\">";
	os << vformat(fmtstring2, make_format_args(C1_Planet_SidPer, 2, Par.RotationPeriod));
	os << vformat(fmtstring2, make_format_args(C1_Planet_SynPer, 2, Par.SynodicPeriod));
	os << "</tr>\n";

	os << vformat(fmtstring, make_format_args(C1_Planet_Obliq, 5, Par.AxialTilt));

	os << "\t\t\t\t<tr class = \"PlanetTable\">";
	os << vformat(fmtstring2, make_format_args(C1_Planet_AlbedoB, 2, Par.AlbedoBond));
	os << vformat(fmtstring2, make_format_args(C1_Planet_AlbedoG, 2, Par.AlbedoGeom));
	os << "</tr>\n";

	os << "\t\t\t\t<tr class = \"PlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
	os << "\t\t\t\t\t<tr class = \"PlanetTable\"><td class = \"PlanetTableSubHead\"></td><td class = \"PlanetTableSubHead\">" + C1_Planet_TempMin + "</td><td class = \"PlanetTableSubHead\">" + C1_Planet_TempMean + "</td><td class = \"PlanetTableSubHead\">" + C1_Planet_TempMax + "</td></tr>\n";
	string tfmtstring = "\t\t\t\t\t<tr class = \"PlanetTable\"><td class = \"PlanetTableHead\" colspan = 1 >" + C1_Planet_Temp + "</td><td class = \"PlanetTableData\" colspan = 1>{:." + to_string(_OUT_PRECISION) + "g}</td><td class = \"PlanetTableData\" colspan = 1>{:." + to_string(_OUT_PRECISION) + "g}</td><td class = \"PlanetTableData\" colspan = 1>{:." + to_string(_OUT_PRECISION) + "g}</td></tr>\n";
	vec3 Temperatures(Par.MinTemperature, Par.MeanTemperature, Par.MaxTemperature);
	if
	(
		!Obj->NoAtmosphere && !isinf(Obj->Atmosphere.Greenhouse) 
		#if IGNORE_GAS_GIANT_GREENHOUSE
		&& (Obj->Class != "Jupiter" && Obj->Class != "GasGiant" && Obj->Class != "Neptune" && Obj->Class != "IceGiant")
		#endif
	)
	{
		Temperatures += Obj->Atmosphere.Greenhouse;
	}
	os << vformat(tfmtstring, make_format_args(Temperatures.x, Temperatures.y, Temperatures.z));
	Obj->Teff = Temperatures.y;
	os << "\t\t\t\t</table></td></tr>\n";

	if (!Obj->NoAtmosphere)
	{
		Atmosphere Atm = gbuffer_atmosphere(Obj);
		os << "\t\t\t\t<tr class = \"PlanetTable\"><td colspan = 6 class = \"PlanetTableSectionHeader\" align=\"center\">" + C1_Planet_Atm + "</td></tr>\n";
		string aformatstring = "\t\t\t\t\t<tr class = \"PlanetTable\"><td class = \"PlanetTableHead\" colspan = 1 >{}</td><td class = \"PlanetTableData\" colspan = 1>{:." + to_string(_OUT_PRECISION) + "g}%</td><td class = \"PlanetTableData\" colspan = 4>{}</td></tr>\n";
		os << vformat(fmtstring, make_format_args(C1_Planet_Pressure, 5, Atm.SurfPressure));
		auto it = Atm.Composition.begin();
		auto end = Atm.Composition.end();
		while (it != end)
		{
			if (it == Atm.Composition.begin())
			{
				os << vformat(aformatstring, make_format_args(C1_Planet_AtmComp, it->second, it->first));
			}
			else
			{
				os << vformat(aformatstring, make_format_args("", it->second, it->first));
			}
			++it;
		}
	}

	if (!Obj->NoOcean)
	{
		Ocean Oc = gbuffer_water(Obj);
		os << "\t\t\t\t<tr class = \"PlanetTable\"><td colspan = 6 class = \"PlanetTableSectionHeader\" align=\"center\">" + C1_Planet_Ocean + "</td></tr>\n";
		string aformatstring = "\t\t\t\t<tr class = \"PlanetTable\"><td class = \"PlanetTableHead\" colspan = 1 >{}</td><td class = \"PlanetTableData\" colspan = 1>{:." + to_string(_OUT_PRECISION) + "g}%</td><td class = \"PlanetTableData\" colspan = 4>{}</td></tr>\n";
		os << vformat(fmtstring, make_format_args(C1_Planet_OceanDep, 5, Oc.Height));
		auto it = Oc.Composition.begin();
		auto end = Oc.Composition.end();
		while (it != end)
		{
			if (it == Oc.Composition.begin())
			{
				os << vformat(aformatstring, make_format_args(C1_Planet_OcComp, it->second, it->first));
			}
			else
			{
				os << vformat(aformatstring, make_format_args("", it->second, it->first));
			}
			++it;
		}
	}

	os << HTMLGenSubSys(Obj);

	os << "\t\t\t" << _Html_Tags::_table_end << "\n\t\t\t" << _Html_Tags::_html_endl << "\n";

	return os.str();
}

string GHMarkDownProcPlanet(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	string ifmtstring = "| {} | {} |\n";
	string OrbitString = GHMarkDownGenOrbit(Obj);
	PlanetParams Par = gbuffer_planet(Obj);
	os << vformat("\n### {} - {}\n", make_format_args(Obj->Name[0], GenPlanetType(Obj)));

	if (Obj->Orbit.RefPlane != NO_DATA_STRING && Obj->Orbit.RefPlane != "Static" && Obj->Orbit.RefPlane != "Fixed")
	{
		os << OrbitString;
	}

	os << " * " + C1_Physical + "\n";
	os << "| | |\n|:---|---:|\n";
	os << vformat(fmtstring, make_format_args(C1_Planet_MRad, Par.MeanRadius));
	if (Par.Flattening != 0)
	{
		os << vformat(fmtstring, make_format_args(C1_Planet_ERad, Par.EqRadius));
		os << vformat(fmtstring, make_format_args(C1_Planet_PRad, Par.PolarRadius));
	}
	os << vformat(fmtstring, make_format_args(C1_Planet_Oblate, Par.Flattening));
	os << vformat(fmtstring, make_format_args(C1_Planet_ECircum, Par.EqCircum));
	os << vformat(fmtstring, make_format_args(C1_Planet_MCircum, Par.MeriCircum));
	os << vformat(fmtstring, make_format_args(C1_Planet_SurfArea, Par.SurfArea));
	os << vformat(fmtstring, make_format_args(C1_Planet_Volume, Par.Volume));
	os << vformat(fmtstring, make_format_args(C1_Planet_Mass, Par.Mass));
	os << vformat(fmtstring, make_format_args(C1_Planet_Density, Par.MeanDensity));
	os << vformat(fmtstring, make_format_args(C1_Planet_SurfGrav, Par.SurfGrav));
	os << vformat(fmtstring, make_format_args(C1_Planet_EscVel, Par.EscapeVel));
	if (ParentBodyStack.top().Pointer->Type == "Planet" && Obj->TidalLocked)
	{
		Par.SynodicPeriod = SynodicPeriodBuffer;
	}
	os << vformat(fmtstring, make_format_args(C1_Planet_SynPer, Par.SynodicPeriod));
	os << vformat(fmtstring, make_format_args(C1_Planet_SidPer, Par.RotationPeriod));
	os << vformat(fmtstring, make_format_args(C1_Planet_Obliq, Par.AxialTilt));
	os << vformat(fmtstring, make_format_args(C1_Planet_AlbedoB, Par.AlbedoBond));
	os << vformat(fmtstring, make_format_args(C1_Planet_AlbedoG, Par.AlbedoGeom));

	os << " * " + C1_Planet_Temp + "\n";
	os << "| " + C1_Planet_TempMin + " | " + C1_Planet_TempMean + " | " + C1_Planet_TempMax + " |\n|:---:|:---:|:---:|\n";
	string tfmtstring = "| {:." + to_string(_OUT_PRECISION) + "g} | {:." + to_string(_OUT_PRECISION) + "g} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	vec3 Temperatures(Par.MinTemperature, Par.MeanTemperature, Par.MaxTemperature);

	if
	(
		!Obj->NoAtmosphere && !isinf(Obj->Atmosphere.Greenhouse) 
		#if IGNORE_GAS_GIANT_GREENHOUSE
		&& (Obj->Class != "Jupiter" && Obj->Class != "GasGiant" && Obj->Class != "Neptune" && Obj->Class != "IceGiant")
		#endif
	)
	{
		Temperatures += Obj->Atmosphere.Greenhouse;
	}
	os << vformat(tfmtstring, make_format_args(Temperatures.x, Temperatures.y, Temperatures.z));
	Obj->Teff = Temperatures.y;

	if (!Obj->NoAtmosphere)
	{
		Atmosphere Atm = gbuffer_atmosphere(Obj);
		os << " * " + C1_Planet_Atm + "\n";
		os << "| | |\n|:---|:---|\n";
		string aformatstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g}% {} |\n";
		os << vformat(fmtstring, make_format_args(C1_Planet_Pressure, Atm.SurfPressure));
		auto it = Atm.Composition.begin();
		auto end = Atm.Composition.end();
		while (it != end)
		{
			if (it == Atm.Composition.begin())
			{
				os << vformat(aformatstring, make_format_args(C1_Planet_AtmComp, it->second, it->first));
			}
			else
			{
				os << vformat(aformatstring, make_format_args("", it->second, it->first));
			}
			++it;
		}
	}

	if (!Obj->NoOcean)
	{
		Ocean Oc = gbuffer_water(Obj);
		os << " * " + C1_Planet_Ocean + "\n";
		os << "| | |\n|:---|:---|\n";
		string aformatstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g}% {} |\n";
		os << vformat(fmtstring, make_format_args(C1_Planet_OceanDep, Oc.Height));
		auto it = Oc.Composition.begin();
		auto end = Oc.Composition.end();
		while (it != end)
		{
			if (it == Oc.Composition.begin())
			{
				os << vformat(aformatstring, make_format_args(C1_Planet_OcComp, it->second, it->first));
			}
			else
			{
				os << vformat(aformatstring, make_format_args("", it->second, it->first));
			}
			++it;
		}
	}

	os << GHMarkDownGenSubSys(Obj);

	return os.str();
}

string HTMLProc(shared_ptr<Object> Obj)
{
	ostringstream os;
	if (Obj->Type == "Barycenter")
	{
		os << HTMLProcBar(Obj);
	}
	else if (Obj->Type == "Star")
	{
		os << HTMLProcStar(Obj);
	}
	else if (Obj->Type == "Planet" || Obj->Type == "DwarfPlanet" || Obj->Type == "Moon")
	{
		os << HTMLProcPlanet(Obj);
	}
	return os.str();
}

string GHMarkDownProc(shared_ptr<Object> Obj)
{
	ostringstream os;
	if (Obj->Type == "Barycenter")
	{
		os << GHMarkDownProcBar(Obj);
	}
	else if (Obj->Type == "Star")
	{
		os << GHMarkDownProcStar(Obj);
	}
	else if (Obj->Type == "Planet" || Obj->Type == "DwarfPlanet" || Obj->Type == "Moon")
	{
		os << GHMarkDownProcPlanet(Obj);
	}
	return os.str();
}

bool IsBinaryObject(OrbitParam Orbit1, OrbitParam Orbit2)
{
	return
	(
		cse::abs(Orbit1.Period - Orbit2.Period) < 1e-2 &&
		Orbit1.Eccentricity == Orbit2.Eccentricity &&
		Orbit1.Inclination == Orbit2.Inclination &&
		Orbit1.AscendingNode == Orbit2.AscendingNode &&
		cse::abs(Orbit1.ArgOfPericenter - Orbit2.ArgOfPericenter) - 180 < 1e-5 &&
		Orbit1.MeanAnomaly == Orbit2.MeanAnomaly
	);
}

void TransportLuminosoty(shared_ptr<Object>& Barycenter, shared_ptr<Object>& Primary, shared_ptr<Object>& Companion)
{
	if (isinf(Primary->LumBol)) { Primary->LumBol = 0; }
	if (isinf(Companion->LumBol)) { Companion->LumBol = 0; }
	Barycenter->LumBol = Primary->LumBol + Companion->LumBol;
}

void __DFS_TransLum(shared_ptr<SystemStruct> SysTree)
{
	if (SysTree == nullptr) { return; }
	for (size_t i = 0; i < SysTree->Catalog.size(); i++)
	{
		__DFS_TransLum(SysTree->Catalog[i].SubSystem);
		if (SysTree->Catalog[i].Pointer->Type == "Star" && IsStarRemnant(SysTree->Catalog[i].Pointer->SpecClass) && !SysTree->Catalog[i].Pointer->NoAccretionDisk)
		{
			SysTree->Catalog[i].Pointer->Luminosity += SysTree->Catalog[i].Pointer->AccretionDisk.Luminosity;
			SysTree->Catalog[i].Pointer->LumBol += SysTree->Catalog[i].Pointer->AccretionDisk.LumBol;
			SysTree->Catalog[i].Pointer->Teff = max(SysTree->Catalog[i].Pointer->Teff, SysTree->Catalog[i].Pointer->AccretionDisk.DiskParams1.x);
		}
		if (SysTree->Catalog[i].Pointer->Type == "Barycenter")
		{
			// Find components of system barycenter
			shared_ptr<Object> Primary = nullptr;
			shared_ptr<Object> Companion = nullptr;
			for (size_t j = 0; j < SysTree->Catalog[i].SubSystem->Catalog.size() && (Primary == nullptr || Companion == nullptr); j++)
			{
				for (size_t k = j + 1; k < SysTree->Catalog[i].SubSystem->Catalog.size() && (Primary == nullptr || Companion == nullptr); k++)
				{
					if (IsBinaryObject(SysTree->Catalog[i].SubSystem->Catalog[j].Pointer->Orbit, SysTree->Catalog[i].SubSystem->Catalog[k].Pointer->Orbit))
					{
						Primary = SysTree->Catalog[i].SubSystem->Catalog[j].Pointer;
						Companion = SysTree->Catalog[i].SubSystem->Catalog[k].Pointer;
					}
				}
			}
			if (Primary != nullptr && Companion != nullptr)
			{
				SysTree->Catalog[i].Pointer->Mass = Primary->Mass + Companion->Mass;
				if (Primary->Orbit.SemiMajorAxis() > Companion->Orbit.SemiMajorAxis())
				{
					Primary.swap(Companion);
				}
				if (!isinf(Primary->LumBol) && !isinf(Companion->LumBol))
				{
					TransportLuminosoty(SysTree->Catalog[i].Pointer, Primary, Companion);
					SysTree->Catalog[i].Pointer->Class = "Sun";
					cout << vformat("Calculated luminosity of Barycenter \"{}\" : {}", make_format_args(SysTree->Catalog[i].Pointer->Name[0], SysTree->Catalog[i].Pointer->LumBol)) << '\n';
				}
			}
		}
	}
}

void TransportOrbitData(shared_ptr<Object>& Barycenter, shared_ptr<Object>& Primary, shared_ptr<Object>& Companion)
{
	Companion->Orbit.Binary = true;
	Companion->Orbit.PericenterDist += Primary->Orbit.PericenterDist;
	Companion->ParentBody = Primary->Name[0];
	if (Primary->Type == "Star" || Primary->Type == "Barycenter")
	{
		Primary->Orbit = OrbitParam(); // Clear barycenter's data
	}
	else
	{
		Primary->Orbit = Barycenter->Orbit;
		Primary->ParentBody = Barycenter->ParentBody;
		Barycenter->Orbit = OrbitParam(); // Clear barycenter's data
	}

	if (Primary->TidalLocked && isinf(Primary->Rotation.RotationPeriod))
	{
		Primary->Rotation.RotationPeriod = Companion->Orbit.Period;
		Primary->TidalLocked = false;
	}
	if (Companion->TidalLocked && isinf(Companion->Rotation.RotationPeriod)) { Companion->Rotation.RotationPeriod = Companion->Orbit.Period; }
}

void Proccess(shared_ptr<Object> Obj)
{
	switch (OFormat)
	{
	case HTML:
		if (!Astrobiology && !PartitionReload) { HTMLcontent += HTMLProc(Obj); }
		else { HTMLProc(Obj); }
		break;
	case MD:
	default:
		if (!Astrobiology && !PartitionReload){Final += GHMarkDownProc(Obj);}
		else { GHMarkDownProc(Obj); }
		break;
	}
}

void __DFS_Iterate(shared_ptr<SystemStruct> SysTree)
{
	if (SysTree == nullptr) { return; }
	for (size_t i = 0; i < SysTree->Catalog.size(); i++)
	{
		cout << "Proccessing: " + SysTree->Catalog[i].Pointer->Name[0] << '\n';
		if (SysTree->Catalog[i].Pointer->TidalLocked && isinf(SysTree->Catalog[i].Pointer->Rotation.RotationPeriod)) { SysTree->Catalog[i].Pointer->Rotation.RotationPeriod = SysTree->Catalog[i].Pointer->Orbit.Period; } // Fill rotation period when object is tidal-locked
		if (SysTree->Catalog[i].Pointer->Type == "Barycenter")
		{
			if (SysTree->Catalog[i].Pointer->Orbit.Binary)
			{
				CurrentSubSystem = SysTree->Catalog[i].SubSystem;
				Proccess(SysTree->Catalog[i].Pointer);
			}
			// Find components of system barycenter
			shared_ptr<Object> Primary = nullptr;
			shared_ptr<Object> Companion = nullptr;
			for (size_t j = 0; j < SysTree->Catalog[i].SubSystem->Catalog.size() && (Primary == nullptr || Companion == nullptr); j++)
			{
				for (size_t k = j + 1; k < SysTree->Catalog[i].SubSystem->Catalog.size() && (Primary == nullptr || Companion == nullptr); k++)
				{
					if (IsBinaryObject(SysTree->Catalog[i].SubSystem->Catalog[j].Pointer->Orbit, SysTree->Catalog[i].SubSystem->Catalog[k].Pointer->Orbit))
					{
						Primary = SysTree->Catalog[i].SubSystem->Catalog[j].Pointer;
						Companion = SysTree->Catalog[i].SubSystem->Catalog[k].Pointer;
					}
				}
			}
			if (Primary != nullptr && Companion != nullptr)
			{
				if (Primary->Orbit.SemiMajorAxis() > Companion->Orbit.SemiMajorAxis())
				{
					Primary.swap(Companion);
				}
				TransportOrbitData(SysTree->Catalog[i].Pointer, Primary, Companion);
			}
			if (!SysTree->Catalog[i].Pointer->Orbit.Binary)
			{
				CurrentSubSystem = SysTree->Catalog[i].SubSystem;
				Proccess(SysTree->Catalog[i].Pointer);
			}
		}
		else
		{
			CurrentSubSystem = SysTree->Catalog[i].SubSystem;
			Proccess(SysTree->Catalog[i].Pointer);
		}

		ParentBodyStack.push(SysTree->Catalog[i]); // Push parent body into stack, Ignore Planet barycenter
		__DFS_Iterate(SysTree->Catalog[i].SubSystem);
		ParentBodyStack.pop();
	}
}

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetLcString(string Key, string* Val);

void GetLocaleComp1()
{
	GetLcString("C1_Title",           &C1_Title2);
	GetLcString("C1_Orbit_Title",     &C1_Orbit_Title);
	GetLcString("C1_Orbit_Epoch",     &C1_Orbit_Epoch);
	GetLcString("C1_Orbit_RefPlane",  &C1_Orbit_RefPlane);
	GetLcString("C1_Orbit_Prim",      &C1_Orbit_Prim);
	GetLcString("C1_Orbit_Comp",      &C1_Orbit_Comp);
	GetLcString("C1_Orbit_ApH",       &C1_Orbit_ApH);
	GetLcString("C1_Orbit_PeriH",     &C1_Orbit_PeriH);
	GetLcString("C1_Orbit_ApG",       &C1_Orbit_ApG);
	GetLcString("C1_Orbit_PeriG",     &C1_Orbit_PeriG);
	GetLcString("C1_Orbit_Apoapsis",  &C1_Orbit_Apoapsis);
	GetLcString("C1_Orbit_Periapsis", &C1_Orbit_Periapsis);
	GetLcString("C1_Orbit_a",         &C1_Orbit_a);
	GetLcString("C1_Orbit_e",         &C1_Orbit_e);
	GetLcString("C1_Orbit_P1",        &C1_Orbit_P1);
	GetLcString("C1_Orbit_P2",        &C1_Orbit_P2);
	GetLcString("C1_Orbit_P3",        &C1_Orbit_P3);
	GetLcString("C1_Orbit_sp",        &C1_Orbit_sp);
	GetLcString("C1_Orbit_M0",        &C1_Orbit_M0);
	GetLcString("C1_Orbit_i",         &C1_Orbit_i);
	GetLcString("C1_Orbit_ome",       &C1_Orbit_ome);
	GetLcString("C1_Orbit_w1",        &C1_Orbit_w1);
	GetLcString("C1_Orbit_w2",        &C1_Orbit_w2);
	GetLcString("C1_Orbit_w3",        &C1_Orbit_w3);
	GetLcString("C1_Orbit_w4",        &C1_Orbit_w4);
	GetLcString("C1_Physical",        &C1_Physical);
	GetLcString("C1_Star_MagBol",     &C1_Star_MagBol);
	GetLcString("C1_Star_Class",      &C1_Star_Class);
	GetLcString("C1_Star_EqRad",      &C1_Star_EqRadius);
	GetLcString("C1_Star_Oblate",     &C1_Star_Oblate);
	GetLcString("C1_Star_Volume",     &C1_Star_Volume);
	GetLcString("C1_Star_Mass",       &C1_Star_Mass);
	GetLcString("C1_Star_Density",    &C1_Star_Density);
	GetLcString("C1_Star_SurfGrav",   &C1_Star_SurfGrav);
	GetLcString("C1_Star_EscVel",     &C1_Star_EscVel);
	GetLcString("C1_Star_Teff",       &C1_Star_Teff);
	GetLcString("C1_Star_Lum",        &C1_Star_Lum);
	GetLcString("C1_Star_Obliq",      &C1_Star_Obliq);
	GetLcString("C1_Star_RP",         &C1_Star_RP);
	GetLcString("C1_BH_SRadius",      &C1_BH_SRadius);
	GetLcString("C1_BH_KerrSpin",     &C1_BH_KerrSpin);
	GetLcString("C1_BH_KerrCharge",   &C1_BH_KerrCharge);
	GetLcString("C1_Planet_MRad",     &C1_Planet_MRad);
	GetLcString("C1_Planet_ERad",     &C1_Planet_ERad);
	GetLcString("C1_Planet_PRad",     &C1_Planet_PRad);
	GetLcString("C1_Planet_Oblate",   &C1_Planet_Oblate);
	GetLcString("C1_Planet_ECircum",  &C1_Planet_ECircum);
	GetLcString("C1_Planet_MCircum",  &C1_Planet_MCircum);
	GetLcString("C1_Planet_SurfArea", &C1_Planet_SurfArea);
	GetLcString("C1_Planet_Volume",   &C1_Planet_Volume);
	GetLcString("C1_Planet_Mass",     &C1_Planet_Mass);
	GetLcString("C1_Planet_Density",  &C1_Planet_Density);
	GetLcString("C1_Planet_SurfGrav", &C1_Planet_SurfGrav);
	GetLcString("C1_Planet_EscVel",   &C1_Planet_EscVel);
	GetLcString("C1_Planet_SynPer",   &C1_Planet_SynPer);
	GetLcString("C1_Planet_SidPer",   &C1_Planet_SidPer);
	GetLcString("C1_Planet_Obliq",    &C1_Planet_Obliq);
	GetLcString("C1_Planet_AlbedoB",  &C1_Planet_AlbedoB);
	GetLcString("C1_Planet_AlbedoG",  &C1_Planet_AlbedoG);
	GetLcString("C1_Planet_Temp",     &C1_Planet_Temp);
	GetLcString("C1_Planet_TempMin",  &C1_Planet_TempMin);
	GetLcString("C1_Planet_TempMean", &C1_Planet_TempMean);
	GetLcString("C1_Planet_TempMax",  &C1_Planet_TempMax);
	GetLcString("C1_Planet_Atm",      &C1_Planet_Atm);
	GetLcString("C1_Planet_Pressure", &C1_Planet_Pressure);
	GetLcString("C1_Planet_AtmComp",  &C1_Planet_AtmComp);
	GetLcString("C1_Planet_Ocean",    &C1_Planet_Ocean);
	GetLcString("C1_Planet_OceanDep", &C1_Planet_OceanDep);
	GetLcString("C1_Planet_OcComp",   &C1_Planet_OcComp);
	GetLcString("C1_SubSys_Title1",   &C1_SubSys_Title1);
	GetLcString("C1_SubSys_Title2",   &C1_SubSys_Title2);
	GetLcString("C1_SubSys_Title3",   &C1_SubSys_Title3);
	GetLcString("C1_SubSys_Name",     &C1_SubSys_Name);
	GetLcString("C1_SubSys_Diam",     &C1_SubSys_Diam);
	GetLcString("C1_SubSys_Mass",     &C1_SubSys_Mass);
	GetLcString("C1_SubSys_a",        &C1_SubSys_a);
	GetLcString("C1_SubSys_P",        &C1_SubSys_P);
	GetLcString("C1_SubSys_i",        &C1_SubSys_i);
	GetLcString("C1_SubSys_e",        &C1_SubSys_e);
}

void composite1()
{
	GetLocaleComp1();

	auto Barycenters = TypeIndices.find("Barycenter");
	if (Barycenters != TypeIndices.end())
	{
		for (size_t i = 0; i < Barycenters->second.size(); i++)
		{
			System[Barycenters->second[i]].LumBol = 0;
		}
	}

	cout << "Rebuilding system tree...\n";

	SystemStructure = BuildTree(Companions);

	cout << "DONE.\n";

	switch (OFormat)
	{
	case HTML:
		if (!Astrobiology && !PartitionReload)
		{
			//HTMLcontent += "\t\t\t" + string(_Html_Tags::_html_line) + '\n';
			HTMLcontent += "\t\t\t" + string(_Html_Tags::_h2_begin) + C1_Title2 + _Html_Tags::_h2_end + "\n";
			//HTMLcontent += "\t\t\t" + string(_Html_Tags::_html_line) + '\n';
		}
		break;
	case MD:
	default:
		if (!Astrobiology && !PartitionReload)
		{
			Final += "\n## " + C1_Title2 + "\n";
		}
		break;
	}

	__DFS_TransLum(SystemStructure);
	__DFS_Iterate(SystemStructure);
}