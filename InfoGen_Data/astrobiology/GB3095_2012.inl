#pragma once

// Ambient air quality standards
// https://www.mee.gov.cn/ywgz/fgbz/bz/bzwb/dqhjbh/dqhjzlbz/201203/t20120302_224165.shtml

#ifndef __GB3095_2012__
#define __GB3095_2012__

#include <CSE/Object.h>

_EXTERN_C

inline constexpr cse::float64 MolarVolume(cse::float64 Temperature, cse::float64 Pressure)
{
	return (GasConstant * Temperature) / Pressure;
}

inline int __GB3095_SO2(cse::AtmParam Atmosphere, char Base = 'y')
{
	if (Atmosphere.Composition.find("SO2") == Atmosphere.Composition.end()) { return 1; }

	cse::float64 L1, L2; // Kg/m^3
	switch (Base)
	{
	default:
	case 'y':
		L1 = 20E-9;
		L2 = 60E-9;
		break;
	case 'd':
		L1 = 50E-9;
		L2 = 150E-9;
		break;
	case 'h':
		L1 = 150E-9;
		L2 = 500E-9;
		break;
	}

	cse::float64 L1M = MolarVolume(273, StdAtm) * (L1 / (CSEM_SO2 / 1000.));
	cse::float64 L2M = MolarVolume(273, StdAtm) * (L2 / (CSEM_SO2 / 1000.));
	cse::float64 SO2Percent = Atmosphere.Composition.find("SO2")->second / 100.;
	if (L1M > SO2Percent) { return 1; }
	else if (L1M <= SO2Percent && L2M > SO2Percent) { return 2; }
	else { return 0; }
}

inline int __GB3095_CO(cse::AtmParam Atmosphere, char Base = 'd')
{
	if (Atmosphere.Composition.find("CO") == Atmosphere.Composition.end()) { return 1; }

	cse::float64 L1; // Kg/m^3
	switch (Base)
	{
	default:
	case 'd':
		L1 = 4E-6;
		break;
	case 'h':
		L1 = 10E-6;
		break;
	}

	cse::float64 L1M = MolarVolume(273, StdAtm) * (L1 / (CSEM_CO / 1000.));
	cse::float64 COPercent = Atmosphere.Composition.find("CO")->second / 100.;
	if (L1M > COPercent) { return 1; }
	else { return 0; }
}

_END_EXTERN_C

#endif