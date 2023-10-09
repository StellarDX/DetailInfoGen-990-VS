#include "partition.h"

#include "../composite.h"
#include "../composite1.h"
#include "../final.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <set>
#include <CSE/Core/ConstLists.h>
#include <CSE/SCStream.h>

using namespace std;
using namespace cse;
using namespace std::filesystem;

// -------------------- FLAGS --------------------

//            Parameter            Flag                   Sequence in full output
const int8_t  CSVStarSystem      = 0b00000001;         // 0
const int8_t  CSVStarDesignation = 0b00000010;         // 1
const int8_t  CSVStarClass       = 0b00000100;         // 2
const int8_t  CSVStarMass        = 0b00001000;         // 3
const int8_t  CSVStarMagnitude   = 0b00010000;         // 4
const int8_t  CSVStarLumBol      = 0b00100000;         // 5
const int8_t  CSVStarTEff        = 0b01000000;         // 6
const int8_t  CSVStarRadius      = 0b10000000;         // 7
const int8_t  CSVStarMask        = 0b11111111;

const int16_t CSVPlanLabel       = 0b0000000000000001; // 0
const int16_t CSVPlanMass        = 0b0000000000000010; // 1
const int16_t CSVPlanRadius      = 0b0000000000000100; // 2
const int16_t CSVPlanSurfGrav    = 0b0000000000001000; // 3
const int16_t CSVPlanDensity     = 0b0000000000010000; // 4
const int16_t CSVPlanOrbitT      = 0b0000000000100000; // 5
const int16_t CSVPlanOrbitA      = 0b0000000001000000; // 6
const int16_t CSVPlanOrbitE      = 0b0000000010000000; // 7
const int16_t CSVPlanOrbitI      = 0b0000000100000000; // 8
const int16_t CSVPlanTemp        = 0b0000001000000000; // 9
const int16_t CSVPlanDiscMethod  = 0b0000010000000000; // A
const int16_t CSVPlanDiscDate    = 0b0000100000000000; // B
const int16_t CSVPlanParentName  = 0b0001000000000000; // C
const int16_t CSVPlanParentCls   = 0b0010000000000000; // D
const int16_t CSVPlanParentMass  = 0b0100000000000000; // E
const int16_t CSVPlanParentTeff  = 0b1000000000000000; // F
const int16_t CSVPlanMask        = 0b1111111111111111;
const int16_t CSVPlanPsyData     = 0b0000100000011111;
const int16_t CSVPlanOrbData     = 0b0000000111100000;
const int16_t CSVPlanDisData     = 0b0000011000000000;
const int16_t CSVPlanParData     = 0b1111000000000000;

const int8_t  CSVSateName        = 0b00000001; // 0
const int8_t  CSVSateParent      = 0b00000010; // 1
const int8_t  CSVSateRadius      = 0b00000100; // 2
const int8_t  CSVSateOrbitA      = 0b00001000; // 3
const int8_t  CSVSateOrbitT      = 0b00010000; // 4
const int8_t  CSVSateDiscDate    = 0b00100000; // 5
const int8_t  CSVSateMask        = 0b00111111;

// ----------------------------------------------

// 接手的兄弟我知道你看到大片的0和1会很吃惊
// 事实上这只是C/C++语言里头，尤其是系统底层和嵌入式常出现的小把戏而已。
// 你现在看的这段是整个项目唯一用中文写的注释。
// 具体我是什么用意自己去看帮助文档吧，文件名是PARTITION_HELP.md，我承认如果不细说代码连帝弓都看不懂。

// 2023/10/02，青雀。你干吗学太卜啊，我才刚闲下来你就把项目扔过来了。
// 我服了，修了一个Bug又多一个Bug，完全跑不起来
// 2023/10/04 不管了，走了走了，打麻将去了。

// 2023/10/05，符玄
// 这方法到底是谁想出来的，一眼望去全是01，而且很多操作看似非比寻常，真是奇怪...
// 2023/10/07 麻了，本座看了半天才稍微看出些所以然，原想着试下常规操作能不能实现相同功能，没想到比没重构Bug还多。
// 2023/10/08 不管了，告辞。

