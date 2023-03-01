#include "composite.h"
#include "composite1.h"
#include "composite2.h"
#include "final.h"

#include "sorting.h"

using namespace std;
using namespace cse;

MPSArg MinorPlanetSortArg;
vector<shared_ptr<Object>> MinorPlanetBuffer;

void __DFS_SearchMinorPlanets(ostream& os, shared_ptr<SystemStruct> SysTree, function<bool(shared_ptr<Object>)> _PFunc)
{
	if (SysTree == nullptr) { return; }
	for (size_t i = 0; i < SysTree->Catalog.size(); i++)
	{
		if ((SysTree->Catalog[i].Pointer->Type == "Asteroid" || SysTree->Catalog[i].Pointer->Type == "DwarfPlanet" || SysTree->Catalog[i].Pointer->Type == "Comet") && _PFunc(SysTree->Catalog[i].Pointer))
		{
			MinorPlanetBuffer.push_back(SysTree->Catalog[i].Pointer);
		}
		__DFS_SearchMinorPlanets(os, SysTree->Catalog[i].SubSystem, _PFunc);
	}
}

// Standard functions

bool DiameterLargerThan120(shared_ptr<Object> Obj)
{
	return Obj->Radius() * 2. > 120000;
}

bool MassLargerThan1E18(shared_ptr<Object> Obj)
{
	return Obj->Mass > 1E18;
}

bool RPeriodGreaterThan1000h(shared_ptr<Object> Obj)
{
	return Obj->Rotation.RotationPeriod > 3600000;
}

bool RPeriodLessThan100s(shared_ptr<Object> Obj)
{
	return Obj->Rotation.RotationPeriod < 100;
}

bool IsRetrograde(shared_ptr<Object> Obj)
{
	return (Obj->Orbit.Inclination > 90 && Obj->Orbit.Inclination <= 270) ||
		(Obj->Orbit.Inclination < -90 && Obj->Orbit.Inclination >= -270);
}

bool IsHighInclined(shared_ptr<Object> Obj)
{
	return (Obj->Orbit.Inclination > 10 && Obj->Orbit.Inclination <= 90) ||
		(Obj->Orbit.Inclination < -10 && Obj->Orbit.Inclination >= -90);
}

// Output templates

string GHMarkDownMPDiam(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "| {} | {} | " + PrecStr + " | (" + PrecStr + " × " + PrecStr + " × " + PrecStr + ") | " + PrecStr + " |\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->ParentBody,
		cse::cbrt(Obj->Dimensions.x * Obj->Dimensions.y * Obj->Dimensions.z),
		Obj->Dimensions.x,
		Obj->Dimensions.y,
		Obj->Dimensions.z,
		Obj->Orbit.SemiMajorAxis()
	));
}

string GHMarkDownMPMass(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "| {} | {} | " + PrecStr + " |\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->ParentBody,
		Obj->Mass
	));
}

string GHMarkDownMPRot(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "| {} | " + PrecStr + " | {} | " + PrecStr + " |\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->Rotation.RotationPeriod,
		Obj->ParentBody,
		cse::cbrt(Obj->Dimensions.x * Obj->Dimensions.y * Obj->Dimensions.z)
	));
}

string GHMarkDownMPIncl(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "| {} | {} | " + PrecStr + " |\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->ParentBody,
		Obj->Orbit.Inclination
	));
}

// Sort engine caller and compareing functions

void _cdecl __Qsort_Objects(const vector<shared_ptr<Object>>::iterator First, const vector<shared_ptr<Object>>::iterator Last, function<bool(shared_ptr<Object>, shared_ptr<Object>)> Cmp)
{
	_Adl_verify_range(First, Last);
	const auto UFirst = _Get_unwrapped(First);
	const auto ULast = _Get_unwrapped(Last);
	__Smart_Sort_Engine(UFirst, ULast, ULast - UFirst, Cmp);
}

bool DiameterCmp(shared_ptr<Object> Left, shared_ptr<Object> Right)
{
	return Left->Radius() * 2. > Right->Radius() * 2.;
}

bool MassCmp(shared_ptr<Object> Left, shared_ptr<Object> Right)
{
	return Left->Mass > Right->Mass;
}

bool RotGreaterCmp(shared_ptr<Object> Left, shared_ptr<Object> Right)
{
	return Left->Rotation.RotationPeriod > Right->Rotation.RotationPeriod;
}

bool RotLessCmp(shared_ptr<Object> Left, shared_ptr<Object> Right)
{
	return Left->Rotation.RotationPeriod < Right->Rotation.RotationPeriod;
}

bool InclCmp(shared_ptr<Object> Left, shared_ptr<Object> Right)
{
	return Left->Orbit.Inclination < Right->Orbit.Inclination;
}

string GHMarkDownMPList()
{
	ostringstream os;
	os << "## List of exceptional asteroids";
	switch (MinorPlanetSortArg)
	{
	case MPS_Diam:
	default:
		os << " (Largest by diameter)\n";
		os << "| Name | Parent | Diameter (m) (geometric mean) | Dimensions | Mean distance from parent (m) |\n";
		os << "|:---|:---|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, DiameterLargerThan120);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), DiameterCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPDiam(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_Mass:
		os << " (Most massive)\n";
		os << "| Name | Parent | Mass |\n";
		os << "|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, MassLargerThan1E18);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), MassCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPMass(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_FastRot:
		os << " (Fastest rotators)\n";
		os << "| Name | Rotation period (s) | Parent | Diameter (m) |\n";
		os << "|:---|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, RPeriodLessThan100s);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), RotLessCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPRot(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_SlowRot:
		os << " (Slowest rotators)\n";
		os << "| Name | Rotation period (s) | Parent | Diameter (m) |\n";
		os << "|:---|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, RPeriodGreaterThan1000h);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), RotGreaterCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPRot(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_Retro:
		os << " (Retrograde)\n";
		os << "| Name | Parent | Inclination |\n";
		os << "|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, IsRetrograde);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPIncl(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_Incl:
		os << "(Highly inclined)\n";
		os << "| Name | Parent | Inclination |\n";
		os << "|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, IsHighInclined);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), InclCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPIncl(MinorPlanetBuffer[i]);
		}
		break;
	}

	return os.str();
}

/////////////////////////MAIN//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void composite2(vector<string> args)
{
	cout << "Generating minor planet list...\n";

	string MPSStr = "diameter";
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 17) == "-minorplanetsort=")
		{
			string encodstr = args[i];
			MPSStr = encodstr.substr(17, encodstr.size() - 17);
			break;
		}
	}

	if (MPSStr == "diameter") { MinorPlanetSortArg = MPS_Diam; }
	else if (MPSStr == "mass") { MinorPlanetSortArg = MPS_Mass; }
	else if (MPSStr == "slowrotator") { MinorPlanetSortArg = MPS_SlowRot; }
	else if (MPSStr == "fastrotator") { MinorPlanetSortArg = MPS_FastRot; }
	else if (MPSStr == "retrograde") { MinorPlanetSortArg = MPS_Retro; }
	else if (MPSStr == "highinclined") { MinorPlanetSortArg = MPS_Incl; }

	switch (OFormat)
	{
	case HTML:
		break;
	case MD:
	default:
		if (!Astrobiology)
		{
			Final += GHMarkDownMPList();
		}
		break;
	}
}