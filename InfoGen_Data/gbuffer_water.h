#pragma once

#ifndef __GBUF_WATER__
#define __GBUF_WATER__

#include <CSE\Object.h>

struct Ocean
{
	cse::float64 Height;
	std::map<std::string, cse::float64> Composition;
};

Ocean gbuffer_water(std::shared_ptr<cse::Object> Obj);

#endif