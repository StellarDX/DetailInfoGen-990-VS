#pragma once

#ifndef __COMPOSITE2__
#define __COMPOSITE2__

#include <vector>
#include <CSE/Object.h>

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

bool DiameterLargerThan120(std::shared_ptr<cse::Object> Obj);
bool MassLargerThan1E18(std::shared_ptr<cse::Object> Obj);

void composite2(std::vector<std::string> args);

#endif