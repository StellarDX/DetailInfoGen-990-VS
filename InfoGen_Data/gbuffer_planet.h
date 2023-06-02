#pragma once

#ifndef __GBUF_PLANETS__
#define __GBUF_PLANETS__

#include <CSE/Object.h>
#include "Composite1.h"

struct PlanetParams
{
	cse::float64 MeanRadius;
	cse::float64 EqRadius;
	cse::float64 PolarRadius;
	cse::float64 Flattening;
	cse::float64 EqCircum;
	cse::float64 MeriCircum;
	cse::float64 SurfArea;
	cse::float64 Volume;
	cse::float64 Mass;
	cse::float64 MeanDensity;
	cse::float64 SurfGrav;
	cse::float64 EscapeVel;
	cse::float64 SynodicPeriod;
	cse::float64 RotationPeriod;
	cse::float64 AxialTilt;
	cse::float64 AlbedoBond;
	cse::float64 AlbedoGeom;

	cse::float64 MinTemperature;
	cse::float64 MeanTemperature;
	cse::float64 MaxTemperature;
};

std::string GenPlanetType(std::shared_ptr<cse::Object> Obj);
cse::float64 EarthSimIndex_RRhoVT(std::shared_ptr<cse::Object> Target);

PlanetParams gbuffer_planet(std::shared_ptr<cse::Object> Companion);

#endif