#include "composite.h"

#include "../final.h"

using namespace std;
using namespace cse;

#define HadeanPhases 4
array<float64, HadeanPhases> __Hadean_Eon_Timeline;

#define ArcheanPhases 3
array<float64, ArcheanPhases> __Archean_Eon_Timeline;

#define ProterozoicPhases 5
array<float64, ProterozoicPhases> __Proterozoic_Eon_Timeline;

#define PalaeozoicPhases 21
array<float64, PalaeozoicPhases> __Palaeozoic_Era_Timeline;

#define MesozoicPhases 13
array<float64, MesozoicPhases> __Mesozoic_Era_Timeline;

#define CenozoicPhases 12
array<float64, CenozoicPhases> __Cenozoic_Era_Timeline;

float64 TotalAge;
bool HasMoon = false;

float64 FindAge(Object Target, Object Parent)
{
	if (!isinf(Parent.Age)) { return Parent.Age; }
	else
	{
		auto it = SysIn->begin();
		auto end = SysIn->end();
		while (it != end)
		{
			Object Obj = ObjectLoader(it);
			if (!isinf(Obj.Age)) { return Obj.Age; }
			++it;
		}
		cout << "System age is unknown.\n";
		abort();
	}
}

string GHMD_Hadean(Object Target, Object Parent, float64 Age)
{
	ostringstream os;
	os << "## Hadean Eon ";
	os << vformat("({} - {} Yrs)\n", make_format_args(0, 540E6));

	for (size_t i = 0; i < HadeanPhases; i++)
	{
		if (Age - __Hadean_Eon_Timeline[i] > 0)
		{
			if (i == 1 && !HasMoon) { continue; }
			string Event = vformat(__Hadean_Eon_Events[i], make_format_args(Target.Name[0], Parent.Name[0]));
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(__Hadean_Eon_Timeline[i], Event));
		}
	}
	return os.str();
}

string GHMD_Archean(float64 Age)
{
	ostringstream os;
	os << "## Archean Eon ";
	os << vformat("({} - {} Yrs)\n", make_format_args(540E6, 2040E6));

	for (size_t i = 0; i < ArcheanPhases; i++)
	{
		if (Age - __Archean_Eon_Timeline[i] > 0)
		{
			string Event = vformat(__Archean_Eon_Events[i], make_format_args());
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(__Archean_Eon_Timeline[i], Event));
		}
	}
	return os.str();
}

string GHMD_Proterozoic(float64 Age)
{
	ostringstream os;
	os << "## Proterozoic Eon ";
	os << vformat("({} - {} Yrs)\n", make_format_args(2040E6, 4001E6));

	for (size_t i = 0; i < ProterozoicPhases; i++)
	{
		if (Age - __Proterozoic_Eon_Timeline[i] > 0)
		{
			string Event = vformat(__Proterozoic_Eon_Events[i], make_format_args());
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(__Proterozoic_Eon_Timeline[i], Event));
		}
	}
	return os.str();
}

string GHMD_Paleozoic(Object Target, float64 Age)
{
	ostringstream os;
	os << "### Paleozoic Era ";
	os << vformat("({} - {} Yrs)\n", make_format_args(4001.2E6, 4288.1E6));

	for (size_t i = 0; i < PalaeozoicPhases; i++)
	{
		if (Age - __Palaeozoic_Era_Timeline[i] > 0)
		{
			string Event = vformat(__Palaeozoic_Era_Events[i], make_format_args(Target.Name[0]));
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(__Palaeozoic_Era_Timeline[i], Event));
		}
	}
	return os.str();
}

string GHMD_Mesozoic(float64 Age)
{
	ostringstream os;
	os << "### Mesozoic Era ";
	os << vformat("({} - {} Yrs)\n", make_format_args(4288.1E6, 4474E6));

	for (size_t i = 0; i < MesozoicPhases; i++)
	{
		if (Age - __Mesozoic_Era_Timeline[i] > 0)
		{
			string Event = vformat(__Mesozoic_Era_Events[i], make_format_args());
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(__Mesozoic_Era_Timeline[i], Event));
		}
	}
	return os.str();
}

string GHMD_Cenozoic(float64 Age)
{
	ostringstream os;
	os << "### Cenozoic Era ";
	os << vformat("({} Yrs - {})\n", make_format_args(4474E6, "present"));

	for (size_t i = 0; i < CenozoicPhases; i++)
	{
		if (Age - __Cenozoic_Era_Timeline[i] > 0)
		{
			string Event = vformat(__Cenozoic_Era_Events[i], make_format_args());
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(__Cenozoic_Era_Timeline[i], Event));
		}
	}
	return os.str();
}

