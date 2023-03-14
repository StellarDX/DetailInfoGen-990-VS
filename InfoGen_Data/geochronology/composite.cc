#include "composite.h"
#include "final.h"
#include "../gbuffer_html.h"
#include "../final.h"

#include <map>

using namespace std;
using namespace cse;

vec2 __Hadean_Eon_Range(wrtval(POS_INF_DOUBLE));
vec2 __Archean_Eon_Range(wrtval(POS_INF_DOUBLE));
vec2 __Proterozoic_Eon_Range(wrtval(POS_INF_DOUBLE));
vec2 __Phanerozoic_Eon_Start(wrtval(POS_INF_DOUBLE));
vec2 __Palaeozoic_Era_Range(wrtval(POS_INF_DOUBLE));
vec2 __Mesozoic_Era_Range(wrtval(POS_INF_DOUBLE));
vec2 __Cenozoic_Era_Begin(wrtval(POS_INF_DOUBLE));

map<float64, string> __Hadean_Eon_Timeline;
map<float64, string> __Archean_Eon_Timeline;
map<float64, string> __Proterozoic_Eon_Timeline;
map<float64, string> __Palaeozoic_Era_Timeline;
map<float64, string> __Mesozoic_Era_Timeline;
map<float64, string> __Cenozoic_Era_Timeline;
map<float64, string> __Civilization_Timeline;

float64 EndYear = 0;
float64 EndCivil = 0;
string EndStr = "Civilization on this planet maybe more-developed than Earth. But this generation can only reach this point.";

float64 TotalAge;
bool HasMoon = false;

namespace Localization
{
	string Geo_Title       = "Timeline of the evolutionary history of life on {}";
	string Geo_Age         = "Total age";
	string Geo_Present     = "present";
	string Geo_Hadean      = "Hadean Eon";
	string Geo_Archean     = "Archean Eon";
	string Geo_Proterozoic = "Proterozoic Eon";
	string Geo_Phanerozoic = "Phanerozoic Eon";
	string Geo_Paleozoic   = "Paleozoic Era";
	string Geo_Mesozoic    = "Mesozoic Era";
	string Geo_Cenozoic    = "Cenozoic Era";
	string Geo_CvTitle     = "History of technology on Planet {} (Years since Paleolithic)";
}

using namespace Localization;

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

void HTMLPushMenuGeo(string Name, string ClassName, string YrRange, size_t TitleLvl)
{
	ostringstream os;
	string NSpace;
	for (size_t i = 0; i < TitleLvl; i++)
	{
		NSpace += "&nbsp&nbsp";
	}
	if (YrRange.empty()) { os << vformat("\t\t\t{1}<a href = \"#{0}\">{2}</a>", make_format_args(ClassName, NSpace, Name)); }
	else { os << vformat("\t\t\t{2}<a href = \"#{0}\">{3}</a> - {1}", make_format_args(ClassName, YrRange, NSpace, Name)); }
	os << _Html_Tags::_html_endl << '\n';
	HTMLMenu += os.str();
}

string HTML_Eons(Object Target, Object Parent, float64 Age, string Title, string ClassName, vec2 Range, map<float64, string> Timeline, function<bool(size_t)> Func = nullptr)
{
	ostringstream os;
	string LvStr = "\t\t\t";

	os << LvStr << _Html_Tags::_table_begin << '\n';
	os << vformat(LvStr + "\t<tr class = \"{0}Table\"><td colspan = 4 class = \"{0}TableTop\"><a name = \"{0}\">{1}</a></td>", make_format_args(ClassName, Title));
	string YRange;
	if (isinf(Range.y))
	{
		os << vformat("<td colspan = 2 class = \"{}TableTopEnd\" align=\"right\">{} Yrs - {}</td></tr>\n", make_format_args(ClassName, Range.x, Geo_Present));
		YRange = vformat("{} Yrs - {}", make_format_args(Range.x, Geo_Present));
	}
	else
	{
		os << vformat("<td colspan = 2 class = \"{}TableTopEnd\" align=\"right\">{} - {} Yrs</td></tr>\n", make_format_args(ClassName, Range.x, Range.y));
		YRange = vformat("{} Yrs - {}", make_format_args(Range.x, Range.y));
	}
	HTMLPushMenuGeo(Title, ClassName, YRange, 0);

	auto it = Timeline.begin();
	auto end = Timeline.end();
	for (size_t i = 0; it != end; ++it, ++i)
	{
		if (Age - it->first >= 0)
		{
			if (Func && Func(i)) { continue; }
			string Event = vformat(it->second, make_format_args(Target.Name[0], Parent.Name[0]));
			os << vformat(LvStr + "\t<tr class = \"{0}Table\"><td class = \"{0}TableHead\" colspan = 1 >{1}</td><td class = \"{0}TableData\" colspan = 5>{2}</td></tr>\n", make_format_args(ClassName, it->first, Event));
		}
	}

	os << LvStr << _Html_Tags::_table_end << '\n';
	return os.str();
}

