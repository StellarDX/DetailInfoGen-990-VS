#pragma once

// Emission standards for odor pollutants
// https://www.mee.gov.cn/ywgz/fgbz/bz/bzwb/dqhjbh/dqgdwrywrwpfbz/199401/t19940115_67548.shtml

#ifndef __GB14554_93__
#define __GB14554_93__

#include <CSE/Object.h>

_EXTERN_C

inline constexpr cse::float64 MolarVolume(cse::float64 Temperature, cse::float64 Pressure);

inline int __GB14554_NH3(cse::AtmParam Atmosphere)
{
	if (Atmosphere.Composition.find("NH3") == Atmosphere.Composition.end()) { return 1; }
	cse::float64 L1 = 1E-6, L2 = 1.5E-6, N2 = 2E-6, L3 = 4E-6, N3 = 5E-6;

	cse::float64 L1M = MolarVolume(273, StdAtm) * (L1 / (CSEM_NH3 / 1000.));
	cse::float64 L2M = MolarVolume(273, StdAtm) * (L2 / (CSEM_NH3 / 1000.));
	cse::float64 N2M = MolarVolume(273, StdAtm) * (N2 / (CSEM_NH3 / 1000.));
	cse::float64 L3M = MolarVolume(273, StdAtm) * (L3 / (CSEM_NH3 / 1000.));
	cse::float64 N3M = MolarVolume(273, StdAtm) * (N3 / (CSEM_NH3 / 1000.));

	cse::float64 NH3Percent = Atmosphere.Composition.find("NH3")->second / 100.;
	if (NH3Percent < L1M) { return 1; }
	else if (L1M <= NH3Percent && NH3Percent < L2M) { return 2; }
	else if (L2M <= NH3Percent && NH3Percent < N2M) { return -2; }
	else if (N2M <= NH3Percent && NH3Percent < L3M) { return 3; }
	else if (L3M <= NH3Percent && NH3Percent < N3M) { return -3; }
	else { return 0; }
}

inline int __GB14554_H2S(cse::AtmParam Atmosphere)
{
	if (Atmosphere.Composition.find("H2S") == Atmosphere.Composition.end()) { return 1; }
	cse::float64 L1 = 0.03E-6, L2 = 0.06E-6, N2 = 0.1E-6, L3 = 0.32E-6, N3 = 0.6E-6;

	cse::float64 L1M = MolarVolume(273, StdAtm) * (L1 / (CSEM_H2S / 1000.));
	cse::float64 L2M = MolarVolume(273, StdAtm) * (L2 / (CSEM_H2S / 1000.));
	cse::float64 N2M = MolarVolume(273, StdAtm) * (N2 / (CSEM_H2S / 1000.));
	cse::float64 L3M = MolarVolume(273, StdAtm) * (L3 / (CSEM_H2S / 1000.));
	cse::float64 N3M = MolarVolume(273, StdAtm) * (N3 / (CSEM_H2S / 1000.));

	cse::float64 H2SPercent = Atmosphere.Composition.find("H2S")->second / 100.;
	if (H2SPercent < L1M) { return 1; }
	else if (L1M <= H2SPercent && H2SPercent < L2M) { return 2; }
	else if (L2M <= H2SPercent && H2SPercent < N2M) { return -2; }
	else if (N2M <= H2SPercent && H2SPercent < L3M) { return 3; }
	else if (L3M <= H2SPercent && H2SPercent < N3M) { return -3; }
	else { return 0; }
}

_END_EXTERN_C

#endif