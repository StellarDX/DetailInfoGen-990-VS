#pragma once

#ifndef __GCL_COMPOSITE0__
#define __GCL_COMPOSITE0__

#include <CSE/SCStream.h>
#include <CSE/Object.h>
#include <CSE/Random.h>

extern cse::vec2 __Hadean_Eon_Range;
extern cse::vec2 __Archean_Eon_Range;
extern cse::vec2 __Proterozoic_Eon_Range;
extern cse::vec2 __Phanerozoic_Eon_Start;
extern cse::vec2 __Palaeozoic_Era_Range;
extern cse::vec2 __Mesozoic_Era_Range;
extern cse::vec2 __Cenozoic_Era_Begin;

extern std::map<cse::float64, std::string> __Hadean_Eon_Timeline;
extern std::map<cse::float64, std::string> __Archean_Eon_Timeline;
extern std::map<cse::float64, std::string> __Proterozoic_Eon_Timeline;
extern std::map<cse::float64, std::string> __Palaeozoic_Era_Timeline;
extern std::map<cse::float64, std::string> __Mesozoic_Era_Timeline;
extern std::map<cse::float64, std::string> __Cenozoic_Era_Timeline;
extern std::map<cse::float64, std::string> __Civilization_Timeline;

extern cse::float64 EndYear;
extern cse::float64 EndCivil;
extern std::string EndStr;

extern cse::ISCStream SysIn;
void composite0geo(cse::Object Target, cse::Object Parent);

#include "COMMON.INL"

#endif