string HTML_Eras(Object Target, Object Parent, float64 Age, string Title, string ClassName, vec2 Range, map<float64, string> Timeline, function<bool(size_t)> Func = nullptr)
{
	ostringstream os;
	string LvStr = "\t\t\t";

	string YRange;
	if (isinf(Range.y)){YRange = vformat("{} Yrs - {}", make_format_args(Range.x, Geo_Present));}
	else{YRange = vformat("{} - {} Yrs", make_format_args(Range.x, Range.y));}
	os << vformat(LvStr + "\t<tr class = \"{0}Table\"><td colspan = 6 class = \"{0}TableSectionHeader\" align=\"center\"><a name = \"{0}\">{1} ({2})</a></td></tr>\n", make_format_args(ClassName, Title, YRange));
	HTMLPushMenuGeo(Title, ClassName, YRange, 1);

	auto it = Timeline.begin();
	auto end = Timeline.end();
	for (size_t i = 0; it != end; ++it, ++i)
	{
		if (Age - it->first >= 0)
		{
			if (Func && Func(i)) { continue; }
			string Event = vformat(it->second, make_format_args(Target.Name[0], Parent.Name[0]));
			os << vformat(LvStr + "\t<tr class = \"{0}Table\"><td class = \"{0}TableHead\" colspan = 1 >{1}</td><td class = \"{0}TableData\" colspan = 5>{2}</td></tr>\n", make_format_args(ClassName, it->first, Event));
		}
	}

	return os.str();
}

string HTML_Civil(float64 Age, string PlanetName)
{
	ostringstream os;
	os << "\t\t\t" << _Html_Tags::_table_begin << '\n';
	os << vformat("\t\t\t\t<tr class = \"CivilizationTable\"><td colspan = 6 class = \"CivilizationTableTop\"><a name = \"Civilization\">" + Geo_CvTitle + "</a></td></tr>\n", make_format_args(PlanetName));
	HTMLPushMenuGeo(vformat(Geo_CvTitle, make_format_args(PlanetName)), "Civilization", "", 0);
	auto it = __Civilization_Timeline.begin();
	auto end = __Civilization_Timeline.end();
	for (; it != end; ++it)
	{
		if (Age - it->first >= 0)
		{
			string Event = vformat(it->second, make_format_args(__Cenozoic_Era_Timeline.rbegin()->first + it->first));
			os << vformat("\t\t\t\t<tr class = \"{0}Table\"><td class = \"{0}TableHead\" colspan = 1 >{1}</td><td class = \"{0}TableData\" colspan = 5>{2}</td></tr>\n", make_format_args("Civilization", it->first, Event));
		}
	}
	os << "\t\t\t" << _Html_Tags::_table_end << '\n';
	return os.str();
}

