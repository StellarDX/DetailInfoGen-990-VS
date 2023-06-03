#include "gbuffer_html.h"

#include <sstream>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <windows.h>

using namespace std;

string DefaultCSSPath = "./InfoGen_Data/html_themes/Default.css";
int CSSEncod = 65001;
extern string OutputFileName;

map<int, string> HTMLCharsetsPerm
{
	// Reference: https://html.spec.whatwg.org/multipage/parsing.html#character-encodings
	{65001, "UTF-8"       }, {28592, "ISO-8859-2"  }, {28597, "ISO-8859-7"  }, {28598, "ISO-8859-8"  },
	{  874, "windows-874" }, { 1250, "windows-1250"}, { 1251, "windows-1251"}, { 1252, "windows-1252"},
	{ 1254, "windows-1254"}, { 1255, "windows-1255"}, { 1256, "windows-1256"}, { 1257, "windows-1257"},
	{ 1258, "windows-1258"}, {  936, "GBK"         }, {  950, "Big5"        }, {50220, "ISO-2022-JP" },
	{  932, "Shift_JIS"   }, {51949, "EUC-KR"      }, { 1201, "UTF-16BE"    }, { 1200, "UTF-16LE"    }
};

map<int, string> HTMLCharsets4b1d
{
	/*{     , "CESU-8"      },*/ {65000, "UTF-7"      }, /*{      , "BOCU-1"      },*/ 
	/*{     , "SCSU"        },*/ /*{     , "EBCDIC"     },*/ /*{      , "UTF-32"      }*/
};

string MoveCSS(string _Src, string _Dst)
{
	ifstream BufferS(_Src);
	struct stat BufferD;
	bool IsCopy = true;
	if (!BufferS)
	{
		cout << "Source css file is not exist.\n";
		abort();
	}
	if (!stat(_Dst.c_str(), &BufferD))
	{
		cout << "Destination css file is already exist, cover this file? (Y/N)\n";
		char YN;
		cin >> YN;
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
	if (IsCopy)
	{
		CopyFileA(_Src.c_str(), _Dst.c_str(), false);
	}

	string FileNameTmp;
	vector<string> FileName;
	for (size_t i = 0; i < _Dst.size(); i++)
	{
		if (_Dst[i] == '/') { _Dst[i] = '\\'; }
	}
	istringstream in(_Dst);
	while (getline(in, FileNameTmp, '\\'))
	{
		FileName.push_back(FileNameTmp);
	}
	return FileName.back();
}

string MakeHTMLHead(string Title, int Charset, string CSSPath, LinkCSS Copy)
{
	ostringstream os;
	os << "\t" << _Html_Tags::_head_begin << '\n';

	if (HTMLCharsets4b1d.find(Charset) != HTMLCharsets4b1d.end() ||
		HTMLCharsetsPerm.find(Charset) == HTMLCharsetsPerm.end())
	{
		cout << "Code page is not used or forbidden in HTML.\n";
		abort();
	}

	string CP = HTMLCharsetsPerm.find(Charset)->second;

	os << "\t\t" << vformat(_Html_Tags::_meta, make_format_args(CP)) << '\n';
	os << "\t\t" << _Html_Tags::_title_begin << Title << _Html_Tags::_title_end << '\n';

	if (CSSPath.empty())
	{
		CSSPath = DefaultCSSPath;
		os << "\t\t" << vformat(_Html_Tags::_link, make_format_args(CSSPath)) << '\n';
	}
	if (Copy == LinkCSS::Copy)
	{
		CSSPath = MoveCSS(CSSPath, OutputFileName.substr(0, OutputFileName.size() - 5) + ".css");
		os << "\t\t" << vformat(_Html_Tags::_link, make_format_args(CSSPath)) << '\n';
	}
	if (Copy == Inline)
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
