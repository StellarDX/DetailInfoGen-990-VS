#pragma once

// Integrated emission standard of air pollutants
// https://www.mee.gov.cn/ywgz/fgbz/bz/bzwb/dqhjbh/dqgdwrywrwpfbz/199701/t19970101_67504.htm

#ifndef __GB16297_1996__
#define __GB16297_1996__

#include <CSE/Object.h>

_EXTERN_C

inline constexpr cse::float64 MolarVolume(cse::float64 Temperature, cse::float64 Pressure);

inline int __GB16297_Cl2(cse::AtmParam Atmosphere)
{
	if (Atmosphere.Composition.find("Cl2") == Atmosphere.Composition.end()) { return 1; }
	cse::float64 L1 = 0.4E-6, L2 = 0.5E-6;

	cse::float64 L1M = MolarVolume(273, StdAtm) * (L1 / (CSEM_Cl2 / 1000.));
	cse::float64 L2M = MolarVolume(273, StdAtm) * (L2 / (CSEM_Cl2 / 1000.));
	cse::float64 Cl2Percent = Atmosphere.Composition.find("Cl2")->second / 100.;
	if (L1M > Cl2Percent) { return 1; }
	else if (L1M <= Cl2Percent && L2M > Cl2Percent) { return 2; }
	else { return 0; }
}

inline int __GB16297_Hydrocarbon(cse::AtmParam Atmosphere)
{
	bool HasC2H2 = Atmosphere.Composition.find("C2H2") != Atmosphere.Composition.end();
	bool HasC2H4 = Atmosphere.Composition.find("C2H4") != Atmosphere.Composition.end();
	bool HasC2H6 = Atmosphere.Composition.find("C2H6") != Atmosphere.Composition.end();
	bool HasC3H8 = Atmosphere.Composition.find("C3H8") != Atmosphere.Composition.end();
	bool HasC8H18 = Atmosphere.Composition.find("C8H18") != Atmosphere.Composition.end();
	if (!HasC2H2 && !HasC2H4 && !HasC2H6 && !HasC3H8 && !HasC8H18) { return 1; }

	cse::float64 TotalHydroCarbonPercent = 0, C2H2 = 0, C2H4 = 0, C2H6 = 0, C3H8 = 0, C8H18 = 0;
	if (HasC2H2) { C2H2 += Atmosphere.Composition.find("C2H2")->second / 100.; }
	if (HasC2H4) { C2H4 += Atmosphere.Composition.find("C2H4")->second / 100.; }
	if (HasC2H6) { C2H6 += Atmosphere.Composition.find("C2H6")->second / 100.; }
	if (HasC3H8) { C3H8 += Atmosphere.Composition.find("C3H8")->second / 100.; }
	if (HasC8H18) { C8H18 += Atmosphere.Composition.find("C8H18")->second / 100.; }
	TotalHydroCarbonPercent = C2H2 + C2H4 + C2H6 + C3H8 + C8H18;

	cse::float64 L1 = 0.4E-6, L2 = 0.5E-6;
	cse::float64 AvgMolarMass = (CSEM_C2H2 * C2H2 + CSEM_C2H4 * C2H4 + CSEM_C2H6 * C2H6 + CSEM_C3H8 * C3H8 + CSEM_C8H18 * C8H18) / TotalHydroCarbonPercent;
	cse::float64 L1M = MolarVolume(273, StdAtm) * (L1 / (AvgMolarMass / 1000.));
	cse::float64 L2M = MolarVolume(273, StdAtm) * (L2 / (AvgMolarMass / 1000.));

	if (L1M > TotalHydroCarbonPercent) { return 1; }
	else if (L1M <= TotalHydroCarbonPercent && L2M > TotalHydroCarbonPercent) { return 2; }
	else { return 0; }
}

_END_EXTERN_C

#endif