string GHMD_General(Object Target, Object Parent, float64 Age, string Title, size_t TitleLvl, vec2 Range, map<float64, string> Timeline, function<bool(size_t)> Func = nullptr)
{
	ostringstream os;
	string LvStr;
	for (size_t i = 0; i < TitleLvl; i++) { LvStr += '#'; }
	LvStr += ' ';
	os << LvStr + Title + " ";
	if (isinf(Range.y)) { os << vformat("({} Yrs - {})\n", make_format_args(Range.x, Geo_Present)); }
	else { os << vformat("({} - {} Yrs)\n", make_format_args(Range.x, Range.y)); }

	auto it = Timeline.begin();
	auto end = Timeline.end();
	for (size_t i = 0; it != end; ++it, ++i)
	{
		if (Age - it->first >= 0)
		{
			if (Func && Func(i)) { continue; }
			string Event = vformat(it->second, make_format_args(Target.Name[0], Parent.Name[0]));
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(it->first, Event));
		}
	}
	return os.str();
}

string GHMD_Civil(float64 Age)
{
	ostringstream os;
	auto it = __Civilization_Timeline.begin();
	auto end = __Civilization_Timeline.end();
	for (; it != end; ++it)
	{
		if (Age - it->first >= 0)
		{
			string Event = vformat(it->second, make_format_args(__Cenozoic_Era_Timeline.rbegin()->first + it->first));
			os << vformat(" * **{:.0f} Yrs** : {}\n", make_format_args(it->first, Event));
		}
	}
	return os.str();
}

string HTMLProcessEarth(Object Target, Object Parent) // For earth model
{
	ostringstream os;

	os << vformat("\t\t\t" + string(_Html_Tags::_h1_begin) + Geo_Title + string(_Html_Tags::_h1_end) + "\n", make_format_args(Target.Name[0]));
	os << vformat("\t\t\t" + string(_Html_Tags::_para_begin) + Geo_Age + ": {}" + string(_Html_Tags::_para_end) + "\n", make_format_args(TotalAge));

	os << HTML_Eons(Target, Parent, TotalAge, Geo_Hadean, "HadeanEon", __Hadean_Eon_Range, __Hadean_Eon_Timeline,
		(function<bool(size_t)>)[=](size_t i)->bool {return i == 1 && !HasMoon;});
	os << "\t\t\t" << _Html_Tags::_html_endl << '\n';
	if (!__Archean_Eon_Timeline.empty() && TotalAge > __Archean_Eon_Timeline.begin()->first)
	{
		os << HTML_Eons(Target, Parent, TotalAge, Geo_Archean, "ArcheanEon", __Archean_Eon_Range, __Archean_Eon_Timeline);
		os << "\t\t\t" << _Html_Tags::_html_endl << '\n';
	}
	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.begin()->first)
	{
		os << HTML_Eons(Target, Parent, TotalAge, Geo_Proterozoic, "ProterozoicEon", __Proterozoic_Eon_Range, __Proterozoic_Eon_Timeline);
		os << "\t\t\t" << _Html_Tags::_html_endl << '\n';
	}

	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.rbegin()->first)
	{
		os << "\t\t\t" << _Html_Tags::_table_begin << '\n';
		os << vformat("\t\t\t\t<tr class = \"{0}Table\"><td colspan = 4 class = \"{0}TableTop\"><a name = \"{0}\">{1}</a></td>", make_format_args("PhanerozoicEon", Geo_Phanerozoic));
		HTMLPushMenuGeo(Geo_Phanerozoic, "PhanerozoicEon", vformat("{} Yrs - {}", make_format_args(__Phanerozoic_Eon_Start.x, Geo_Present)), 0);
		os << vformat("<td colspan = 2 class = \"{}TableTopEnd\" align=\"right\">{} Yrs - {}</td></tr>\n", make_format_args("PhanerozoicEon", __Phanerozoic_Eon_Start.x, Geo_Present));
		if (!__Palaeozoic_Era_Timeline.empty() && TotalAge > __Palaeozoic_Era_Timeline.begin()->first)
		{
			os << HTML_Eras(Target, Parent, TotalAge, Geo_Paleozoic, "PalaeozoicEra", __Palaeozoic_Era_Range, __Palaeozoic_Era_Timeline);
		}
		if (!__Mesozoic_Era_Timeline.empty() && TotalAge > __Mesozoic_Era_Timeline.begin()->first)
		{
			os << HTML_Eras(Target, Parent, TotalAge, Geo_Mesozoic, "MesozoicEra", __Mesozoic_Era_Range, __Mesozoic_Era_Timeline);
		}
		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.begin()->first)
		{
			os << HTML_Eras(Target, Parent, TotalAge, Geo_Cenozoic, "CenozoicEra", __Cenozoic_Era_Begin, __Cenozoic_Era_Timeline);
		}
		os << "\t\t\t" << _Html_Tags::_table_end << '\n';
		os << "\t\t\t" << _Html_Tags::_html_endl << '\n';

		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.rbegin()->first)
		{
			//os << vformat("\t\t\t" + string(_Html_Tags::_h2_begin) + Geo_CvTitle + string(_Html_Tags::_h2_end) + "\n", make_format_args(Target.Name[0]));
			os << HTML_Civil(TotalAge - __Cenozoic_Era_Timeline.rbegin()->first, Target.Name[0]);

			if (TotalAge - __Cenozoic_Era_Timeline.rbegin()->first > __Civilization_Timeline.rbegin()->first)
			{
				os << "\t\t\t" << _Html_Tags::_para_begin + EndStr + _Html_Tags::_para_end + "\n";
			}
		}
	}

	os << "\t\t\t" << _Html_Tags::_h2_begin << "Reference" << _Html_Tags::_h2_end << "\n";
	os << "\t\t\t<p><a href = \"https://en.wikipedia.org/wiki/History_of_Earth\">Wikipedia: History of Earth</a></p>\n";
	os << "\t\t\t<p><a href = \"https://en.wikipedia.org/wiki/Timeline_of_the_evolutionary_history_of_life#Detailed_timeline\">Wikipedia: Timeline of the evolutionary history of life</a></p>\n";
	os << "\t\t\t<p><a href = \"https://en.wikipedia.org/wiki/History_of_technology\">Wikipedia: History of technology</a></p>\n";
	os << "\t\t\t<p><a href = \"https://en.wikipedia.org/wiki/Timeline_of_historic_inventions\">Wikipedia: Timeline of historic inventions</a></p>\n";

	return os.str();
}

