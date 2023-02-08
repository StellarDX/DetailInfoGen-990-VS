#include "gbuffer_basic.h"
#include "composite.h"

using namespace std;
using namespace cse;

bool ValueExist(float64 Dst)
{
	if (isinf(Dst)) { return false; }
	return true;
}

bool ValueExist(string Dst)
{
	if (NO_DATA_STRING == Dst) { return false; }
	return true;
}

SystemInfo gbuffer_basic(ObjectBuffer& Sys, std::map<std::string, std::vector<size_t>>& TypeIdx)
{
	SystemInfo Info;

	auto it = TypeIdx.find("Star");
	auto end = TypeIdx.end();

	if (it != end)
	{
		Info.NStars = it->second.size();
		if (ValueExist(Sys[it->second[0]].Age))
		{
			Info.Age = Sys[it->second[0]].Age;
		}

		string SpTyStr;
		if (it->second.size() > 1)
		{
			for (size_t i = 0; i < it->second.size(); i++)
			{
				if (IsNeutronStar(Sys[it->second[i]].SpecClass)) { SpTyStr += "Pulsar"; }
				else if (IsBlackHole(Sys[it->second[i]].SpecClass)) { SpTyStr += "BlackHole"; }
				else { SpTyStr += Sys[it->second[i]].SpecClass; }
				if (i < it->second.size() - 1)
				{
					SpTyStr += " + ";
				}
			}
		}
		Info.SpType = SpTyStr;
	}

	for (size_t i = 0; i < Sys.size(); i++)
	{
		if (Sys[i].Type != "Barycenter")
		{
			Info.Mass += Sys[i].Mass;
		}
	}

	it = TypeIdx.find("Planet");

	if (it != end)
	{
		Info.NPlanets = it->second.size();
		for (size_t i = 0; i < it->second.size(); i++)
		{
			Info.DistOutPlanet = Info.DistOutPlanet > Sys[it->second[0]].Orbit.SemiMajorAxis() ? Info.DistOutPlanet : Sys[it->second[0]].Orbit.SemiMajorAxis();
		}
	}

	it = TypeIdx.find("DwarfPlanet");

	if (it != end)
	{
		Info.NDPlanets = it->second.size();
	}

	it = TypeIdx.find("Moon");

	if (it != end)
	{
		Info.NRSatellites = it->second.size();
		Info.NSatellites += it->second.size();
	}

	it = TypeIdx.find("DwarfMoon");

	if (it != end)
	{
		Info.NSatellites += it->second.size();
	}

	it = TypeIdx.find("Asteroid");

	if (it != end)
	{
		Info.NMPlanets = it->second.size();
	}

	it = TypeIdx.find("Comet");

	if (it != end)
	{
		Info.NComets = it->second.size();
	}

	return Info;
}