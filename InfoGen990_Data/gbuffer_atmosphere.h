#pragma once

#ifndef __GBUF_ATM__
#define __GBUF_ATM__

#include <CSE\Object.h>

struct Atmosphere
{
	cse::float64 SurfPressure;
	std::map<std::string, cse::float64> Composition;
};

Atmosphere gbuffer_atmosphere(std::shared_ptr<cse::Object> Obj);

#endif