string GHMarkDownProcessEarth(Object Target, Object Parent) // For earth model
{
	ostringstream os;

	os << vformat("# " + Geo_Title + "\n", make_format_args(Target.Name[0]));
	os << vformat(Geo_Age + ": {}\n", make_format_args(TotalAge));

	os << GHMD_General(Target, Parent, TotalAge, Geo_Hadean, 2, __Hadean_Eon_Range, __Hadean_Eon_Timeline,
		(function<bool(size_t)>)[=](size_t i)->bool {return i == 1 && !HasMoon;});
	if (!__Archean_Eon_Timeline.empty() && TotalAge > __Archean_Eon_Timeline.begin()->first)
	{
		os << GHMD_General(Target, Parent, TotalAge, Geo_Archean, 2, __Archean_Eon_Range, __Archean_Eon_Timeline);
	}
	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.begin()->first)
	{
		os << GHMD_General(Target, Parent, TotalAge, Geo_Proterozoic, 2, __Proterozoic_Eon_Range, __Proterozoic_Eon_Timeline);
	}
	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.rbegin()->first)
	{
		os << "## " + Geo_Phanerozoic + " ";
		os << vformat("({} Yrs - {})\n", make_format_args(__Phanerozoic_Eon_Start.x, Geo_Present));
		if (!__Palaeozoic_Era_Timeline.empty() && TotalAge > __Palaeozoic_Era_Timeline.begin()->first)
		{
			os << GHMD_General(Target, Parent, TotalAge, Geo_Paleozoic, 3, __Palaeozoic_Era_Range, __Palaeozoic_Era_Timeline);
		}
		if (!__Mesozoic_Era_Timeline.empty() && TotalAge > __Mesozoic_Era_Timeline.begin()->first)
		{
			os << GHMD_General(Target, Parent, TotalAge, Geo_Mesozoic, 3, __Mesozoic_Era_Range, __Mesozoic_Era_Timeline);
		}
		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.begin()->first)
		{
			os << GHMD_General(Target, Parent, TotalAge, Geo_Cenozoic, 3, __Cenozoic_Era_Begin, __Cenozoic_Era_Timeline);
		}
		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.rbegin()->first)
		{
			os << vformat("## " + Geo_CvTitle + "\n", make_format_args(Target.Name[0]));
			os << GHMD_Civil(TotalAge - __Cenozoic_Era_Timeline.rbegin()->first);
			if (TotalAge - __Cenozoic_Era_Timeline.rbegin()->first > __Civilization_Timeline.rbegin()->first)
			{
				os << "\n" + EndStr + "\n";
			}
		}
	}

	os << "## Reference\n";
	os << " * [Wikipedia: History of Earth](https://en.wikipedia.org/wiki/History_of_Earth)\n";
	os << " * [Wikipedia: Timeline of the evolutionary history of life](https://en.wikipedia.org/wiki/Timeline_of_the_evolutionary_history_of_life#Detailed_timeline)\n";
	os << " * [Wikipedia: History of technology](https://en.wikipedia.org/wiki/History_of_technology)\n";
	os << " * [Wikipedia: Timeline of historic inventions](https://en.wikipedia.org/wiki/Timeline_of_historic_inventions)\n";

	return os.str();
}