const int64_t DefSequence        = 0xFEDCBA9876543210; // Little Endian

// -------------------- PRESETS --------------------

struct CSVPreset8 { int8_t _Flags; int32_t _Seq = int32_t(DefSequence & 0xFFFFFFFFLL); };
struct CSVPreset16 { int16_t _Flags; int64_t _Seq = DefSequence; };

const CSVPreset8  CSVStarPreset1 = {CSVStarSystem | CSVStarDesignation | CSVStarClass | CSVStarMass | CSVStarMagnitude,            0x00043210}; // Default preset
const CSVPreset8  CSVStarPreset2 = {CSVStarSystem | CSVStarDesignation | CSVStarLumBol | CSVStarMagnitude | CSVStarTEff,           0x00042310}; // By Luminosity
const CSVPreset8  CSVStarPreset3 = {CSVStarSystem | CSVStarDesignation | CSVStarMass | CSVStarMagnitude | CSVStarTEff,             0x00043210}; // By Mass
const CSVPreset8  CSVStarPreset4 = {CSVStarSystem | CSVStarDesignation | CSVStarRadius | CSVStarClass,                             0x00002310}; // By Radius
const CSVPreset8  CSVStarPreset5 = {CSVStarSystem | CSVStarDesignation | CSVStarTEff | CSVStarMass | CSVStarLumBol | CSVStarClass, 0x00243510}; // By Temperature
const CSVPreset8  CSVStarPresetF = {CSVStarMask, int32_t(DefSequence & 0xFFFFFFFFLL)};

const CSVPreset16 CSVPlanPreset1 = {CSVPlanLabel | CSVPlanMass | CSVPlanRadius | CSVPlanOrbitT | CSVPlanOrbitA | CSVPlanTemp | CSVPlanDiscMethod | CSVPlanParentMass | CSVPlanParentTeff, 0x0000000876543210}; // Default preset
const CSVPreset16 CSVPlanPreset2 = {CSVPlanParentName | CSVPlanParentMass | CSVPlanLabel | CSVPlanMass | CSVPlanRadius | CSVPlanOrbData | CSVPlanDisData,                                 0x0000010A98756432}; // Wikipedia-1
const CSVPreset16 CSVPlanPreset3 = {CSVPlanLabel | CSVPlanDisData | CSVPlanMass | CSVPlanRadius | CSVPlanDensity | CSVPlanOrbitT | CSVPlanOrbitA | CSVPlanOrbitE,                         0x0000000817654320}; // Kepler style
const CSVPreset16 CSVPlanPreset4 = {CSVPlanLabel | CSVPlanMass | CSVPlanOrbData | CSVPlanDiscDate,                                                                                        0x0000000006543210}; // Wikipedia-2
const CSVPreset16 CSVPlanPreset5 = {CSVPlanLabel | CSVPlanMass | CSVPlanRadius | CSVPlanDensity | CSVPlanTemp | CSVPlanOrbData | CSVPlanDiscDate,                                         0x0000009487653210}; // Wikipedia-3
const CSVPreset16 CSVPlanPreset6 = {CSVPlanParentName | CSVPlanLabel | CSVPlanMass | CSVPlanRadius | CSVPlanOrbitT | CSVPlanOrbitA | CSVPlanOrbitE | CSVPlanDiscDate,                     0x0000000007654321}; // Wikipedia-4
const CSVPreset16 CSVPlanPreset7 = {CSVPlanLabel | CSVPlanMass | CSVPlanRadius | CSVPlanSurfGrav | CSVPlanTemp | CSVPlanOrbitA | CSVPlanOrbitE,                                           0x0000000004653210}; // Terrestrial
const CSVPreset16 CSVPlanPresetF = {CSVPlanMask, DefSequence};

const CSVPreset8  CSVSatePresetF = {CSVSateMask, int32_t(DefSequence & 0xFFFFFFLL)};

// -------------------------------------------------

