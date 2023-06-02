#pragma once

#ifndef __GBUF_STAR__
#define __GBUF_STAR__

#include <CSE/Lumine.h>
#include <CSE/Object.h>
#include <CSE/Stars.h>

struct StarParams
{
	cse::float64 AbsMagnBol;      // Bolometric Magniyude
	std::string  SpType;          // Spectral classification
	cse::float64 EqRadius;        // Equatorial radius
	cse::float64 EqCircum;        // Equatorial circumference
	cse::float64 Flattening;      // Flattening
	cse::float64 SurfArea;        // Surface area
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

struct BlackHoleParams
{
	// Physical characteristics of a black hole is only defined by 3 parameters
	// https://en.wikipedia.org/wiki/No-hair_theorem
	// https://en.wikipedia.org/wiki/Einstein_field_equations#Einstein%E2%80%93Maxwell_equations
	cse::float64 Mass;                // mass–energy
	cse::float64 SchwarzschildRadius; // Schwarzschild Radius
	cse::float64 Spin;                // angular momentum
	cse::float64 Charge;              // electric charge
};

std::string GenStarType(std::shared_ptr<cse::Object> Pointer);
BlackHoleParams BlackHolePar(std::shared_ptr<cse::Object> arg);
StarParams gbuffer_star(std::shared_ptr<cse::Object> arg);

#endif