string HTMLProcess(Object Target, Object Parent) // For earth model
{
	ostringstream os;

	os << vformat("\t\t\t" + string(_Html_Tags::_h1_begin) + Geo_Title + string(_Html_Tags::_h1_end) + "\n", make_format_args(Target.Name[0]));
	os << vformat("\t\t\t" + string(_Html_Tags::_para_begin) + Geo_Age + ": {}" + string(_Html_Tags::_para_end) + "\n", make_format_args(TotalAge));

	os << HTML_Eons(Target, Parent, TotalAge, Geo_Hadean, "HadeanEon", __Hadean_Eon_Range, __Hadean_Eon_Timeline);
	os << "\t\t\t" << _Html_Tags::_html_endl << '\n';
	if (!__Archean_Eon_Timeline.empty() && TotalAge > __Archean_Eon_Timeline.begin()->first)
	{
		os << HTML_Eons(Target, Parent, TotalAge, Geo_Archean, "ArcheanEon", __Archean_Eon_Range, __Archean_Eon_Timeline);
		os << "\t\t\t" << _Html_Tags::_html_endl << '\n';
	}
	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.begin()->first)
	{
		os << HTML_Eons(Target, Parent, TotalAge, Geo_Proterozoic, "ProterozoicEon", __Proterozoic_Eon_Range, __Proterozoic_Eon_Timeline);
		os << "\t\t\t" << _Html_Tags::_html_endl << '\n';
	}

	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.rbegin()->first)
	{
		os << "\t\t\t" << _Html_Tags::_table_begin << '\n';
		os << vformat("\t\t\t\t<tr class = \"{0}Table\"><td colspan = 4 class = \"{0}TableTop\"><a name = \"{0}\">{1}</a></td>", make_format_args("PhanerozoicEon", Geo_Phanerozoic));
		if (isinf(__Phanerozoic_Eon_Start.y)) { os << vformat("<td colspan = 2 class = \"{}TableTopEnd\" align=\"right\">{} Yrs - {}</td></tr>\n", make_format_args("PhanerozoicEon", __Phanerozoic_Eon_Start.x, Geo_Present)); }
		else { os << vformat("<td colspan = 2 class = \"{}TableTopEnd\" align=\"right\">{} - {} Yrs</td></tr>\n", make_format_args("PhanerozoicEon", __Phanerozoic_Eon_Start.x, __Phanerozoic_Eon_Start.y)); }
		HTMLPushMenuGeo(Geo_Phanerozoic, "PhanerozoicEon", vformat("{} Yrs - {}", make_format_args(__Phanerozoic_Eon_Start.x, Geo_Present)), 0);
		
		if (!__Palaeozoic_Era_Timeline.empty() && TotalAge > __Palaeozoic_Era_Timeline.begin()->first)
		{
			os << HTML_Eras(Target, Parent, TotalAge, Geo_Paleozoic, "PalaeozoicEra", __Palaeozoic_Era_Range, __Palaeozoic_Era_Timeline);
		}
		if (!__Mesozoic_Era_Timeline.empty() && TotalAge > __Mesozoic_Era_Timeline.begin()->first)
		{
			os << HTML_Eras(Target, Parent, TotalAge, Geo_Mesozoic, "MesozoicEra", __Mesozoic_Era_Range, __Mesozoic_Era_Timeline);
		}
		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.begin()->first)
		{
			os << HTML_Eras(Target, Parent, TotalAge, Geo_Cenozoic, "CenozoicEra", __Cenozoic_Era_Begin, __Cenozoic_Era_Timeline);
		}
		os << "\t\t\t" << _Html_Tags::_table_end << '\n';
		os << "\t\t\t" << _Html_Tags::_html_endl << '\n';

		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.rbegin()->first)
		{
			//os << vformat("\t\t\t" + string(_Html_Tags::_h2_begin) + Geo_CvTitle + string(_Html_Tags::_h2_end) + "\n", make_format_args(Target.Name[0]));
			os << HTML_Civil(TotalAge - __Cenozoic_Era_Timeline.rbegin()->first, Target.Name[0]);
		}
	}

	if (TotalAge > EndYear)
	{
		os << "\t\t\t" << _Html_Tags::_para_begin + EndStr + _Html_Tags::_para_end + "\n";
	}

	return os.str();
}

