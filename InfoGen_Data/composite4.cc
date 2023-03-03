#include "composite4.h"

#include "gbuffer_planet.h"
#include "composite.h"
#include "composite3.h"
#include "final.h"

#include "astrobiology/GB3095_2012.inl"
#include "astrobiology/DB23T_1791_2021.inl"
#include "astrobiology/GB16297-1996.inl"
#include "astrobiology/GB14554_93.inl"

#include <set>

using namespace std;
using namespace cse;

enum HabRank { ACE, SSS, SS, S, A, B, C, D };
static const string HabRankStr[8]{ "ACE", "SSS", "SS", "S", "A", "B", "C", "D" };

// Reference: 
// https://en.wikipedia.org/wiki/Planetary_habitability
// https://en.wikipedia.org/wiki/Superhabitable_planet

static const string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";

// Min gravity(calculated from 0.1 MEarth and 0.5 REarth, ~3.919315179958494017732921783783)
// But min mass maybe as low as 0.0268 MEarth.
static const float64 MinHabitGrav = GravConstant * (0.1 * MassEarth) / cse::pow(0.5 * RadEarth, 2);
// Max gravity(calculated from 5 MEarth and 1.5 REarth, ~21.773973221991633431849565465461)
static const float64 MaxHabitGrav = GravConstant * (5. * MassEarth) / cse::pow(1.5 * RadEarth, 2);
// [A possable option given by New Bing AI]
// According to a web search result1, planets with up to about 2 times the mass of Earth should probably be habitable. 
// With normal planetary materials, the surface gravity of worlds with such masses should get up to about 1.6 g. 
// This means that a habitable planet could have a range of surface gravity from about 0.38 g (Mars) to about 1.6 g (a super-Earth).
// (g = 9.7982879498962350443323044594576(Min) - 9.8643234800650223859143567039082(Max))

// Rotation using standard of previous version(possible values), both web source and New Bing are failed to produce better standard.
static const uint64 RotationLevels = 5;
static const float64 RotationStd[] = {0, 10, 20, 35, 50};
static const string RotationStr[] = { "Very short", "Short", "Moderate", "Long", "Very long" };

// SE standard temperature lines
static const uint64 TempLevels = 7;
static const float64 TempStd[] = { 0, 90, 170, 250, 330, 500, 1000 };
static const string TempStr[] = { "Frigid", "Cold", "Cool", "Temperate", "Warm", "Hot", "Torrid"};

// Pressure lines
// Reference:
// https://en.wikipedia.org/wiki/Armstrong_limit
// https://en.wikipedia.org/wiki/Effects_of_high_altitude_on_humans
// https://space.stackexchange.com/questions/32640/what-is-the-maximum-atmospheric-pressure-a-human-can-tolerate
static const uint64 PressureLevels = 7;
static const float64 PressureStd[] = {0, 6250, 38300, 47500, 30*StdAtm, 60*StdAtm , 100*StdAtm };
static const string PressureStr[] = { "Extremely Low", "Very low", "Low", "Moderate", "High", "Very high", "Extremely high"};

string GHMDProcPlanHabGrav(shared_ptr<Object> Obj)
{
	float64 Grav = (GravConstant * Obj->Mass) / cse::pow(Obj->Radius(), 2);
	string FmtStr = " * **Surface Gravity**: " + PrecStr + " m/s^2 - {}\n";
	if (Grav < MinHabitGrav) { return vformat(FmtStr, make_format_args(Grav, "Too low")); }
	else if (Grav > MaxHabitGrav) { return vformat(FmtStr, make_format_args(Grav, "Too high")); }
	else { return vformat(FmtStr, make_format_args(Grav, "Moderate")); }
}

string GHMDProcPlanHabRotation(shared_ptr<Object> Obj)
{
	float64 SynodicPeriod;
	if ((Obj->Rotation.Obliquity > 90 && Obj->Rotation.Obliquity < 270) ||
		(Obj->Rotation.Obliquity < -90 && Obj->Rotation.Obliquity > -270))
	{
		SynodicPeriod = -(Obj->Orbit.Period * Obj->Rotation.RotationPeriod) / (Obj->Orbit.Period + Obj->Rotation.RotationPeriod);
	}
	else { SynodicPeriod = (Obj->Orbit.Period * Obj->Rotation.RotationPeriod) / (Obj->Orbit.Period - Obj->Rotation.RotationPeriod); }
	
	string FmtStr = " * **Synodic rotation period**: " + PrecStr + " sec - {}\n";
	if (isinf(SynodicPeriod)) { return vformat(FmtStr, make_format_args(SynodicPeriod, "Tidal locked")); }
	for (size_t i = 0; i < RotationLevels - 1; i++)
	{
		if (3600. * RotationStd[i] < cse::abs(SynodicPeriod) && cse::abs(SynodicPeriod) < 3600. * RotationStd[i + 1])
		{
			return vformat(FmtStr, make_format_args(SynodicPeriod, RotationStr[i]));
		}
	}

	string fstr = vformat(FmtStr, make_format_args(SynodicPeriod, RotationStr[RotationLevels - 1]));
	if ((Obj->Rotation.Obliquity > 60 && Obj->Rotation.Obliquity < 120) ||
		(Obj->Rotation.Obliquity < -60 && Obj->Rotation.Obliquity > -120) ||
		(Obj->Rotation.Obliquity < 240 && Obj->Rotation.Obliquity > 300) || 
		(Obj->Rotation.Obliquity < -240 && Obj->Rotation.Obliquity > -300))
	{
		fstr += " * **High axial tilt**\n";
	}

	return fstr;
}

