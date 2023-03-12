#include "gbuffer_html.h"

#include <sstream>
#include <format>

using namespace std;

string MakeHTMLHead(string Title, string CSSPath)
{
	ostringstream os;
	os << "\t" << _Html_Tags::_head_begin << '\n';
	if (!CSSPath.empty())
	{
		os << "\t\t" << vformat(_Html_Tags::_link, make_format_args(CSSPath)) << '\n';
	}
	os << "\t\t" << _Html_Tags::_meta << '\n';
	os << "\t\t" << _Html_Tags::_title_begin << Title << _Html_Tags::_title_end << '\n';
	os << "\t" << _Html_Tags::_head_end << '\n';
	return os.str();
}