string GHMarkDownProcess(Object Target, Object Parent)
{
	ostringstream os;

	os << vformat("# " + Geo_Title + "\n", make_format_args(Target.Name[0]));
	os << vformat(Geo_Age + ": {}\n", make_format_args(TotalAge));

	os << GHMD_General(Target, Parent, TotalAge, Geo_Hadean, 2, __Hadean_Eon_Range, __Hadean_Eon_Timeline);
	if (!__Archean_Eon_Timeline.empty() && TotalAge > __Archean_Eon_Timeline.begin()->first)
	{
		os << GHMD_General(Target, Parent, TotalAge, Geo_Archean, 2, __Archean_Eon_Range, __Archean_Eon_Timeline);
	}
	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.begin()->first)
	{
		os << GHMD_General(Target, Parent, TotalAge, Geo_Proterozoic, 2, __Proterozoic_Eon_Range, __Proterozoic_Eon_Timeline);
	}
	if (!__Proterozoic_Eon_Timeline.empty() && TotalAge > __Proterozoic_Eon_Timeline.rbegin()->first)
	{
		os << "## " + Geo_Phanerozoic + " ";
		if (isinf(__Phanerozoic_Eon_Start.y)) { os << vformat("({} Yrs - {})\n", make_format_args(__Phanerozoic_Eon_Start.x, Geo_Present)); }
		else { os << vformat("({} Yrs - {})\n", make_format_args(__Phanerozoic_Eon_Start.x, __Phanerozoic_Eon_Start.y)); }
		if (!__Palaeozoic_Era_Timeline.empty() && TotalAge > __Palaeozoic_Era_Timeline.begin()->first)
		{
			os << GHMD_General(Target, Parent, TotalAge, Geo_Paleozoic, 3, __Palaeozoic_Era_Range, __Palaeozoic_Era_Timeline);
		}
		if (!__Mesozoic_Era_Timeline.empty() && TotalAge > __Mesozoic_Era_Timeline.begin()->first)
		{
			os << GHMD_General(Target, Parent, TotalAge, Geo_Mesozoic, 3, __Mesozoic_Era_Range, __Mesozoic_Era_Timeline);
		}
		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.begin()->first)
		{
			os << GHMD_General(Target, Parent, TotalAge, Geo_Cenozoic, 3, __Cenozoic_Era_Begin, __Cenozoic_Era_Timeline);
		}
		if (!__Cenozoic_Era_Timeline.empty() && TotalAge > __Cenozoic_Era_Timeline.rbegin()->first)
		{
			os << vformat("## " + Geo_CvTitle + "\n", make_format_args(Target.Name[0]));
			os << GHMD_Civil(TotalAge - __Cenozoic_Era_Timeline.rbegin()->first);
		}
	}

	if (TotalAge > EndYear)
	{
		os << "\n" + EndStr + "\n";
	}

	return os.str();
}