string GHMarkDownProcess(Object Target, Object Parent)
{
	ostringstream os;

	os << "# Timeline of the evolutionary history of life on " << Target.Name[0] << '\n';
	os << vformat("Total age: {}\n", make_format_args(TotalAge));

	os << GHMD_Hadean(Target, Parent, TotalAge);
	if (TotalAge > __Archean_Eon_Timeline[0])
	{
		os << GHMD_Archean(TotalAge);
	}
	if (TotalAge > __Proterozoic_Eon_Timeline[0])
	{
		os << GHMD_Proterozoic(TotalAge);
	}
	if (TotalAge > __Proterozoic_Eon_Timeline[ProterozoicPhases - 1])
	{
		os << "## Phanerozoic Eon ";
		os << vformat("({} Yrs - {})\n", make_format_args(4001E6, "present"));
		if (TotalAge > __Palaeozoic_Era_Timeline[0])
		{
			os << GHMD_Paleozoic(Target, TotalAge);
		}
		if (TotalAge > __Mesozoic_Era_Timeline[0])
		{
			os << GHMD_Mesozoic(TotalAge);
		}
		if (TotalAge > __Cenozoic_Era_Timeline[0])
		{
			os << GHMD_Cenozoic(TotalAge);
		}
	}

	os << "## Reference\n";
	os << " * [Wikipedia: History of Earth](https://en.wikipedia.org/wiki/History_of_Earth)\n";
	os << " * [Wikipedia: Timeline of the evolutionary history of life](https://en.wikipedia.org/wiki/Timeline_of_the_evolutionary_history_of_life#Detailed_timeline)\n";
	os << " * [Wikipedia: History of technology](https://en.wikipedia.org/wiki/History_of_technology)\n";
	os << " * [Wikipedia: Timeline of historic inventions](https://en.wikipedia.org/wiki/Timeline_of_historic_inventions)\n";

	return os.str();
}

void composite0geo(Object Target, Object Parent)
{
	// Initialize
	float64 __H1 = random.uniform(260E6, 770E6);
	__Hadean_Eon_Timeline = { 0, 30E6, 136E6, __H1 };
	float64 __A1 = random.uniform(770E6, 1040E6);
	__Archean_Eon_Timeline = { __H1, __A1, 1540E6 };
	float64 __P1 = random.uniform(3960E6, 4001E6);
	__Proterozoic_Eon_Timeline = { 2040E6, 2690E6, 3540E6, 3790E6, __P1 };
	float64 __EP1 = random.uniform(4205E6, 4215E6);
	float64 __EP2 = random.uniform(4215E6, 4220E6);
	float64 __EP3 = random.uniform(4288.1E6, 4288.6E6);
	__Palaeozoic_Era_Timeline = { 4001.2E6, 4005E6, 4010E6, 4054.6E6, 4055E6, 4096.2E6, 4100E6, 4120E6, 4120.8E6, 4145E6, 4165E6, 4177E6, 4180E6, 4181.1E6, 4190E6, __EP1, __EP2, 4235E6, 4241.1E6, 4260E6, __EP3 };
	__Mesozoic_Era_Timeline = { 4288.098E6, 4290E6, 4295E6, 4315E6, 4320E6, 4335E6, 4338.6E6, 4345E6, 4370E6, 4385E6, 4395E6, 4405E6, 4450E6 };
	float64 __EC1 = random.uniform(4474E6, 4474.001E6);
	__Cenozoic_Era_Timeline = { 4474E6, 4474E6, __EC1, 4480E6, 4485E6, 4490E6, 4505E6, 4516.97E6, 4530E6, 4533.5E6, 4536E6, 4536.7E6 };

	TotalAge = FindAge(Target, Parent);

	// Find Moons
	for (size_t i = 0; i < Target.Name.size(); i++)
	{
		auto it = SysIn->begin();
		auto end = SysIn->end();
		while (it != end)
		{
			Object Obj = ObjectLoader(it);
			if (Obj.ParentBody == Target.Name[i])
			{
				HasMoon = true;
				break;
			}
			++it;
		}
	}

	switch (OFormat)
	{
	case HTML:
		break;
	case MD:
	default:
		Final = GHMarkDownProcess(Target, Parent);
		break;
	}
}