namespace Localization
{
	static string CP_Star_PSys          = "System";
	static string CP_Star_Name          = "Designation";
	static string CP_Star_Class         = "Spectral classification";
	static string CP_Star_Mass          = "Mass (Kg)";
	static string CP_Star_MagBol        = "Bolometric magnitude";
	static string CP_Star_Lum           = "Luminosity (W)";
	static string CP_Star_Teff          = "Temperature (°K)";
	static string CP_Star_MRad          = "Radius (m)";

	static string CP_Planet_Label       = "Name";
	static string CP_Planet_Mass        = "Mass (Kg)";
	static string CP_Planet_MRad        = "Radius (m)";
	static string CP_Planet_SurfGrav    = "Surface gravity (m/s^2)";
	static string CP_Planet_Density     = "Density (Kg/m^3)";
	static string CP_Orbit_P            = "Period (P) (s)";
	static string CP_Orbit_a            = "Semi-major axis (a) (m)";
	static string CP_Orbit_e            = "Eccentricity (e)";
	static string CP_Orbit_i            = "Inclination (i)";
	static string CP_Planet_Temp        = "Temp. (°K)";
	static string CP_Planet_DiscMeth    = "Discovery method";
	static string CP_Planet_DiscDate    = "Discovery year";
	static string CP_Planet_StarName    = "Star";
	static string CP_Planet_StarType    = "Star type";
	static string CP_Planet_StarMass    = "Host star mass (Kg)";
	static string CP_Planet_StarTemp    = "Host star temp. (°K)";

	static string CP_Satellite_Name     = "Name";
	static string CP_Satellite_Parent   = "Parent";
	static string CP_Satellite_Radius   = "Mean Radius (m)";
	static string CP_Satellite_OrbitA   = "Orbital semi-major axis (m)";
	static string CP_Satellite_OrbitP   = "Siderial period (s) (r = retrograde)";
	static string CP_Satellite_DiscDate = "Discovery Year";
};

using namespace Localization;

vector<Object> ObjStarBuffer, ObjPlanetBuffer, ObjMoonBuffer;
bool PartitionReload = false;

template<typename _ITy>
void __Selection_Sort(vector<_ITy>& arr, vector<_ITy>& order, size_t n)
{
	// Reference: https://blog.csdn.net/Roger_717/article/details/120029509
	for (size_t i = 0; i < n; i++)
	{
		order[i] = _ITy(i);
	}
	for (size_t i = 0; i < n; i++)
	{
		_ITy Minkey = arr[i];
		_ITy order_num = order[i];
		for (size_t j = i; j < n; j++)
		{
			if (Minkey > arr[j])
			{
				swap(arr[j], Minkey);
				swap(order[j], order_num);
			}
		}
		arr[i] = Minkey;
		order[i] = order_num;
	}
}

void SortParameters(vector<string>* Buf, uint64 Seq)
{
	auto Size = Buf->size();
	
	// Check sequences
	vector<int8_t> Indices;
	for (size_t i = 0; i < Size; i++)
	{
		int8_t ExactIdx = (Seq & (0xFULL << (i * 4))) >> (i * 4);
		if (find(Indices.begin(), Indices.end(), ExactIdx) != Indices.end()) { cout << vformat("Index {} is already exist.\n", make_format_args(ExactIdx)); }
		else { Indices.push_back(ExactIdx); }
	}
	if (*cse::max(Indices.begin(), Indices.end()) - *cse::min(Indices.begin(), Indices.end()) != Indices.size() - 1)
	{
		vector<int8_t> SortedIndices = Indices, Position;
		Position.resize(Indices.size());
		__Selection_Sort(SortedIndices, Position, Indices.size());
		Indices = Position;
	}
	if (Indices.size() != Size)
	{
		cout << "Indices number is not enough for parameters, auto adding...\n";
		for (int8_t i = *cse::max(Indices.begin(), Indices.end()) + 1; i < Size; ++i)
		{
			Indices.push_back(i);
		}
	}

	vector<string> OriginalArray = *Buf;
	for (size_t i = 0; i < Size; i++)
	{
		(*Buf)[i] = OriginalArray[Indices[i]];
	}
}