void _DefInit()
{
	__Hadean_Eon_Range = vec2(0, 540E6);
	__Archean_Eon_Range = vec2(540E6, 2040E6);
	__Proterozoic_Eon_Range = vec2(2040E6, 4001E6);
	__Phanerozoic_Eon_Start = vec2(4001E6, wrtval(POS_INF_DOUBLE));
	__Palaeozoic_Era_Range = vec2(4001.2E6, 4288.1E6);
	__Mesozoic_Era_Range = vec2(4288.1E6, 4474E6);
	__Cenozoic_Era_Begin = vec2(4474E6, wrtval(POS_INF_DOUBLE));

	float64 __H1 = random.uniform(260E6, 770E6);
	__Hadean_Eon_Timeline = 
	{
		{0,     __Hadean_Eon_Events[0]},
		{30E6,  __Hadean_Eon_Events[1]},
		{136E6, __Hadean_Eon_Events[2]},
		{__H1,  __Hadean_Eon_Events[3]}
	};
	float64 __A1 = random.uniform(770E6, 1040E6);
	__Archean_Eon_Timeline = 
	{
		{__H1,   __Archean_Eon_Events[0]},
		{__A1,   __Archean_Eon_Events[1]},
		{1540E6, __Archean_Eon_Events[2]}
	};
	float64 __P1 = random.uniform(3960E6, 4001E6);
	__Proterozoic_Eon_Timeline = 
	{
		{2040E6, __Proterozoic_Eon_Events[0]},
		{2690E6, __Proterozoic_Eon_Events[1]},
		{3540E6, __Proterozoic_Eon_Events[2]},
		{3790E6, __Proterozoic_Eon_Events[3]},
		{__P1,   __Proterozoic_Eon_Events[4]}
	};
	float64 __EP1 = random.uniform(4205E6, 4215E6);
	float64 __EP2 = random.uniform(4215E6, 4220E6);
	float64 __EP3 = random.uniform(4288.1E6, 4288.6E6);
	__Palaeozoic_Era_Timeline = 
	{
		{4001.2E6, __Palaeozoic_Era_Events[0]},
		{4005E6,   __Palaeozoic_Era_Events[1]},
		{4010E6,   __Palaeozoic_Era_Events[2]},
		{4054.6E6, __Palaeozoic_Era_Events[3]},
		{4055E6,   __Palaeozoic_Era_Events[4]},
		{4096.2E6, __Palaeozoic_Era_Events[5]},
		{4100E6,   __Palaeozoic_Era_Events[6]},
		{4120E6,   __Palaeozoic_Era_Events[7]},
		{4120.8E6, __Palaeozoic_Era_Events[8]},
		{4145E6,   __Palaeozoic_Era_Events[9]},
		{4165E6,   __Palaeozoic_Era_Events[10]},
		{4177E6,   __Palaeozoic_Era_Events[11]},
		{4180E6,   __Palaeozoic_Era_Events[12]},
		{4181.1E6, __Palaeozoic_Era_Events[13]},
		{4190E6,   __Palaeozoic_Era_Events[14]},
		{__EP1,    __Palaeozoic_Era_Events[15]},
		{__EP2,    __Palaeozoic_Era_Events[16]},
		{4235E6,   __Palaeozoic_Era_Events[17]},
		{4241.1E6, __Palaeozoic_Era_Events[18]},
		{4260E6,   __Palaeozoic_Era_Events[19]},
		{__EP3,    __Palaeozoic_Era_Events[20]}
	};
	__Mesozoic_Era_Timeline = 
	{
		{4288.098E6, __Mesozoic_Era_Events[0]},
		{4290E6,     __Mesozoic_Era_Events[1]},
		{4295E6,     __Mesozoic_Era_Events[2]},
		{4315E6,     __Mesozoic_Era_Events[3]},
		{4320E6,     __Mesozoic_Era_Events[4]},
		{4335E6,     __Mesozoic_Era_Events[5]},
		{4338.6E6,   __Mesozoic_Era_Events[6]},
		{4345E6,     __Mesozoic_Era_Events[7]},
		{4370E6,     __Mesozoic_Era_Events[8]},
		{4385E6,     __Mesozoic_Era_Events[9]},
		{4395E6,     __Mesozoic_Era_Events[10]},
		{4405E6,     __Mesozoic_Era_Events[11]},
		{4450E6,     __Mesozoic_Era_Events[12]}
	};
	float64 __EC1 = random.uniform(4474E6, 4474.001E6);
	__Cenozoic_Era_Timeline = 
	{
		{4474E6,    __Cenozoic_Era_Events[0]},
		{4474E6 + 1, __Cenozoic_Era_Events[1]}, // avoid comflict
		{__EC1,     __Cenozoic_Era_Events[2]},
		{4480E6,    __Cenozoic_Era_Events[3]},
		{4485E6,    __Cenozoic_Era_Events[4]},
		{4490E6,    __Cenozoic_Era_Events[5]},
		{4505E6,    __Cenozoic_Era_Events[6]},
		{4516.97E6, __Cenozoic_Era_Events[7]},
		{4530E6,    __Cenozoic_Era_Events[8]},
		{4533.5E6,  __Cenozoic_Era_Events[9]},
		{4536E6,    __Cenozoic_Era_Events[10]},
		{4536.7E6,  __Cenozoic_Era_Events[11]}
	};
	float64 __C1 = random.uniform(3280000, 3285000);
	float64 __C2 = random.uniform(3294700, 3295300);
	float64 __C3 = random.uniform(3296700, 3296825);
	__Civilization_Timeline = 
	{
		{0,       __Civilization_Events[0]},
		{720000,  __Civilization_Events[1]},
		{__C1,    __Civilization_Events[2]},
		{3288000, __Civilization_Events[3]},
		{__C2,    __Civilization_Events[4]},
		{__C3,    __Civilization_Events[5]},
		{3299760, __Civilization_Events[6]},
		{3299870, __Civilization_Events[7]},
		{3299947, __Civilization_Events[8]},
		{3300016, __Civilization_Events[9]}
	};
}

