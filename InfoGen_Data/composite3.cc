#include "composite3.h"

#include "gbuffer_planet.h"
#include "composite.h"
#include "composite2.h"
#include "final.h"

using namespace std;
using namespace cse;

vector<shared_ptr<Object>> RockyObjBuf;

namespace Localization
{
	string Asb_Title  = "Habitability of rocky objects in \"{}\" System.";
	string Asb_Name   = "Object Name";
	string Asb_Parent = "Parent";
	string Asb_ESI    = "ESI";
}

using namespace Localization;

bool IsRockyPlanet(shared_ptr<Object> Obj);

void __DFS_FindRockyObj(shared_ptr<SystemStruct> SysTree)
{
	if (SysTree == nullptr) { return; }
	for (size_t i = 0; i < SysTree->Catalog.size(); i++)
	{
		if ((SysTree->Catalog[i].Pointer->Type == "Planet" || SysTree->Catalog[i].Pointer->Type == "DwarfPlanet" 
			|| SysTree->Catalog[i].Pointer->Type == "Moon") && IsRockyPlanet(SysTree->Catalog[i].Pointer))
		{
			RockyObjBuf.push_back(SysTree->Catalog[i].Pointer);
		}
		__DFS_FindRockyObj(SysTree->Catalog[i].SubSystem);
	}
}

string GHMDAstrobiology()
{
	ostringstream os;
	os << vformat("# " + Asb_Title + "\n", make_format_args(SystemBarycenter));
	os << "| " + Asb_Name + " | " + Asb_Parent + " | " + Asb_ESI + " |\n|:---|:---|:---|\n";
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

void GetLcString(string Key, string* Val);

void GetLocalAsB1()
{
	GetLcString("Asb_Title",  &Asb_Title);
	GetLcString("Asb_Name",   &Asb_Name);
	GetLcString("Asb_Parent", &Asb_Parent);
	GetLcString("Asb_ESI",    &Asb_ESI);
}

extern string LcID;
void ParseLocalStrings(string FileName, string LCID, UINT CP);

void composite3(vector<string> args)
{
	// Load localization
	UINT LCodePage = 65001;
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 8) == "-lchset=")
		{
			string lccp = args[i];
			LCodePage = stoul(lccp.substr(8, lccp.size() - 8));
			break;
		}
	}
	cout << "Loading localizations...\n";
	ParseLocalStrings("Astrobiology.cfg", LcID, LCodePage);

	GetLocalAsB1();
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