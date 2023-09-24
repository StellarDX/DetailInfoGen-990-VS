#pragma once

#ifndef __GCL_FINAL__
#define __GCL_FINAL__

#include <vector>
#include <CSE/SCStream.h>

#include "DataBuffer.h"
#include "html/gbuffer_html.h"

extern cse::ISCStream SysIn;
extern bool CustomModel;
extern enum OutputFormat OFormat;
extern std::string Final;
extern UINT srcencoding;
extern UINT outencoding;
extern std::string OutputFileName;

extern std::string CSSPath;
extern enum LinkCSS LCSS;

extern std::map<std::string, std::string> LocStrings;

extern enum CopyOption Cpm;
extern std::string HTMLhead;
extern std::string HTMLcontent;
extern std::string HTMLMenu;
extern int CSSEncod;

//void geochronology(cse::ISCStream& SystemIn, std::vector<std::string> args);

#endif