void CSVStarOutput(string* _Dst, CSVPreset8 _Preset = CSVStarPreset1)
{
	// Title
	vector<string> Titles;
	if (_Preset._Flags & CSVStarSystem) { Titles.push_back(CP_Star_PSys); }
	if (_Preset._Flags & CSVStarDesignation) { Titles.push_back(CP_Star_Name); }
	if (_Preset._Flags & CSVStarClass) { Titles.push_back(CP_Star_Class); }
	if (_Preset._Flags & CSVStarMass) { Titles.push_back(CP_Star_Mass); }
	if (_Preset._Flags & CSVStarMagnitude) { Titles.push_back(CP_Star_MagBol); }
	if (_Preset._Flags & CSVStarLumBol) { Titles.push_back(CP_Star_Lum); }
	if (_Preset._Flags & CSVStarTEff) { Titles.push_back(CP_Star_Teff); }
	if (_Preset._Flags & CSVStarRadius) { Titles.push_back(CP_Star_MRad); }
	SortParameters(&Titles, _Preset._Seq);

	for (size_t i = 0; i < Titles.size(); i++)
	{
		*_Dst += Titles[i];
		if (i < Titles.size() - 1) { *_Dst += ','; }
	}

	*_Dst += '\n';

	for (auto i : ObjStarBuffer)
	{
		vector<string> Contents;
		if (_Preset._Flags & CSVStarSystem) { Contents.push_back(i.ParentBody); }
		if (_Preset._Flags & CSVStarDesignation) { Contents.push_back(i.Name[0]); }
		if (_Preset._Flags & CSVStarClass) { Contents.push_back(i.SpecClass); }
		if (_Preset._Flags & CSVStarMass) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Mass))); }
		float64 AbsMagn = i.AbsMagn;
		if (isinf(AbsMagn)) { AbsMagn = ToAbsMagn4(i.LumBol); }
		if (_Preset._Flags & CSVStarMagnitude) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(AbsMagn))); }
		if (_Preset._Flags & CSVStarLumBol) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.LumBol))); }
		if (_Preset._Flags & CSVStarTEff) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Teff))); }
		if (_Preset._Flags & CSVStarRadius) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Radius()))); }
		SortParameters(&Contents, _Preset._Seq);

		for (size_t i = 0; i < Contents.size(); i++)
		{
			*_Dst += Contents[i];
			if (i < Contents.size() - 1) { *_Dst += ','; }
		}

		*_Dst += '\n';
	}
}

void __DFS_Find(shared_ptr<SystemStruct> SysTree, string Name, shared_ptr<Object>& OBuf)
{
	if (SysTree == nullptr || OBuf) { return; }
	for (auto i : SysTree->Catalog)
	{
		if (find(i.Pointer->Name.begin(), i.Pointer->Name.end(), Name) != i.Pointer->Name.end())
		{
			OBuf = i.Pointer;
			return;
		}
		__DFS_Find(i.SubSystem, Name, OBuf);
	}
}

