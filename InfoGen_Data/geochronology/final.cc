#include "composite.h"
#include "final.h"

#include <iostream>
#include <CSE/Object.h>

using namespace std;
using namespace cse;

ISCStream SysIn;
string Final;

bool CustomModel          = false;
enum OutputFormat OFormat = MD;
UINT srcencoding          = 65001;
UINT outencoding          = 65001;
string OutputFileName;
string CSSPath;
enum LinkCSS LCSS         = Static;

string LcID               = "1033";
map<string, string> LocStrings;

enum CopyOption Cpm       = Asking;
string HTMLhead;
string HTMLcontent;
string HTMLMenu;
int CSSEncod              = 65001;

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

void ParseLocalStrings(string FileName, string LCID, UINT CP)
{
	ISCStream LCIS;
	bool FileExist = true;
	try { LCIS = ParseFile("InfoGen_Data\\geochronology\\" + LCID + "\\" + FileName, CP); }
	catch (const std::exception&)
	{
		cout << "Localization is not specified or file not exist, using default strings.\n";
		FileExist = false;
	}

	if (!FileExist) { return; }

	string InternationalName = sc::GetAs<string>(LCIS, "InternationalName");
	string LocalName = sc::GetAs<string>(LCIS, "LocalName");
	cout << "Enabled localization: " + LocalName + '(' + InternationalName + ')' + '\n';

	auto it = LCIS->find("Translations");
	if (it == LCIS->end())
	{
		cout << "Invalid localization file.\n";
		abort();
	}

	auto Translations = it->SubTable;
	for (size_t i = 0; i < Translations->Catalogue.size(); i++)
	{
		LocStrings.insert({ Translations->Catalogue[i].Key, sc::GetAs<string>(Translations, Translations->Catalogue[i].Key) });
	}
}

void ReceiveBufferData(HINSTANCE hInstance)
{
	char DBuf[0x7FFF], OFname[200], LocID[5];
	void (*LoadDataFPtr)(char* _DBuf, UINT* _OFmt, UINT* _Scp, UINT* _Ocp, char* _OFName, char* _LCID)
		= (void (*)(char*, UINT*, UINT*, UINT*, char*, char*))GetProcAddress(hInstance, "LoadBasicParams");
	LoadDataFPtr(DBuf, (UINT*)(&OFormat), &srcencoding, &outencoding, OFname, LocID);

	Final = DBuf;
	OutputFileName = OFname;
	LcID = LocID;

	cout << "Data loaded : \n"
		<< "\tBuffer Content : \n" << Final << '\n'
		<< "\tOutput Format : " << OFormat << '\n'
		<< "\tSource encoding : " << srcencoding << '\n'
		<< "\tOutput encoding : " << outencoding << '\n'
		<< "\tOutput File Name : " << OutputFileName << '\n'
		<< "\tLCID : " << LcID << '\n';

	if (OFormat == HTML)
	{
		char CPath[200];
		void (*LoadHTMLFPtr)(int* _LnkCSS, char* _CPath, int* _Cpm)
			= (void (*)(int*, char*, int*))GetProcAddress(hInstance, "LoadHTMLParams");
		LoadHTMLFPtr((int*)(&LCSS), CPath, (int*)&Cpm);
		CSSPath = CPath;
	}
}

void SendBufferData(HINSTANCE hInstance)
{
	void (*SendDataFPtr)(string _Buf, int _OFmt, UINT _ICP, UINT _OCP, string _OFName, string _LCID)
		= (void (*)(string, int, UINT, UINT, string, string))GetProcAddress(hInstance, "SendBasicParams");
	SendDataFPtr(Final, OFormat, srcencoding, outencoding, OutputFileName, LcID);
}

int main(int argc, char const* argv[]) 
{
	vector<string> args;
	for (size_t i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

	string ObjectName, ParentName;

	UINT LCodePage = 65001;

	string CMFName;
	uint64 CMEncoding = 65001;
	for (size_t i = 0; i < args.size(); i++)
	{
		// Seed
		if (args[i].substr(0, 6) == "-seed=")
		{
			string encodstr = args[i];
			random.seed(stoull(encodstr.substr(6, encodstr.size() - 6), nullptr, 16));
		}

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

	// Receiving Buffer data

	cout << "Loading data buffer...\n";
	auto DataBufferhInst = LoadLibraryA(_DATABUFFER_PATH_INNER);
	if (DataBufferhInst)
	{
		cout << "Data buffer is found in address " << DataBufferhInst << "\n";
		ReceiveBufferData(DataBufferhInst);
	}
	else { cout << "Failed to load data buffer.\n"; }

	cout << "Loading localizations...\n";
	ParseLocalStrings("Geochronology.cfg", LcID, LCodePage);

	if (ObjectName.empty())
	{
		cout << "Object is unknown.\n";
		abort();
	}

	if (!CMFName.empty()) { LoadCustomModel(CMFName, CMEncoding); }

	// Find object

	try { SysIn = ParseFile(args[1], srcencoding); }
	catch (exception e)
	{
		cout << e.what();
		abort();
	}
	Object Target = GetSEObject(SysIn, ObjectName);
	Object Parent;
	if (ParentName.empty()) { Parent = GetSEObject(SysIn, Target.ParentBody); }
	else { Parent = GetSEObject(SysIn, ParentName); }

	if (!(Target.Type != "Jupiter" && Target.Type != "GasGiant" && Target.Type != "Neptune" && Target.Type != "IceGiant") &&
		Target.NoOcean && Target.NoAtmosphere &&
		!(Target.Life[0].Class == "Organic" && Target.Life[0].Type == "Multicellular") &&
		!(Target.Life[1].Class == "Organic" && Target.Life[1].Type == "Multicellular"))
	{
		cout << Target.Name[0] + " is not match the requirements.\n";
		abort();
	}

	composite0geo(Target, Parent);

	if (OFormat == HTML) { HTMLWrite(&Final, HTMLhead, HTMLMenu, HTMLcontent); }

	SendBufferData(DataBufferhInst);
}