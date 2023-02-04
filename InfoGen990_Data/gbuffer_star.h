#pragma once

#ifndef __GBUF_STAR__
#define __GBUF_STAR__

#include <CSE/Lumine.h>
#include <CSE/Object.h>

struct StarParams
{
	cse::float64 AbsMagnBol;      // Bolometric Magniyude
	std::string  SpType;          // Spectral classification
	cse::float64 EqRadius;        // Equatorial radius
	//cse::float64 EqCircum;        // Equatorial circumference
	cse::float64 Flattening;      // Flattening
	//cse::float64 SurfArea;        // Surface area
	cse::float64 Volume;          // Volume
	cse::float64 Mass;            // Mass
	cse::float64 AvgDensity;      // Average density
	cse::float64 EqSurfGrav;      // Equatorial surface gravity
	cse::float64 EscapeVelocity;  // Escape velocity
	cse::float64 Temperature;     // Temperature
	cse::float64 Luminosity;      // Luminosity
	cse::float64 Obliquity;       // Obliquity
	cse::float64 RotationPeriod;  // Sidereal rotation period
};

std::string GenStarType(cse::SPECSTR Spec);

StarParams gbuffer_star(std::shared_ptr<cse::Object> arg);

#endif