void CSVPlanOutput(string* _Dst, ISCStream& SystemIn, vector<string> args, CSVPreset16 _Preset = CSVPlanPreset1)
{
	// Title
	vector<string> Titles;
	if (_Preset._Flags & CSVPlanLabel) { Titles.push_back(CP_Planet_Label); }
	if (_Preset._Flags & CSVPlanMass) { Titles.push_back(CP_Planet_Mass); }
	if (_Preset._Flags & CSVPlanRadius) { Titles.push_back(CP_Planet_MRad); }
	if (_Preset._Flags & CSVPlanSurfGrav) { Titles.push_back(CP_Planet_SurfGrav); }
	if (_Preset._Flags & CSVPlanDensity) { Titles.push_back(CP_Planet_Density); }
	if (_Preset._Flags & CSVPlanOrbitT) { Titles.push_back(CP_Orbit_P); }
	if (_Preset._Flags & CSVPlanOrbitA) { Titles.push_back(CP_Orbit_a); }
	if (_Preset._Flags & CSVPlanOrbitE) { Titles.push_back(CP_Orbit_e); }
	if (_Preset._Flags & CSVPlanOrbitI) { Titles.push_back(CP_Orbit_i); }
	if (_Preset._Flags & CSVPlanTemp) { Titles.push_back(CP_Planet_Temp); }
	if (_Preset._Flags & CSVPlanDiscMethod) { Titles.push_back(CP_Planet_DiscMeth); }
	if (_Preset._Flags & CSVPlanDiscDate) { Titles.push_back(CP_Planet_DiscDate); }
	if (_Preset._Flags & CSVPlanParentName) { Titles.push_back(CP_Planet_StarName); }
	if (_Preset._Flags & CSVPlanParentCls) { Titles.push_back(CP_Planet_StarType); }
	if (_Preset._Flags & CSVPlanParentMass) { Titles.push_back(CP_Planet_StarMass); }
	if (_Preset._Flags & CSVPlanParentTeff) { Titles.push_back(CP_Planet_StarTemp); }
	SortParameters(&Titles, _Preset._Seq);

	for (size_t i = 0; i < Titles.size(); i++)
	{
		*_Dst += Titles[i];
		if (i < Titles.size() - 1) { *_Dst += ','; }
	}

	*_Dst += '\n';

	for (auto i : ObjPlanetBuffer)
	{
		vector<string> Contents;
		if (_Preset._Flags & CSVPlanLabel) { Contents.push_back(i.Name[0]); }
		if (_Preset._Flags & CSVPlanMass) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Mass))); }
		if (_Preset._Flags & CSVPlanRadius) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Radius()))); }
		if (_Preset._Flags & CSVPlanSurfGrav) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(SurfaceGravity(i)))); }
		if (_Preset._Flags & CSVPlanDensity) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(MeanDensity(i)))); }
		if (_Preset._Flags & CSVPlanOrbitT) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Orbit.Period))); }
		if (_Preset._Flags & CSVPlanOrbitA) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Orbit.SemiMajorAxis()))); }
		if (_Preset._Flags & CSVPlanOrbitE) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Orbit.Eccentricity))); }
		if (_Preset._Flags & CSVPlanOrbitI) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Orbit.Inclination))); }
		if (_Preset._Flags & CSVPlanTemp)
		{
			if (!SystemStructure)
			{
				PartitionReload = true;
				cout << "Preparing to reload resources...\n";
				NormalProcess(SystemIn, args);
			}
			shared_ptr<Object> ObjInSys = nullptr;
			__DFS_Find(SystemStructure, i.Name[0], ObjInSys);
			if (!ObjInSys) { cout << "Object is not found. Is it invalid or failed to load?\n"; }
			else { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(ObjInSys->Teff))); }
		}
		if (_Preset._Flags & CSVPlanDiscMethod) { Contents.push_back(i.DiscMethod); }
		if (_Preset._Flags & CSVPlanDiscDate) { Contents.push_back(i.DiscDate); }

		if (_Preset._Flags & CSVPlanParData)
		{
			if (!SystemStructure)
			{
				PartitionReload = true;
				cout << "Preparing to reload resources...\n";
				NormalProcess(SystemIn, args);
			}
			shared_ptr<Object> ObjInSys = nullptr;
			__DFS_Find(SystemStructure, i.Name[0], ObjInSys);
			if (!ObjInSys) { cout << "Object is not found. Is it invalid or failed to load?\n"; }
			else
			{
				shared_ptr<Object> ObjParent = nullptr;
				__DFS_Find(SystemStructure, ObjInSys->ParentBody, ObjParent);
				if (!ObjParent) { cout << "Object is not found. Is it invalid or failed to load?\n"; }

				if (ObjParent->Type == "Star")
				{
					if (_Preset._Flags & CSVPlanParentName) { Contents.push_back(ObjParent->Name[0]); }
					if (_Preset._Flags & CSVPlanParentCls) { Contents.push_back(ObjParent->SpecClass); }
					if (_Preset._Flags & CSVPlanParentMass) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(ObjParent->Mass))); }
					if (_Preset._Flags & CSVPlanParentTeff) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(ObjParent->Teff))); }
				}
				else if (ObjParent->Type == "Planet")
				{
					string CurParent = ObjParent->ParentBody;
					ObjParent = nullptr;
					__DFS_Find(SystemStructure, CurParent, ObjParent);

					if (ObjParent->Type == "Star")
					{
						if (_Preset._Flags & CSVPlanParentName) { Contents.push_back(ObjParent->Name[0]); }
						if (_Preset._Flags & CSVPlanParentCls) { Contents.push_back(ObjParent->SpecClass); }
						if (_Preset._Flags & CSVPlanParentMass) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(ObjParent->Mass))); }
						if (_Preset._Flags & CSVPlanParentTeff) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(ObjParent->Teff))); }
					}
					else if (ObjParent->Type == "Barycenter")
					{
						if (_Preset._Flags & CSVPlanParentName) { Contents.push_back(ObjParent->Name[0]); }
						if (_Preset._Flags & CSVPlanParentCls) { Contents.push_back("(Barycenter)"); }
						if (_Preset._Flags & CSVPlanParentMass) { Contents.push_back("(Barycenter)"); }
						if (_Preset._Flags & CSVPlanParentTeff) { Contents.push_back("(Barycenter)"); }
					}
				}
				else if (ObjParent->Type == "Barycenter")
				{
					if (_Preset._Flags & CSVPlanParentName) { Contents.push_back(ObjParent->Name[0]); }
					if (_Preset._Flags & CSVPlanParentCls) { Contents.push_back("(Barycenter)"); }
					if (_Preset._Flags & CSVPlanParentMass) { Contents.push_back("(Barycenter)"); }
					if (_Preset._Flags & CSVPlanParentTeff) { Contents.push_back("(Barycenter)"); }
				}
			}
		}
		SortParameters(&Contents, _Preset._Seq);

		for (size_t i = 0; i < Contents.size(); i++)
		{
			*_Dst += Contents[i];
			if (i < Contents.size() - 1) { *_Dst += ','; }
		}

		*_Dst += '\n';
	}
}

