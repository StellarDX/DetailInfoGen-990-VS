#include "gbuffer_html.h"

#include <sstream>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <windows.h>

using namespace std;
using namespace std::filesystem;

string DefaultCSSPath = "./InfoGen_Data/html_themes/Default.css";
int CSSEncod = 65001;
extern string OutputFileName;

enum CopyDefaultMethod DefaultCCpyMethod = Asking;

map<int, string> HTMLCharsetsPerm
{
	// Reference: https://html.spec.whatwg.org/multipage/parsing.html#character-encodings
	{65001, "UTF-8"       }, {28592, "ISO-8859-2"  }, {28597, "ISO-8859-7"  }, {28598, "ISO-8859-8"  }, {38598, "ISO-8859-8"  },
	{  874, "windows-874" }, { 1250, "windows-1250"}, { 1251, "windows-1251"}, { 1252, "windows-1252"},
	{ 1254, "windows-1254"}, { 1255, "windows-1255"}, { 1256, "windows-1256"}, { 1257, "windows-1257"},
	{ 1258, "windows-1258"}, {  936, "GBK"         }, {  950, "Big5"        }, {50220, "ISO-2022-JP" },
	{  932, "Shift_JIS"   }, {51949, "EUC-KR"      }, { 1201, "UTF-16BE"    }, { 1200, "UTF-16LE"    }
};

map<int, string> HTMLCharsets4b1d
{
	/*{     , "CESU-8"      },*/ {65000, "UTF-7"       }, /*{     , "BOCU-1"      },*/
	/*{     , "SCSU"        },*/ {12000, "UTF-32LE"    }, {12001, "UTF-32BE"    }
};

