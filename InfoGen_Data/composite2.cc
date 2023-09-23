#include "gbuffer_html.h"
#include "composite.h"
#include "composite1.h"
#include "composite2.h"
#include "final.h"

#include "sorting.h"

using namespace std;
using namespace cse;

namespace Localization
{
	static string C2_Title3    = "List of exceptional asteroids";
	static string C2_Sort0     = "Largest by diameter";
	static string C2_Sort1     = "Most massive";
	static string C2_Sort2     = "Fastest rotators";
	static string C2_Sort3     = "Slowest rotators";
	static string C2_Sort4     = "Retrograde";
	static string C2_Sort5     = "Highly inclined";
	static string C2_AstName   = "Name";
	static string C2_AstParent = "Parent";
	static string C2_AstDiam   = "Diameter (m) (geometric mean)";
	static string C2_AstDim    = "Dimensions";
	static string C2_AstDist   = "Mean distance from parent (m)";
	static string C2_AstMass   = "Mass";
	static string C2_AstRotate = "Rotation period (s)";
	static string C2_AstIncl   = "Inclination";
}

using namespace Localization;

MPSArg MinorPlanetSortArg;
vector<shared_ptr<Object>> MinorPlanetBuffer;

const float64 MaxAsterDiameter = 120000;  // IRAS Standard, in meters
const float64 MaxAsterMass     = 1E18;    // in Kg
const float64 MinAsterRotation = 3600000; // in sec
const float64 MaxAsterRotation = 100;     // in sec

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
	return Obj->Radius() * 2. > MaxAsterDiameter;
}

bool MassLargerThan1E18(shared_ptr<Object> Obj)
{
	return Obj->Mass > MaxAsterMass;
}

bool RPeriodGreaterThan1000h(shared_ptr<Object> Obj)
{
	return Obj->Rotation.RotationPeriod > MinAsterRotation;
}

