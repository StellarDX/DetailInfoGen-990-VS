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

namespace Localization
{
	string Asb_Rating     = "Overall rating";
	string AsB_SurfGrav   = "Surface Gravity";
	string AsB_SurfGravL1 = "Too low";
	string AsB_SurfGravL2 = "Moderate";
	string AsB_SurfGravL3 = "Too high";
	string AsB_SPeriod    = "Synodic rotation period";
	string AsB_SPeriodL6  = "Tidal locked";
	string AsB_TSurf      = "Mean surface temperature";
	string Asb_HighAxT    = "High axial tilt";
	string Asb_NoAtm      = "No Atmosphere";
	string Asb_ToxicAtm   = "Toxic atmosphere";
	string Asb_UAtm1      = "Unbreathable atmosphere: Lack of oxygen.";
	string Asb_UAtm2      = "Unbreathable atmosphere: Excessive oxygen.";
	string Asb_BAtm       = "Breathable atmosphere";
	string Asb_Pressure   = "Atmosphere pressure";
}

using namespace Localization;

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
string RotationStr[] = { "Very short", "Short", "Moderate", "Long", "Very long" };

// SE standard temperature lines
static const uint64 TempLevels = 7;
static const float64 TempStd[] = { 0, 90, 170, 250, 330, 500, 1000 };
string TempStr[] = { "Frigid", "Cold", "Cool", "Temperate", "Warm", "Hot", "Torrid"};

// Pressure lines
// Reference:
// https://en.wikipedia.org/wiki/Armstrong_limit
// https://en.wikipedia.org/wiki/Effects_of_high_altitude_on_humans
// https://space.stackexchange.com/questions/32640/what-is-the-maximum-atmospheric-pressure-a-human-can-tolerate
static const uint64 PressureLevels = 7;
static const float64 PressureStd[] = {0, 6250, 38300, 47500, 30*StdAtm, 60*StdAtm , 100*StdAtm };
string PressureStr[] = { "Extremely Low", "Very low", "Low", "Moderate", "High", "Very high", "Extremely high"};

string GHMDProcPlanHabGrav(shared_ptr<Object> Obj)
{
	float64 Grav = (GravConstant * Obj->Mass) / cse::pow(Obj->Radius(), 2);
	string FmtStr = " * **" + AsB_SurfGrav + "**: " + PrecStr + " m/s^2 - {}\n";
	if (Grav < MinHabitGrav) { return vformat(FmtStr, make_format_args(Grav, AsB_SurfGravL1)); }
	else if (Grav > MaxHabitGrav) { return vformat(FmtStr, make_format_args(Grav, AsB_SurfGravL3)); }
	else { return vformat(FmtStr, make_format_args(Grav, AsB_SurfGravL2)); }
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
	
	string FmtStr = " * **" + AsB_SPeriod + "**: " + PrecStr + " sec - {}\n";
	if (isinf(SynodicPeriod)) { return vformat(FmtStr, make_format_args(SynodicPeriod, AsB_SPeriodL6)); }
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
		fstr += " * **" + Asb_HighAxT + "**\n";
	}

	return fstr;
}

string GHMDProcPlanHabMST(shared_ptr<Object> Obj)
{
	string FmtStr = " * **" + AsB_TSurf + "**: " + PrecStr + " °K - {}\n";
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
	if (Obj->NoAtmosphere) { return " * **" + Asb_NoAtm + "**\n"; }
	set<string> Exceeds; // Used to analyze toxic atmosphere
	if (!__GB3095_SO2(Obj->Atmosphere)) { Exceeds.insert("SO2"); }
	if (!__GB3095_CO(Obj->Atmosphere)) { Exceeds.insert("CO"); }
	if (!__GB16297_Cl2(Obj->Atmosphere)) { Exceeds.insert("Cl2"); }
	if (!__GB16297_Hydrocarbon(Obj->Atmosphere)) { Exceeds.insert("Hydrocarbon"); }
	if (!__GB14554_NH3(Obj->Atmosphere)) { Exceeds.insert("NH3"); }
	if (!__GB14554_H2S(Obj->Atmosphere)) { Exceeds.insert("H2S"); }

	if (!Exceeds.empty())
	{
		string out = "* **" + Asb_ToxicAtm + "**: ";
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
		return "* **" + Asb_UAtm1 + "**\n";
		break;
	case 2:
		return "* **" + Asb_BAtm + "**\n";
		break;
	case 3:
		return "* **" + Asb_UAtm2 + "**\n";
		break;
	}

	return ""; // Unreachable
}

string GHMDProcPlanHabAtmPressure(shared_ptr<Object> Obj)
{
	if (Obj->NoAtmosphere) { return ""; }
	string FmtStr = " * **" + Asb_Pressure + "**: " + PrecStr + " Pa - {}\n";
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
	os << vformat("*" + Asb_Rating + " : {}*\n", make_format_args("Incompleted"));
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

void GetLcString(string Key, string* Val);

void GetLocalAsB2()
{
	GetLcString("Asb_Rating",     &Asb_Rating);
	GetLcString("AsB_SurfGrav",   &AsB_SurfGrav);
	GetLcString("AsB_SurfGravL1", &AsB_SurfGravL1);
	GetLcString("AsB_SurfGravL2", &AsB_SurfGravL2);
	GetLcString("AsB_SurfGravL3", &AsB_SurfGravL3);
	GetLcString("AsB_SPeriod",    &AsB_SPeriod);
	GetLcString("AsB_SPeriodL1",  &RotationStr[0]);
	GetLcString("AsB_SPeriodL2",  &RotationStr[1]);
	GetLcString("AsB_SPeriodL3",  &RotationStr[2]);
	GetLcString("AsB_SPeriodL4",  &RotationStr[3]);
	GetLcString("AsB_SPeriodL5",  &RotationStr[4]);
	GetLcString("AsB_SPeriodL6",  &AsB_SPeriodL6);
	GetLcString("AsB_TSurf",      &AsB_TSurf);
	GetLcString("AsB_TSurfL1",    &TempStr[0]);
	GetLcString("AsB_TSurfL2",    &TempStr[1]);
	GetLcString("AsB_TSurfL3",    &TempStr[2]);
	GetLcString("AsB_TSurfL4",    &TempStr[3]);
	GetLcString("AsB_TSurfL5",    &TempStr[4]);
	GetLcString("AsB_TSurfL6",    &TempStr[5]);
	GetLcString("AsB_TSurfL7",    &TempStr[6]);
	GetLcString("Asb_HighAxT",    &Asb_HighAxT);
	GetLcString("Asb_NoAtm",      &Asb_NoAtm);
	GetLcString("Asb_ToxicAtm",   &Asb_ToxicAtm);
	GetLcString("Asb_UAtm1",      &Asb_UAtm1);
	GetLcString("Asb_UAtm2",      &Asb_UAtm2);
	GetLcString("Asb_BAtm",       &Asb_BAtm);
	GetLcString("Asb_Pressure",   &Asb_Pressure);
	GetLcString("Asb_PressureL1", &PressureStr[0]);
	GetLcString("Asb_PressureL2", &PressureStr[1]);
	GetLcString("Asb_PressureL3", &PressureStr[2]);
	GetLcString("Asb_PressureL4", &PressureStr[3]);
	GetLcString("Asb_PressureL5", &PressureStr[4]);
	GetLcString("Asb_PressureL6", &PressureStr[5]);
	GetLcString("Asb_PressureL7", &PressureStr[6]);
}

void composite4()
{
	GetLocalAsB2();

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