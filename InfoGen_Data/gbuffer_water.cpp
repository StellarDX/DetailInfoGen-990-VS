#include "gbuffer_water.h"

using namespace std;
using namespace cse;

Ocean gbuffer_water(shared_ptr<Object> Obj)
{
	return
	{
		.Height = Obj->Ocean.Height,
		.Composition = Obj->Ocean.Composition
	};
}