void CSVSateOutput(string* _Dst, CSVPreset8 _Preset = CSVSatePresetF)
{
	// Title
	vector<string> Titles;
	if (_Preset._Flags & CSVSateName) { Titles.push_back(CP_Satellite_Name); }
	if (_Preset._Flags & CSVSateParent) { Titles.push_back(CP_Satellite_Parent); }
	if (_Preset._Flags & CSVSateRadius) { Titles.push_back(CP_Satellite_Radius); }
	if (_Preset._Flags & CSVSateOrbitA) { Titles.push_back(CP_Satellite_OrbitA); }
	if (_Preset._Flags & CSVSateOrbitT) { Titles.push_back(CP_Satellite_OrbitP); }
	if (_Preset._Flags & CSVSateDiscDate) { Titles.push_back(CP_Satellite_DiscDate); }
	SortParameters(&Titles, _Preset._Seq);

	for (size_t i = 0; i < Titles.size(); i++)
	{
		*_Dst += Titles[i];
		if (i < Titles.size() - 1) { *_Dst += ','; }
	}

	*_Dst += '\n';

	for (auto i : ObjMoonBuffer)
	{
		vector<string> Contents;
		if (_Preset._Flags & CSVSateName) { Contents.push_back(i.Name[0]); }
		if (_Preset._Flags & CSVSateParent) { Contents.push_back(i.ParentBody); }
		if (_Preset._Flags & CSVSateRadius) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Radius()))); }
		if (_Preset._Flags & CSVSateOrbitA) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g}", make_format_args(i.Orbit.SemiMajorAxis()))); }
		if (_Preset._Flags & CSVSateOrbitT) { Contents.push_back(vformat("{:." + to_string(_OUT_PRECISION) + "g} {}", make_format_args(i.Orbit.Period, cse::abs(i.Orbit.Period) > 90 && cse::abs(i.Orbit.Period) < 270 ? "(r)" : ""))); }
		if (_Preset._Flags & CSVSateDiscDate) { Contents.push_back(i.DiscDate); }
		SortParameters(&Contents, _Preset._Seq);

		for (size_t i = 0; i < Contents.size(); i++)
		{
			*_Dst += Contents[i];
			if (i < Contents.size() - 1) { *_Dst += ','; }
		}

		*_Dst += '\n';
	}
}