int IsEBCDIC(int Cp) // EBCDIC encodings are forbidden in HTML
{
	static const int _EBCDICEncodings[224]
	(
		1, // USA WP, Original
		2, // USA
		3, // USA Accounting, Version A
		4, // USA
		5, // USA
		6, // Latin America
		7, // Germany F.R. / Austria
		8, // Germany F.R.
		9, // France, Belgium
		10, // Canada(English)
		11, // Canada(French)
		12, // Italy
		13, // Netherlands
		14,
		15, // Switzerland(French)
		16, // Switzerland(French / German)
		17, // Switzerland(German)
		18, // Sweden / Finland
		19, // Sweden / Finland WP, version 2
		20, // Denmark / Norway
		21, // Brazil
		22, // Portugal
		23, // United Kingdom
		24, // United Kingdom
		25, // Japan(Latin)
		26, // Japan(Latin)
		27, // Greece(Latin)
		28,
		29, // Iceland
		30, // Turkey
		31, // South Africa
		32, // Czechoslovakia(Czech / Slovak)
		33, // Czechoslovakia
		34, // Czechoslovakia
		35, // Romania
		36, // Romania
		37, // USA / Canada - CECP(same with euro : 1140)
		//37-2, // The real 3279 APL codepage, as used by C / 370. This is very close to 1047, except for caret and not- sign inverted.It is not officially recognized by IBM, even though SHARE has pointed out its existence.[14]
		38, // USA ASCII
		39, // United Kingdom / Israel
		40, // United Kingdom
		251, // China
		252, // Poland
		254, // Hungary
		256, // International #1 (superseded by 500)
		257, // International #2
		258, // International #3
		259, // Symbols, Set 7
		260, // Canadian French - 116
		264, // Print Train & Text processing extended
		273, // Germany F.R. / Austria - CECP(same with euro : 1141)
		274, // Old Belgium Code Page
		275, // Brazil - CECP
		276, // Canada(French) - 94
		277, // Denmark, Norway - CECP(same with euro : 1142)
		278, // Finland, Sweden - CECP(same with euro : 1143)
		279, // French - 94[14]
		280, // Italy - CECP(same with euro : 1144)
		281, // Japan(Latin) - CECP
		282, // Portugal - CECP
		283, // Spain - 190[14]
		284, // Spain / Latin America - CECP(same with euro : 1145)
		285, // United Kingdom - CECP(same with euro : 1146)
		286, // Austria / Germany F.R.Alternate
		287, // Denmark / Norway Alternate
		288, // Finland / Sweden Alternate
		289, // Spain Alternate
		290, // Japanese(Katakana) Extended
		293, // APL
		297, // France(same with euro : 1147)[14]
		298, // Japan(Katakana)
		300, // Japan(Kanji) DBCS(For JIS X 0213)
		310, // Graphic Escape APL / TN
		320, // Hungary
		321, // Yugoslavia
		322, // Turkey
		330, // International #4
		351, // GDDM default
		352, // Printing and publishing option
		353, // BCDIC - A
		355, // PTTC / BCD standard option
		357, // PTTC / BCD H option
		358, // PTTC / BCD Correspondence option
		359, // PTTC / BCD Monocase option
		360, // PTTC / BCD Duocase option
		361, // EBCDIC Publishing International
		363, // Symbols, set 8
		382, // EBCDIC Publishing Austria, Germany F.R.Alternate
		383, // EBCDIC Publishing Belgium
		384, // EBCDIC Publishing Brazil
		385, // EBCDIC Publishing Canada(French)
		386, // EBCDIC Publishing Denmark, Norway
		387, // EBCDIC Publishing Finland, Sweden
		388, // EBCDIC Publishing France
		389, // EBCDIC Publishing Italy
		390, // EBCDIC Publishing Japan(Latin)
		391, // EBCDIC Publishing Portugal
		392, // EBCDIC Publishing Spain, Philippines
		393, // EBCDIC Publishing Latin America(Spanish Speaking)
		394, // EBCDIC Publishing China(Hong Kong), UK, Ireland
		395, // EBCDIC Publishing Australia, New Zealand, USA, Canada(English)
		410, // Cyrillic(revisions: 880, 1025, 1154)
		420, // Arabic
		421, // Maghreb / French
		423, // Greek(superseded by 875)
		424, // Hebrew(Bulletin Code)
		425, // Arabic / Latin for OS / 390 Open Edition
		435, // Teletext Isomorphic
		500, // International #5 (ECECP; supersedes 256) (same with euro : 1148)
		803, // Hebrew Character Set A(Old Code)
		829, // Host Math Symbols - Publishing
		833, // Korean Extended(SBCS)
		834, // Korean Hangul(KSC5601; DBCS with UDCs)
		835, // Traditional Chinese DBCS
		836, // Simplified Chinese Extended
		837, // Simplified Chinese DBCS
		838, // Thai with Low Marks & Accented Characters(same with euro : 1160)
		839, // Thai DBCS
		870, // Latin 2 (same with euro : 1153) (revision: 1110)
		871, // Iceland(same with euro : 1149)[14]
		875, // Greek(supersedes 423)
		880, // Cyrillic(revision of 410) (revisions: 1025, 1154)
		881, // United States - 5080 Graphics System
		882, // United Kingdom - 5080 Graphics System
		883, // Sweden - 5080 Graphics System
		884, // Germany - 5080 Graphics System
		885, // France - 5080 Graphics System
		886, // Italy - 5080 Graphics System
		887, // Japan - 5080 Graphics System
		888, // France AZERTY - 5080 Graphics System
		889, // Thailand
		890, // Yugoslavia
		892, // EBCDIC, OCR A
		893, // EBCDIC, OCR B
		905, // Latin 3
		918, // Urdu Bilingual
		924, // Latin 9
		930, // Japan MIX(290 + 300) (same with euro : 1390)
		931, // Japan MIX(37 + 300)
		933, // Korea MIX(833 + 834) (same with euro : 1364)
		935, // Simplified Chinese MIX(836 + 837) (same with euro : 1388)
		937, // Traditional Chinese MIX(37 + 835) (same with euro : 1371)
		939, // Japan MIX(1027 + 300) (same with euro : 1399)
		1001, // MICR
		1002, // EBCDIC DCF Release 2 Compatibility
		1003, // EBCDIC DCF, US Text subset
		1005, // EBCDIC Isomorphic Text Communication
		1007, // EBCDIC Arabic(XCOM2)
		1024, // EBCDIC T.61
		1025, // Cyrillic, Multilingual(same with euro : 1154) (Revision of 880)
		1026, // EBCDIC Turkey(Latin 5) (same with euro : 1155) (supersedes 905 in that country)
		1027, // Japanese(Latin) Extended(JIS X 0201 Extended)
		1028, // EBCDIC Publishing Hebrew
		1030, // Japanese(Katakana) Extended
		1031, // Japanese(Latin) Extended
		1032, // MICR, E13 - B Combined
		1033, // MICR, CMC - 7 Combined
		1037, // Korea - 5080 / 6090 Graphics System
		1039, // GML Compatibility
		1047, // Latin 1 / Open Systems[14]
		1068, // DCF Compatibility
		1069, // Latin 4
		1070, // USA / Canada Version 0 (Code page 37 Version 0)
		1071, // Germany F.R. / Austria(Code page 273 Version 0)
		1072, // Belgium(Code page 274 Version 0)
		1073, // Brazil(Code page 275 Version 0)
		1074, // Denmark, Norway(Code page 277 Version 0)
		1075, // Finland, Sweden(Code page 278 Version 0)
		1076, // Italy(Code page 280 Version 0)
		1077, // Japan(Latin) (Code page 281 Version 0)
		1078, // Portugal(Code page 282 Version 0)
		1079, // Spain / Latin America Version 0 (Code page 284 Version 0)
		1080, // United Kingdom(Code page 285 Version 0)
		1081, // France Version 0 (Code page 297 Version 0)
		1082, // Israel(Hebrew)
		1083, // Israel(Hebrew)
		1084, // International#5 Version 0 (Code page 500 Version 0)
		1085, // Iceland(Code page 871 Version 0)
		1087, // Symbol Set
		1091, // Modified Symbols, Set 7
		1093, // IBM Logo[15]
		1097, // Farsi Bilingual
		1110, // Latin 2 (Revision of 870)
		1112, // Baltic Multilingual(same with euro : 1156)
		1113, // Latin 6
		1122, // Estonia(same with euro : 1157)
		1123, // Cyrillic, Ukraine(same with euro : 1158)
		1130, // Vietnamese(same with euro : 1164)
		1132, // Lao EBCDIC
		1136, // Hitachi Katakana
		1137, // Devanagari EBCDIC
		1140, // USA, Canada, etc.ECECP(same without euro : 37) (Traditional Chinese version : 1159)
		1141, // Austria, Germany ECECP(same without euro : 273)
		1142, // Denmark, Norway ECECP(same without euro : 277)
		1143, // Finland, Sweden ECECP(same without euro : 278)
		1144, // Italy ECECP(same without euro : 280)
		1145, // Spain, Latin America(Spanish) ECECP(same without euro : 284)
		1146, // UK ECECP(same without euro : 285)
		1147, // France ECECP with euro(same without euro : 297)
		1148, // International ECECP with euro(same without euro : 500)
		1149, // Icelandic ECECP with euro(same without euro : 871)
		1150, // Korean Extended with box characters
		1151, // Simplified Chinese Extended with box characters
		1152, // Traditional Chinese Extended with box characters
		1153, // Latin 2 Multilingual with euro(same without euro : 870)
		1154, // Cyrillic, Multilingual with euro(same without euro : 1025; an older version is * 1166)
		1155, // Turkey with euro(same without euro : 1026)
		1156, // Baltic Multi with euro(same without euro : 1112)
		1157, // Estonia with euro(same without euro : 1122)
		1158, // Cyrillic, Ukraine with euro(same without euro : 1123)
		1159, // T - Chinese EBCDIC(Traditional Chinese euro update of * 1140)
		1160, // Thai with Low Marks & Accented Characters with euro(same without euro : 838)
		1164, // Vietnamese with euro(same without euro : 1130)
		1165, // Latin 2 / Open Systems
		1166, // Cyrillic Kazakh
		1278, // EBCDIC Adobe(PostScript) Standard Encoding
		1279, // Hitachi Japanese Katakana Host[6]
		1303, // EBCDIC Bar Code
		1364, // Korea MIX(833 + 834 + euro) (same without euro : 933)
		1371, // Traditional Chinese MIX(1159 + 835) (same without euro : 937)
		1376, // Traditional Chinese DBCS Host extension for HKSCS
		1377, // Mixed Host HKSCS Growing(37 + 1376)
		1388, // Simplified Chinese MIX(same without euro : 935) (836 + 837 + euro)
		1390, // Simplified Chinese MIX Japan MIX(same without euro : 930) (290 + 300 + euro)
		1399  // Japan MIX(1027 + 300 + euro) (same without euro : 939)
	);

	if (!find(_EBCDICEncodings, _EBCDICEncodings + 224, Cp)) { return 0; }
	else { return Cp; }
}

