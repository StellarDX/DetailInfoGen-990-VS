#pragma once

#ifndef __COMPOSITE0__
#define __COMPOSITE0__

#include <algorithm>
#include <map>
#include <vector>
#include <string>

#include "gbuffer_basic.h"

#define _OUT_PRECISISION 15

extern std::map<std::string, std::vector<size_t>> TypeIndices;
extern std::map<std::string, std::vector<size_t>> Companions;

void SortSystemType(ObjectBuffer Sys);
void SortParentBody(ObjectBuffer Sys);
std::string SystemInfoTable(SystemInfo Info);

void composite(int argc, char const* argv[]);

#endif