void GetLcString(string Key, string* Val);
void GeoLoadLocStr()
{
	GetLcString("Geo_Title",       &Geo_Title);
	GetLcString("Geo_Age",         &Geo_Age);
	GetLcString("Geo_Present",     &Geo_Present);
	GetLcString("Geo_Hadean",      &Geo_Hadean);
	GetLcString("Geo_Archean",     &Geo_Archean);
	GetLcString("Geo_Proterozoic", &Geo_Proterozoic);
	GetLcString("Geo_Phanerozoic", &Geo_Phanerozoic);
	GetLcString("Geo_Paleozoic",   &Geo_Paleozoic);
	GetLcString("Geo_Mesozoic",    &Geo_Mesozoic);
	GetLcString("Geo_Cenozoic",    &Geo_Cenozoic);
	GetLcString("Geo_CvTitle",     &Geo_CvTitle);
}

void composite0geo(Object Target, Object Parent)
{
	// Initialize
	if (!CustomModel) { _DefInit(); }
	GeoLoadLocStr();

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
		if (CSSPath.empty()) { CSSPath = "./InfoGen_Data/html_themes/Geochronology.css"; }
		Final += MakeHTMLHead(vformat(Geo_Title, make_format_args(Target.Name[0])), CSSPath, CopyCSS);
		if (CustomModel) { HTMLcontent = HTMLProcess(Target, Parent); }
		else { HTMLcontent += HTMLProcessEarth(Target, Parent); }
		break;
	case MD:
	default:
		if (CustomModel) { Final = GHMarkDownProcess(Target, Parent); }
		else { Final = GHMarkDownProcessEarth(Target, Parent); }
		break;
	}
}