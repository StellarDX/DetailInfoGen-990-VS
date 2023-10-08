#pragma once

#ifndef __FINAL__
#define __FINAL__

// Check CSpaceEngine has installed
#ifdef __has_include
#if !__has_include(<CSE/CSE.h>)
#error "Compile or install CSpaceEngine to continue."
#endif
#endif

#include <vector>
#include <CSE/SCStream.h>
#include <CSE/Object.h>

enum OutputFormat
{
	HTML,
	MD
};

enum LinkCSS { Static, Copy, Inline };

extern bool Astrobiology;

extern OutputFormat OFormat;
extern std::string Final;
extern bool MakeDir;
using ObjectBuffer = std::vector<cse::Object>;
extern ObjectBuffer System;
extern std::string LcID;

extern std::map<std::string, std::string> LocStrings;

extern std::string CSSPath;
extern LinkCSS LCSS;

extern int outencoding;
extern std::string OutputFileName;

void ParseLocalStrings(std::string FileName, std::string LCID, UINT CP);
void Transcode(std::string& arg, int encoding);
void NormalProcess(cse::ISCStream& SystemIn, std::vector<std::string> args);

void HTMLWrite();

#endif