string MoveCSS(string _Src, string _Dst)
{
	path FileName(_Dst);

	bool IsCopy = true;
	if (!exists(_Src))
	{
		cout << "Source css file is not exist.\n";
		abort();
	}
	if (exists(_Dst))
	{
		char YN;
		switch (DefaultCCpyMethod)
		{
		case Asking:
		default:
			cout << "Destination css file is already exist, cover this file? (Y/N)\n";
			cin >> YN;
			break;
		case Skip:
			YN = 'N';
			break;
		case Replace:
			YN = 'Y';
			break;
		}

		switch (YN)
		{
		case 'N':
		case 'n':
		default:
			IsCopy = false;
			break;
		case 'Y':
		case 'y':
			break;
		}
	}
	else if (!exists(FileName.parent_path()))
	{
		if (MakeDir)
		{
			cout << "Output directory is not exist, creating...\n";
			if (!create_directory(FileName.parent_path()))
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
	if (IsCopy)
	{
		copy(_Src, _Dst);
	}

	return FileName.stem().string() + FileName.extension().string();
}

string MakeHTMLHead(string Title, int Charset, string CSSPath, LinkCSS Copy)
{
	ostringstream os;
	os << "\t" << _Html_Tags::_head_begin << '\n';

	if (HTMLCharsets4b1d.find(Charset) != HTMLCharsets4b1d.end())
	{
		cout << "Code page \"" + HTMLCharsets4b1d.find(Charset)->second + "\" is forbidden in HTML.\n";
		abort();
	}
	if (IsEBCDIC(Charset))
	{
		cout << "EBCDIC encodings are forbidden in HTML\n";
		abort();
	}
	if (HTMLCharsetsPerm.find(Charset) == HTMLCharsetsPerm.end())
	{
		cout << "Code page \"" + to_string(Charset) + "\" is unsupported in HTML.\n";
		abort();
	}

	string CP = HTMLCharsetsPerm.find(Charset)->second;

	os << "\t\t" << vformat(_Html_Tags::_meta, make_format_args(CP)) << '\n';
	os << "\t\t" << _Html_Tags::_title_begin << Title << _Html_Tags::_title_end << '\n';

	if (CSSPath.empty()){CSSPath = DefaultCSSPath;}

	if (Copy == LinkCSS::Copy)
	{
		CSSPath = MoveCSS(CSSPath, OutputFileName.substr(0, OutputFileName.size() - 5) + ".css");
		os << "\t\t" << vformat(_Html_Tags::_link, make_format_args(CSSPath)) << '\n';
	}
	else if (Copy == Inline)
	{
		ifstream cssf(CSSPath);
		ostringstream tostr;
		tostr << cssf.rdbuf();
		string FinalStr = cse::sc::parser::ConvertChar(tostr.str().c_str(), CSSEncod);
		os << "\t\t" << _Html_Tags::_style_begin << '\n' << FinalStr << "\t\t" << _Html_Tags::_style_end << '\n';
	}
	
	os << "\t" << _Html_Tags::_head_end << '\n';
	return os.str();
}
