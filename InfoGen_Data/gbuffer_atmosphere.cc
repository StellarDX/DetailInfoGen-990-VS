#include "gbuffer_atmosphere.h"

using namespace std;
using namespace cse;

Atmosphere gbuffer_atmosphere(shared_ptr<Object> Obj)
{
	return
	{
		.SurfPressure = Obj->Atmosphere.Pressure,
		.Composition = Obj->Atmosphere.Composition
	};
}