void GetLocalePartition()
{
	void GetLcString(string Key, string * Val);

	GetLcString("CP_Star_PSys",          &CP_Star_PSys);
	GetLcString("CP_Star_Name",          &CP_Star_Name);
	GetLcString("C1_Star_MagBol",        &CP_Star_MagBol);
	GetLcString("C1_Star_Class",         &CP_Star_Class);
	GetLcString("C1_Star_Mass",          &CP_Star_Mass);
	GetLcString("C1_Star_Teff",          &CP_Star_Teff);
	GetLcString("C1_Star_Lum",           &CP_Star_Lum);
	GetLcString("CP_Star_MRad",          &CP_Star_MRad);

	GetLcString("CP_Planet_Label",       &CP_Planet_Label);
	GetLcString("C1_Planet_Mass",        &CP_Planet_Mass);
	GetLcString("C1_Planet_MRad",        &CP_Planet_MRad);
	GetLcString("C1_Planet_SurfGrav",    &CP_Planet_SurfGrav);
	GetLcString("C1_Planet_Density",     &CP_Planet_Density);
	GetLcString("C1_Orbit_P1",           &CP_Orbit_P);
	GetLcString("C1_Orbit_a",            &CP_Orbit_a);
	GetLcString("C1_Orbit_e",            &CP_Orbit_e);
	GetLcString("C1_Orbit_i",            &CP_Orbit_i);
	GetLcString("C1_Planet_Temp",        &CP_Planet_Temp);
	GetLcString("CP_Planet_DiscMeth",    &CP_Planet_DiscMeth);
	GetLcString("CP_Planet_DiscDate",    &CP_Planet_DiscDate);
	GetLcString("CP_Planet_StarName",    &CP_Planet_StarName);
	GetLcString("CP_Planet_StarType",    &CP_Planet_StarType);
	GetLcString("CP_Planet_StarMass",    &CP_Planet_StarMass);
	GetLcString("CP_Planet_StarTemp",    &CP_Planet_StarTemp);

	GetLcString("CP_Satellite_Name",     &CP_Satellite_Name);
	GetLcString("CP_Satellite_Parent",   &CP_Satellite_Parent);
	GetLcString("CP_Satellite_Radius",   &CP_Satellite_Radius);
	GetLcString("CP_Satellite_OrbitA",   &CP_Satellite_OrbitA);
	GetLcString("CP_Satellite_OrbitP",   &CP_Satellite_OrbitP);
	GetLcString("CP_Satellite_DiscDate", &CP_Satellite_DiscDate);
}

