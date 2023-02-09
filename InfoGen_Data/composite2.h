#pragma once

#ifndef __COMPOSITE2__
#define __COMPOSITE2__

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

void composite2(int argc, char const* argv[]);

#endif