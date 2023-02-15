#pragma once

#ifndef __COMPOSITE2__
#define __COMPOSITE2__

#include <vector>

enum MPSArg
{
	MPS_Diam,
	MPS_Mass,
	MPS_FastRot,
	MPS_SlowRot,
	MPS_Retro,
	MPS_Incl
};

extern MPSArg MinorPlanetSortArg;

void composite2(std::vector<std::string> args);

#endif