void partition(ISCStream& SystemIn, vector<string> args)
{
	UINT LCodePage = 65001;
	CSVPreset8 StarPreset = CSVStarPreset1;
	CSVPreset16 PlanPreset = CSVPlanPreset1;
	CSVPreset8 SatePreset = CSVSatePresetF;
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i].substr(0, 8) == "-lchset=")
		{
			string lccp = args[i];
			LCodePage = stoul(lccp.substr(8, lccp.size() - 8));
		}

		else if (args[i].substr(0, 11) == "-starflags=")
		{
			string _Res = args[i].substr(11, args[i].size() - 11);
			if (_Res == "pre1") { StarPreset = CSVStarPreset1; }
			else if (_Res == "pre2") { StarPreset = CSVStarPreset2; }
			else if (_Res == "pre3") { StarPreset = CSVStarPreset3; }
			else if (_Res == "pre4") { StarPreset = CSVStarPreset4; }
			else if (_Res == "pre5") { StarPreset = CSVStarPreset5; }
			else if (_Res == "full") { StarPreset = CSVStarPresetF; }
			else { StarPreset._Flags = (int8_t)stoi(_Res, nullptr, 2) & CSVStarMask; }
		}
		else if (args[i].substr(0, 9) == "-starseq=")
		{
			string _Res = args[i].substr(9, args[i].size() - 9);
			StarPreset._Seq = stol(_Res, nullptr, 16);
		}

		else if (args[i].substr(0, 11) == "-planflags=")
		{
			string _Res = args[i].substr(11, args[i].size() - 11);
			if (_Res == "pre1") { PlanPreset = CSVPlanPreset1; }
			else if (_Res == "pre2") { PlanPreset = CSVPlanPreset2; }
			else if (_Res == "pre3") { PlanPreset = CSVPlanPreset3; }
			else if (_Res == "pre4") { PlanPreset = CSVPlanPreset4; }
			else if (_Res == "pre5") { PlanPreset = CSVPlanPreset5; }
			else if (_Res == "pre6") { PlanPreset = CSVPlanPreset6; }
			else if (_Res == "pre7") { PlanPreset = CSVPlanPreset7; }
			else if (_Res == "full") { PlanPreset = CSVPlanPresetF; }
			else { PlanPreset._Flags = (int16_t)stoi(_Res, nullptr, 2) & CSVPlanMask; }
		}
		else if (args[i].substr(0, 9) == "-planseq=")
		{
			string _Res = args[i].substr(9, args[i].size() - 9);
			PlanPreset._Seq = stoll(_Res, nullptr, 16);
		}

		else if (args[i].substr(0, 11) == "-sateflags=")
		{
			string _Res = args[i].substr(11, args[i].size() - 11);
			SatePreset._Flags = (int8_t)stoi(_Res, nullptr, 2) & CSVSateMask;
		}
		else if (args[i].substr(0, 9) == "-sateseq=")
		{
			string _Res = args[i].substr(9, args[i].size() - 9);
			SatePreset._Seq = stol(_Res, nullptr, 16);
		}
	}

	cout << "Separating objects...\n";
	auto it = SystemIn->Catalogue.begin(), end = SystemIn->Catalogue.end();
	while (it != end)
	{
		if (it->Key == "Star")
		{
			ObjStarBuffer.push_back(ObjectLoader(it));
		}
		else if (it->Key == "Planet" || it->Key == "DwarfPlanet")
		{
			ObjPlanetBuffer.push_back(ObjectLoader(it));
		}
		else if (it->Key == "Moon")
		{
			ObjMoonBuffer.push_back(ObjectLoader(it));
		}
		++it;
	}
	cout << "DONE.\n";

	cout << "Loading localizations...\n";
	ParseLocalStrings("SystemInfo.cfg", LcID, LCodePage);
	GetLocalePartition();

	if (!exists(OutputFileName))
	{
		if (MakeDir)
		{
			cout << "Output directory is not exist, creating...\n";
			if (!create_directory(OutputFileName))
			{
				cout << "Failed to create directory.\n";
				exit(114514);
			}
		}
		else
		{
			cout << "Output directory is not exist.\n";
			exit(114514);
		}
	}

	if (OutputFileName.back() != '/') { OutputFileName.push_back('/'); }

	path ObjStarPath = OutputFileName, ObjPlanetPath = OutputFileName, ObjMoonPath = OutputFileName;

	ObjStarPath += "Stars.csv";
	ObjPlanetPath += "Planets.csv";
	ObjMoonPath += "Moons.csv";

	string ObjStarString, ObjPlanetString, ObjMoonString;

	CSVStarOutput(&ObjStarString, StarPreset);
	CSVPlanOutput(&ObjPlanetString, SystemIn, args, PlanPreset);
	CSVSateOutput(&ObjMoonString, SatePreset);

	Transcode(ObjStarString, outencoding);
	Transcode(ObjPlanetString, outencoding);
	Transcode(ObjMoonString, outencoding);

	ofstream StarFOut(ObjStarPath), PlanFOut(ObjPlanetPath), MoonFOut(ObjMoonPath);
	StarFOut << ObjStarString;
	PlanFOut << ObjPlanetString;
	MoonFOut << ObjMoonString;
	StarFOut.close();
	PlanFOut.close();
	MoonFOut.close();

	cout << "File is output at " + OutputFileName << '\n';

	exit(0);
}