bool RPeriodLessThan100s(shared_ptr<Object> Obj)
{
	return Obj->Rotation.RotationPeriod < MaxAsterRotation;
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

string HTMLMPDiam(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>" + PrecStr + "</td><td class = \"MinorPlanetTableData\" colspan = 1>(" + PrecStr + " × " + PrecStr + " × " + PrecStr + ")</td><td class = \"MinorPlanetTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
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

string HTMLMPMass(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->ParentBody,
		Obj->Mass
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

string HTMLMPRot(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>" + PrecStr + "</td><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->Rotation.RotationPeriod,
		Obj->ParentBody,
		cse::cbrt(Obj->Dimensions.x * Obj->Dimensions.y * Obj->Dimensions.z)
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

string HTMLMPIncl(shared_ptr<Object> Obj)
{
	string PrecStr = "{:." + to_string(_OUT_PRECISION) + "g}";
	string fmtstr = "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>{}</td><td class = \"MinorPlanetTableData\" colspan = 1>" + PrecStr + "</td></tr>\n";
	return vformat(fmtstr, make_format_args
	(
		Obj->Name[0],
		Obj->ParentBody,
		Obj->Orbit.Inclination
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

// Sort engine caller and comparing functions

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

void HTMLPushMenu(string ObjName, string ObjType);

string HTMLMPList()
{
	ostringstream os;
	os << "\t\t\t" << _Html_Tags::_table_begin << '\n';

	os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 4 class = \"MinorPlanetTableTop\"><a name = \"Exceptional Asteroids\">" + C2_Title3 + "</a></td>";

	HTMLPushMenu("Exceptional Asteroids", "");

	switch (MinorPlanetSortArg)
	{
	case MPS_Diam:
	default:
		os << "<td colspan = 2 class = \"MinorPlanetTableTopEnd\">" + C2_Sort0 + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableSubHead\">" + C2_AstName + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstParent + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstDiam + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstDim + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstDist + "</td></tr>\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, DiameterLargerThan120);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), DiameterCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << HTMLMPDiam(MinorPlanetBuffer[i]);
		}
		os << "\t\t\t\t</table></td></tr>\n";
		break;
	case MPS_Mass:
		os << "<td colspan = 2 class = \"MinorPlanetTableTopEnd\">" + C2_Sort1 + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableSubHead\">" + C2_AstName + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstParent + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstMass + "</td></tr>\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, MassLargerThan1E18);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), MassCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << HTMLMPMass(MinorPlanetBuffer[i]);
		}
		os << "\t\t\t\t</table></td></tr>\n";
		break;
	case MPS_FastRot:
		os << "<td colspan = 2 class = \"MinorPlanetTableTopEnd\">" + C2_Sort2 + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableSubHead\">" + C2_AstName + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstRotate + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstParent + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstDiam + "</td></tr>\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, RPeriodLessThan100s);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), RotLessCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << HTMLMPRot(MinorPlanetBuffer[i]);
		}
		os << "\t\t\t\t</table></td></tr>\n";
		break;
	case MPS_SlowRot:
		os << "<td colspan = 2 class = \"MinorPlanetTableTopEnd\">" + C2_Sort3 + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableSubHead\">" + C2_AstName + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstRotate + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstParent + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstDiam + "</td></tr>\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, RPeriodGreaterThan1000h);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), RotGreaterCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << HTMLMPRot(MinorPlanetBuffer[i]);
		}
		os << "\t\t\t\t</table></td></tr>\n";
		break;
	case MPS_Retro:
		os << "<td colspan = 2 class = \"MinorPlanetTableTopEnd\">" + C2_Sort4 + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableSubHead\">" + C2_AstName + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstParent + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstIncl + "</td></tr>\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, IsRetrograde);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << HTMLMPIncl(MinorPlanetBuffer[i]);
		}
		os << "\t\t\t\t</table></td></tr>\n";
		break;
	case MPS_Incl:
		os << "<td colspan = 2 class = \"MinorPlanetTableTopEnd\">" + C2_Sort5 + "</td></tr>\n";
		os << "\t\t\t\t<tr class = \"MinorPlanetTable\"><td colspan = 6><table class = \"SubTable\" border=\"1\">\n";
		os << "\t\t\t\t\t<tr class = \"SubSysTable\"><td class = \"MinorPlanetTableSubHead\">" + C2_AstName + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstParent + "</td><td class = \"MinorPlanetTableSubHead\">" + C2_AstIncl + "</td></tr>\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, IsHighInclined);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), InclCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << HTMLMPIncl(MinorPlanetBuffer[i]);
		}
		os << "\t\t\t\t</table></td></tr>\n";
		break;
	}
	os << "\t\t\t" << _Html_Tags::_table_end << '\n';

	return os.str();
}

string GHMarkDownMPList()
{
	ostringstream os;
	os << "## " + C2_Title3;
	switch (MinorPlanetSortArg)
	{
	case MPS_Diam:
	default:
		os << " (" + C2_Sort0 + ")\n";
		os << "| " + C2_AstName + " | " + C2_AstParent + " | " + C2_AstDiam + " | " + C2_AstDim + " | " + C2_AstDist + " |\n";
		os << "|:---|:---|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, DiameterLargerThan120);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), DiameterCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPDiam(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_Mass:
		os << " (" + C2_Sort1 + ")\n";
		os << "| " + C2_AstName + " | " + C2_AstParent + " | " + C2_AstMass + " |\n";
		os << "|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, MassLargerThan1E18);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), MassCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPMass(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_FastRot:
		os << " (" + C2_Sort2 + ")\n";
		os << "| " + C2_AstName + " | " + C2_AstRotate + " | " + C2_AstParent + " | " + C2_AstDiam + " |\n";
		os << "|:---|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, RPeriodLessThan100s);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), RotLessCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPRot(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_SlowRot:
		os << " (" + C2_Sort3 + ")\n";
		os << "| " + C2_AstName + " | " + C2_AstRotate + " | " + C2_AstParent + " | " + C2_AstDiam + " |\n";
		os << "|:---|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, RPeriodGreaterThan1000h);
		__Qsort_Objects(MinorPlanetBuffer.begin(), MinorPlanetBuffer.end(), RotGreaterCmp);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPRot(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_Retro:
		os << " (" + C2_Sort4 + ")\n";
		os << "| " + C2_AstName + " | " + C2_AstParent + " | " + C2_AstIncl + " |\n";
		os << "|:---|:---|:---|\n";
		__DFS_SearchMinorPlanets(os, SystemStructure, IsRetrograde);
		for (size_t i = 0; i < MinorPlanetBuffer.size(); i++)
		{
			os << GHMarkDownMPIncl(MinorPlanetBuffer[i]);
		}
		break;
	case MPS_Incl:
		os << "(" + C2_Sort5 + ")\n";
		os << "| " + C2_AstName + " | " + C2_AstParent + " | " + C2_AstIncl + " |\n";
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

void GetLcString(string Key, string* Val);

void GetLocaleComp2()
{
	GetLcString("C2_Title3",    &C2_Title3);
	GetLcString("C2_Sort0",     &C2_Sort0);
	GetLcString("C2_Sort1",     &C2_Sort1);
	GetLcString("C2_Sort2",     &C2_Sort2);
	GetLcString("C2_Sort3",     &C2_Sort3);
	GetLcString("C2_Sort4",     &C2_Sort4);
	GetLcString("C2_Sort5",     &C2_Sort5);
	GetLcString("C2_AstName",   &C2_AstName);
	GetLcString("C2_AstParent", &C2_AstParent);
	GetLcString("C2_AstDiam",   &C2_AstDiam);
	GetLcString("C2_AstDim",    &C2_AstDim);
	GetLcString("C2_AstDist",   &C2_AstDist);
	GetLcString("C2_AstMass",   &C2_AstMass);
	GetLcString("C2_AstRotate", &C2_AstRotate);
	GetLcString("C2_AstIncl",   &C2_AstIncl);
}

void composite2(vector<string> args)
{
	GetLocaleComp2();

	cout << "Generating minor planet list...\n";

	string MPSStr = "diameter";
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 8) == "-mpsort=")
		{
			string encodstr = args[i];
			MPSStr = encodstr.substr(8, encodstr.size() - 8);
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
		if (!Astrobiology)
		{
			HTMLcontent += HTMLMPList();
		}
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