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

#include "gbuffer_star.h"
#include "gbuffer_planet.h"
#include "gbuffer_atmosphere.h"
#include "gbuffer_water.h"
#include "composite.h"
#include "composite1.h"

using namespace std;
using namespace cse;

shared_ptr<SystemStruct> SystemStructure;
stack<SystemStruct::Elem> ParentBodyStack;
shared_ptr<SystemStruct> CurrentSubSystem;
float64 SynodicPeriodBuffer;

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

		ParentIndices.insert(pair(ParentIdx, it->second));
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

string GHMarkDownGenOrbit(shared_ptr<Object> Obj)
{
	ostringstream os;
	string fmtstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	string ifmtstring = "| {} | {} |\n";
	os << '\n';
	os << " * Orbital characteristics\n";
	os << "| | |\n|:---|---:|\n";
	if (!isinf(Obj->Orbit.Epoch)) { os << vformat(fmtstring, make_format_args("Epoch", Obj->Orbit.Epoch)); }
	else { os << vformat(fmtstring, make_format_args("Epoch", float64(J2000))); }
	os << vformat(ifmtstring, make_format_args("Reference System", Obj->Orbit.RefPlane));
	if (Obj->Orbit.Binary)
	{
		os << vformat(ifmtstring, make_format_args("Primary", Obj->ParentBody));
		os << vformat(ifmtstring, make_format_args("Companion", Obj->Name[0]));
	}
	if (Obj->Orbit.Eccentricity != 0)
	{
		if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
		{
			if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args("Aphelion (m)", Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist)); }
			os << vformat(fmtstring, make_format_args("Perihelion (m)", Obj->Orbit.PericenterDist));
		}
		else if (ParentBodyStack.top().Pointer->Type == "Planet")
		{
			if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args("Apogee (m)", Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist)); }
			os << vformat(fmtstring, make_format_args("Perigee (m)", Obj->Orbit.PericenterDist));
		}
		else
		{
			if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args("Apoapsis (m)", Obj->Orbit.SemiMajorAxis() * 2 - Obj->Orbit.PericenterDist)); }
			os << vformat(fmtstring, make_format_args("Periapsis (m)", Obj->Orbit.PericenterDist));
		}
	}
	if (Obj->Orbit.Eccentricity < 1) { os << vformat(fmtstring, make_format_args("Semi-major axis (a) (m)", Obj->Orbit.SemiMajorAxis())); }
	os << vformat(fmtstring, make_format_args("Eccentricity (e)", Obj->Orbit.Eccentricity));
	os << vformat(fmtstring, make_format_args("Orbital period (sidereal) (P) (s)", Obj->Orbit.Period));

	// Calculate Synodic month when object is moon.
	if (ParentBodyStack.top().Pointer->Type == "Planet")
	{
		auto OrbitParent = ParentBodyStack.top().Pointer->Orbit;
		if ((Obj->Orbit.Inclination > 90 && Obj->Orbit.Inclination < 270) ||
			(Obj->Orbit.Inclination < -90 && Obj->Orbit.Inclination > -270)) // Check retrograde
		{
			SynodicPeriodBuffer = (Obj->Orbit.Period * OrbitParent.Period) / (OrbitParent.Period + Obj->Orbit.Period);
			os << vformat(fmtstring, make_format_args("Orbital period (synodic) (s)", SynodicPeriodBuffer));
			SynodicPeriodBuffer *= -1;
		}
		else
		{
			SynodicPeriodBuffer = (Obj->Orbit.Period * OrbitParent.Period) / (OrbitParent.Period - Obj->Orbit.Period);
			os << vformat(fmtstring, make_format_args("Orbital period (synodic) (s)", SynodicPeriodBuffer));
		}
	}

	os << vformat(fmtstring, make_format_args("Mean anomaly", Obj->Orbit.MeanAnomaly));
	os << vformat(fmtstring, make_format_args("Inclination (i)", Obj->Orbit.Inclination));
	os << vformat(fmtstring, make_format_args("Longitude of ascending node (Ω)", Obj->Orbit.AscendingNode));
	if (Obj->Orbit.Binary)
	{
		os << vformat(fmtstring, make_format_args("Argument of periastron (ω) (secondary)", Obj->Orbit.ArgOfPericenter));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && Obj->Type == "Planet"))
	{
		os << vformat(fmtstring, make_format_args("Argument of perihelion (ω)", Obj->Orbit.ArgOfPericenter));
	}
	else if (ParentBodyStack.top().Pointer->Type == "Planet")
	{
		os << vformat(fmtstring, make_format_args("Argument of perigee (ω)", Obj->Orbit.ArgOfPericenter));
	}
	else
	{
		os << vformat(fmtstring, make_format_args("Argument of periapsis (ω)", Obj->Orbit.ArgOfPericenter));
	}

	return os.str();
}