string GHMDProcPlanHabMST(shared_ptr<Object> Obj)
{
	string FmtStr = " * **Mean surface temperature**: " + PrecStr + " °K - {}\n";
	for (size_t i = 0; i < TempLevels - 1; i++)
	{
		if (TempStd[i] < Obj->Teff && Obj->Teff < TempStd[i + 1])
		{
			return vformat(FmtStr, make_format_args(Obj->Teff, TempStr[i]));
		}
	}
	return vformat(FmtStr, make_format_args(Obj->Teff, TempStr[TempLevels - 1]));
}

string GHMDProcPlanHabAtm(shared_ptr<Object> Obj)
{
	if (Obj->NoAtmosphere) { return " * **No Atmosphere**\n"; }
	set<string> Exceeds; // Used to analyze toxic atmosphere
	if (!__GB3095_SO2(Obj->Atmosphere)) { Exceeds.insert("SO2"); }
	if (!__GB3095_CO(Obj->Atmosphere)) { Exceeds.insert("CO"); }
	if (!__GB16297_Cl2(Obj->Atmosphere)) { Exceeds.insert("Cl2"); }
	if (!__GB16297_Hydrocarbon(Obj->Atmosphere)) { Exceeds.insert("Hydrocarbon"); }
	if (!__GB14554_NH3(Obj->Atmosphere)) { Exceeds.insert("NH3"); }
	if (!__GB14554_H2S(Obj->Atmosphere)) { Exceeds.insert("H2S"); }

	if (!Exceeds.empty())
	{
		string out = "* **Toxic atmosphere**: ";
		auto it = Exceeds.begin();
		auto end = Exceeds.end();
		int i = 0;
		while (it != end)
		{
			out += *it;
			if (i < Exceeds.size() - 1) { out += ", "; }
			++it;
			++i;
		}
		return out + '\n';
	}

	auto Breathable = __DB23T_1791_O2(Obj->Atmosphere);
	switch (Breathable)
	{
	case 0:
	case 1:
	default:
		return "* **Unbreathable atmosphere**: Lack of oxygen.\n";
		break;
	case 2:
		return "* **Breathable atmosphere**\n";
		break;
	case 3:
		return "* **Unbreathable atmosphere**: Excessive oxygen.\n";
		break;
	}

	return ""; // Unreachable
}

string GHMDProcPlanHabAtmPressure(shared_ptr<Object> Obj)
{
	if (Obj->NoAtmosphere) { return ""; }
	string FmtStr = " * **Atmosphere pressure**: " + PrecStr + " Pa - {}\n";
	for (size_t i = 0; i < PressureLevels - 1; i++)
	{
		if (PressureStd[i] < Obj->Atmosphere.Pressure && Obj->Atmosphere.Pressure < PressureStd[i + 1])
		{
			return vformat(FmtStr, make_format_args(Obj->Atmosphere.Pressure, PressureStr[i]));
		}
	}
	return vformat(FmtStr, make_format_args(Obj->Atmosphere.Pressure, PressureStr[PressureLevels - 1]));
}

string GHMDProcPlanHab(shared_ptr<Object> Obj)
{
	ostringstream os;
	os << vformat("\n### {} - {}\n", make_format_args(Obj->Name[0], GenPlanetType(Obj)));
	os << vformat("*Overall rating : {}*\n", make_format_args("Incompleted"));
	os << GHMDProcPlanHabGrav(Obj);
	os << GHMDProcPlanHabRotation(Obj);
	os << GHMDProcPlanHabMST(Obj);
	os << GHMDProcPlanHabAtm(Obj);
	os << GHMDProcPlanHabAtmPressure(Obj);
	return os.str();
}

string GHMDHabOut()
{
	ostringstream os;
	os << "## Details\n";
	for (size_t i = 0; i < RockyObjBuf.size(); i++)
	{
		os << GHMDProcPlanHab(RockyObjBuf[i]);
	}
	return os.str();
}

void composite4()
{
	switch (OFormat)
	{
	case HTML:
		break;
	case MD:
	default:
		Final += GHMDHabOut();
		break;
	}
}