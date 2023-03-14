#include "composite.h"
#include "final.h"
#include "../final.h"

#include <CSE/Object.h>

using namespace std;
using namespace cse;

ISCStream SysIn;
bool CustomModel = false;

vec2 LoadEonRange(ISCStream& is, string Key)
{
	if (is->find(Key) == is->end()) { return vec2(wrtval(POS_INF_DOUBLE)); }
	auto it = is->find(Key);
	if (find(it->Value.begin(), it->Value.end(), '(') != it->Value.end())
	{
		return is->GetQualified(Key)->as<vec2, float64>()->get();
	}
	else { return vec2(is->GetQualified(Key)->as<float64>()->get(), wrtval(POS_INF_DOUBLE)); }
}

void SeparateTimeEvent(ISCStream& is, string Key, std::map<cse::float64, std::string>* MPtr, float64* EndYr = nullptr)
{
	if (is->find(Key) == is->end()) { return ; }
	auto it = is->find(Key)->SubTable->begin();
	auto end = is->find(Key)->SubTable->end();
	for (; it != end; ++it)
	{
		if (it->Key != "Event") { continue; }
		string Val = it->Value;
		
		enum{Normal, InVec, InStr} State = Normal;
		bool EndTime = false;
		string Time;
		string Event;

		auto it2 = Val.begin();
		auto end2 = Val.end();

		while (it2 != end2)
		{
			if (it2 != end2 && *it2 == '(' && State == Normal) { State = InVec; }
			else if (it2 != end2 && *it2 == ')' && State == InVec) { State = Normal; }
			if (it2 != end2 && *it2 == '\"' && State == Normal) { State = InStr; }
			else if (it2 != end2 && *it2 == '\"' && State == InStr) { State = Normal; }
			
			if (it2 != end2 && (isspace(*it2) || ',' == *it2) && State == Normal)
			{
				++it2;
				continue;
			}

			if (it2 != end2 && (!isspace(*it2) || State == InVec || State == InStr) && *it2 != '\"')
			{
				if (!EndTime) { Time += *it2; }
				else { Event += *it2; }
			}

			++it2;
			if (it2 != end2 && State == Normal && (isspace(*it2) || *it2 == '\"')) { EndTime = true; }
		}

		sc::ValueStr Value1(Time);
		sc::ValueStr Value2(Event);
		float64 TimePt;

		if (find(Time.begin(), Time.end(), '(') != Time.end())
		{
			vec2 MinMax = Value1.as<vec2, float64>()->get();
			TimePt = random.uniform(MinMax.x, MinMax.y);
		}
		else { TimePt = Value1.as<float64>()->get(); }
		if (EndYr) { *EndYr = max(*EndYr, TimePt); }
		MPtr->insert({ TimePt, Event });
	}
}

void LoadCustomModel(string FName, uint64 Encod)
{
	ISCStream is;
	try{is = ParseFile(FName, (UINT)Encod);}
	catch (const std::exception& e)
	{
		cout << e.what() << '\n';
		abort();
	}
	CustomModel = true;

	// Get Ranges for eras or eons
	__Hadean_Eon_Range = LoadEonRange(is, "HadeanEon");
	__Archean_Eon_Range = LoadEonRange(is, "ArcheanEon");
	__Proterozoic_Eon_Range = LoadEonRange(is, "ProterozoicEon");
	__Phanerozoic_Eon_Start = LoadEonRange(is, "PhanerozoicEon");
	SeparateTimeEvent(is, "HadeanEon", &__Hadean_Eon_Timeline, &EndYear);
	SeparateTimeEvent(is, "ArcheanEon", &__Archean_Eon_Timeline, &EndYear);
	SeparateTimeEvent(is, "ProterozoicEon", &__Proterozoic_Eon_Timeline, &EndYear);
	if (is->find("PhanerozoicEon") != is->end())
	{
		auto it = is->find("PhanerozoicEon");
		__Palaeozoic_Era_Range = LoadEonRange(it->SubTable, "PaleozoicEra");
		__Mesozoic_Era_Range = LoadEonRange(it->SubTable, "MesozoicEra");
		__Cenozoic_Era_Begin = LoadEonRange(it->SubTable, "CenozoicEra");
		SeparateTimeEvent(it->SubTable, "PaleozoicEra", &__Palaeozoic_Era_Timeline, &EndYear);
		SeparateTimeEvent(it->SubTable, "MesozoicEra", &__Mesozoic_Era_Timeline, &EndYear);
		SeparateTimeEvent(it->SubTable, "CenozoicEra", &__Cenozoic_Era_Timeline, &EndYear);
	}

	SeparateTimeEvent(is, "CivilizationEvents", &__Civilization_Timeline, &EndCivil);
	EndYear += EndCivil;
	EndStr = is->GetQualified("EndString")->as<string>()->get();
}

extern string LcID;
void ParseLocalStrings(string FileName, string LCID, UINT CP);

void geochronology(ISCStream& SystemIn, vector<string> args)
{
	string ObjectName, ParentName;
	SysIn = SystemIn;

	UINT LCodePage = 65001;

	string CMFName;
	uint64 CMEncoding = 65001;
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 8) == "-lchset=")
		{
			string lccp = args[i];
			LCodePage = stoul(lccp.substr(8, lccp.size() - 8));
		}

		// Find ObjectName and ParentName
		else if (args[i] == "-target" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid object name." << '\n';
				abort();
			}
			ObjectName = args[i + 1];
		}
		else if (args[i] == "-parent" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid parent name." << '\n';
				abort();
			}
			ParentName = args[i + 1];
		}

		// Load Custom Model
		else if (args[i].substr(0, 7) == "-evmcp=")
		{
			string evstr = args[i];
			CMEncoding = stoull(evstr.substr(7, evstr.size() - 7));
		}
		else if (args[i] == "-evmdl" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid input filename." << '\n';
				abort();
			}
			CMFName = args[i + 1];
		}
	}

	cout << "Loading localizations...\n";
	ParseLocalStrings("Geochronology.cfg", LcID, LCodePage);

	if (ObjectName.empty())
	{
		cout << "Object is unknown.\n";
		abort();
	}

	if (!CMFName.empty()) { LoadCustomModel(CMFName, CMEncoding); }

	// Find object

	Object Target = GetSEObject(SystemIn, ObjectName);
	Object Parent;
	if (ParentName.empty()) { Parent = GetSEObject(SystemIn, Target.ParentBody); }
	else { Parent = GetSEObject(SystemIn, ParentName); }

	if (!(Target.Type != "Jupiter" && Target.Type != "GasGiant" && Target.Type != "Neptune" && Target.Type != "IceGiant") &&
		Target.NoOcean && Target.NoAtmosphere &&
		!(Target.Life[0].Class == "Organic" && Target.Life[0].Type == "Multicellular") &&
		!(Target.Life[1].Class == "Organic" && Target.Life[1].Type == "Multicellular"))
	{
		cout << Target.Name[0] + " is not match the requirements.\n";
		abort();
	}

	composite0geo(Target, Parent);

	if (OFormat == HTML) { HTMLWrite(); }
}