string GHMarkDownGenSubSys(shared_ptr<Object> Obj)
{
	ostringstream os;
	if (CurrentSubSystem != nullptr)
	{
		if (Obj->Type == "Barycenter") { os << " * Compositions\n"; }
		else if (Obj->Type == "Moon") { os << " * Sub-satellites\n"; }
		else { os << " * Satellites\n"; }
		os << "| Name | Diameter(m) | Mass(Kg) | Semi-Major Axis(m) | Orbital Period(s) | Inclination | Eccentricity |\n|:---|:---|:---|:---|:---|:---|:---|\n";
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

	os << vformat("\n### {} - {}\n", make_format_args(Obj->Name[0], "System Barycenter"));
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
		os << " * Physical characteristics\n";
		os << "| | |\n|:---|---:|\n";
		os << vformat(fmtstring, make_format_args("Mass (Kg)", Par.Mass));
		os << vformat(fmtstring, make_format_args("Schwarzschild Radius (m)", Par.SchwarzschildRadius));
		os << vformat(fmtstring, make_format_args("Angular momentum", Par.Spin));
		os << vformat(fmtstring, make_format_args("Electric charge", Par.Charge));
	}
	else
	{
		StarParams Params = gbuffer_star(Obj);
		os << " * Physical characteristics\n";
		os << "| | |\n|:---|---:|\n";
		os << vformat(fmtstring, make_format_args("Bolometric magnitude", Params.AbsMagnBol));
		os << vformat(ifmtstring, make_format_args("Spectral classification", Params.SpType));
		os << vformat(fmtstring, make_format_args("Equatorial radius (m)", Params.EqRadius));
		os << vformat(fmtstring, make_format_args("Flattening", Params.Flattening));
		os << vformat(fmtstring, make_format_args("Volume (m^3)", Params.Volume));
		os << vformat(fmtstring, make_format_args("Mass (Kg)", Params.Mass));
		os << vformat(fmtstring, make_format_args("Average density (Kg/m^3)", Params.AvgDensity));
		os << vformat(fmtstring, make_format_args("Equatorial surface gravity (m/s^2)", Params.EqSurfGrav));
		os << vformat(fmtstring, make_format_args("Escape velocity (from the surface) (m/s)", Params.EscapeVelocity));
		os << vformat(fmtstring, make_format_args("Temperature (°K)", Params.Temperature));
		os << vformat(fmtstring, make_format_args("Luminosity (W)", Params.Luminosity));
		if (!Obj->TidalLocked)
		{
			os << vformat(fmtstring, make_format_args("Obliquity", Params.Obliquity));
			os << vformat(fmtstring, make_format_args("RotationPeriod (s)", Params.RotationPeriod));
		}
	}

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

	os << " * Physical characteristics\n";
	os << "| | |\n|:---|---:|\n";
	os << vformat(fmtstring, make_format_args("Mean radius (m)", Par.MeanRadius));
	if (Par.Flattening != 0)
	{
		os << vformat(fmtstring, make_format_args("Equatorial radius (m)", Par.EqRadius));
		os << vformat(fmtstring, make_format_args("Polar radius (m)", Par.PolarRadius));
	}
	os << vformat(fmtstring, make_format_args("Flattening", Par.Flattening));
	os << vformat(fmtstring, make_format_args("Volume (m^3)", Par.Volume));
	os << vformat(fmtstring, make_format_args("Mass (Kg)", Par.Mass));
	os << vformat(fmtstring, make_format_args("Mean density (Kg/m^3)", Par.MeanDensity));
	os << vformat(fmtstring, make_format_args("Surface gravity (m/s^2)", Par.SurfGrav));
	os << vformat(fmtstring, make_format_args("Escape velocity (m/s)", Par.EscapeVel));
	if (ParentBodyStack.top().Pointer->Type == "Planet" && Obj->TidalLocked)
	{
		Par.SynodicPeriod = SynodicPeriodBuffer;
	}
	os << vformat(fmtstring, make_format_args("Synodic rotation period (s)", Par.SynodicPeriod));
	os << vformat(fmtstring, make_format_args("Sidereal rotation period (s)", Par.RotationPeriod));
	os << vformat(fmtstring, make_format_args("Axial tilt", Par.AxialTilt));
	os << vformat(fmtstring, make_format_args("Albedo (Bond)", Par.AlbedoBond));
	os << vformat(fmtstring, make_format_args("Albedo (geometric)", Par.AlbedoGeom));

	os << " * Surface temperature (Approximate value, only for reference)\n";
	os << "| Min | Mean | Max |\n|:---:|:---:|:---:|\n";
	string tfmtstring = "| {:." + to_string(_OUT_PRECISION) + "g} | {:." + to_string(_OUT_PRECISION) + "g} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	vec3 Temperatures(Par.MinTemperature, Par.MeanTemperature, Par.MaxTemperature);

	#define IGNORE_GAS_GIANT_GREENHOUSE 1 // When open, greenhouse effect on gas giants will ignored

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
		os << " * Atmosphere\n";
		os << "| | |\n|:---|:---|\n";
		string aformatstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g}% {} |\n";
		os << vformat(fmtstring, make_format_args("Surface pressure (Pa)", Atm.SurfPressure));
		auto it = Atm.Composition.begin();
		auto end = Atm.Composition.end();
		while (it != end)
		{
			if (it == Atm.Composition.begin())
			{
				os << vformat(aformatstring, make_format_args("Composition by volume", it->second, it->first));
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
		os << " * Ocean\n";
		os << "| | |\n|:---|:---|\n";
		string aformatstring = "| {} | {:." + to_string(_OUT_PRECISION) + "g}% {} |\n";
		os << vformat(fmtstring, make_format_args("Depth (m)", Oc.Height));
		auto it = Oc.Composition.begin();
		auto end = Oc.Composition.end();
		while (it != end)
		{
			if (it == Oc.Composition.begin())
			{
				os << vformat(aformatstring, make_format_args("Composition by volume", it->second, it->first));
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
		break;
	case MD:
	default:
		Final += GHMarkDownProc(Obj);
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

void composite1()
{
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
		break;
	case MD:
	default:
		Final += "\n## Objects\n";
		break;
	}

	__DFS_TransLum(SystemStructure);
	__DFS_Iterate(SystemStructure);
}