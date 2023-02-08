#pragma once

#ifndef __GBUF_BASIC__
#define __GBUF_BASIC__

#include <map>
#include <vector>
#include <CSE/Lumine.h>
#include <CSE/Planets.h>
#include "final.h"

struct SystemInfo
{
	cse::float64 Age = 0;           // Age
	cse::float64 Mass = 0;          // System mass
	cse::float64 DistOutPlanet = 0; // Semi-major axis of outer known planet
	cse::uint64  NStars = 0;        // Stars
	cse::uint64  NPlanets = 0;      // Known planets
	cse::uint64  NDPlanets = 0;     // Known dwarf planets
	cse::uint64  NSatellites = 0;   // Known natural satellites
	cse::uint64  NMPlanets = 0;     // Known minor planets
	cse::uint64  NComets = 0;       // Known comets
	cse::uint64  NRSatellites = 0;  // Identified rounded satellites
	std::string  SpType;            // Spectral type
};

SystemInfo gbuffer_basic(ObjectBuffer& Sys, std::map<std::string, std::vector<size_t>>& TypeIdx);

#endif
