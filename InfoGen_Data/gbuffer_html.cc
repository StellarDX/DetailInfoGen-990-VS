#include "gbuffer_html.h"

#include <sstream>
#include <format>
#include <fstream>
#include <iostream>
#include <vector>
#include <windows.h>
#include <tchar.h>

using namespace std;

string DefaultCSSPath = "./InfoGen_Data/html_themes/Default.css";
extern string OutputFileName;

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
		ofstream os(_Dst);
		os << BufferS.rdbuf();
		os.close();
		BufferS.close();
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

string MakeHTMLHead(string Title, string CSSPath, bool Copy)
{
	ostringstream os;
	os << "\t" << _Html_Tags::_head_begin << '\n';
	if (!CSSPath.empty())
	{
		os << "\t\t" << vformat(_Html_Tags::_link, make_format_args(CSSPath)) << '\n';
	}
	os << "\t\t" << _Html_Tags::_meta << '\n';
	os << "\t\t" << _Html_Tags::_title_begin << Title << _Html_Tags::_title_end << '\n';

	if (CSSPath.empty()) { CSSPath = DefaultCSSPath; }
	if (Copy)
	{
		CSSPath = MoveCSS(CSSPath, OutputFileName.substr(0, OutputFileName.size() - 5) + ".css");
	}
	os << "\t\t" << "<link rel=\"stylesheet\" type=\"text/css\" href=\"" + CSSPath + "\">" << '\n';

	os << "\t" << _Html_Tags::_head_end << '\n';
	return os.str();
}
