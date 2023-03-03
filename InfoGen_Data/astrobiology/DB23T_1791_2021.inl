#pragma once

// Safety Technical Regulations for Working in Limited Space
// https://dbba.sacinfo.org.cn/stdDetail/d63b211e8a3217f11fb8edfa430787eb709b75a654c1e094fec914c90859cac0

#ifndef __DB23T_1791_2021__
#define __DB23T_1791_2021__

#include <CSE/Object.h>

_EXTERN_C

inline int __DB23T_1791_O2(cse::AtmParam Atmosphere)
{
	if (Atmosphere.Composition.find("O2") == Atmosphere.Composition.end()) { return 0; }
	cse::float64 O2Percent = Atmosphere.Composition.find("O2")->second;
	if (0 < O2Percent && O2Percent <= 19.5) { return 1; }
	else if (O2Percent > 23.5) { return 3; }
	else { return 2; }
}

_END_EXTERN_C

#endif