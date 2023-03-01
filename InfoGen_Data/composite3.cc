#include "composite3.h"

#include "gbuffer_planet.h"
#include "composite.h"
#include "composite2.h"
#include "final.h"

#include <vector>

using namespace std;
using namespace cse;

vector<shared_ptr<Object>> RockyObjBuf;

void __DFS_FindRockyObj(shared_ptr<SystemStruct> SysTree)
{
	if (SysTree == nullptr) { return; }
	for (size_t i = 0; i < SysTree->Catalog.size(); i++)
	{
		if (SysTree->Catalog[i].Pointer->Type == "Planet" || SysTree->Catalog[i].Pointer->Type == "DwarfPlanet" || SysTree->Catalog[i].Pointer->Type == "Moon")
		{
			RockyObjBuf.push_back(SysTree->Catalog[i].Pointer);
		}
		__DFS_FindRockyObj(SysTree->Catalog[i].SubSystem);
	}
}

enum HabRank{ACE, SSS, SS, S, A, B, C, D};
static const string HabRankStr[8]{ "ACE", "SSS", "SS", "S", "A", "B", "C", "D" };

string GHMDAstrobiology()
{
	ostringstream os;
	os << vformat("# Habitability of rocky objects in \"{}\" System.\n", make_format_args(SystemBarycenter));
	os << "| Object Name | Parent | ESI |\n|:---|:---|:---|\n";
	string FmtString = "| {} | {} | {:." + to_string(_OUT_PRECISION) + "g} |\n";
	for (size_t i = 0; i < RockyObjBuf.size(); i++)
	{
		os << vformat(FmtString, make_format_args
		(
			RockyObjBuf[i]->Name[0],
			RockyObjBuf[i]->ParentBody,
			EarthSimIndex_RRhoVT(RockyObjBuf[i])
		));
	}
	return os.str();
}

void composite3()
{
	__DFS_FindRockyObj(SystemStructure);

	switch (OFormat)
	{
	case HTML:
		break;
	case MD:
	default:
		Final += GHMDAstrobiology();
		break;
	}
}