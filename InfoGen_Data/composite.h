#pragma once

#ifndef __COMPOSITE0__
#define __COMPOSITE0__

#include <algorithm>
#include <map>
#include <vector>
#include <string>

#include "gbuffer_basic.h"

extern uint32_t _OUT_PRECISION;

extern std::map<std::string, std::vector<size_t>> TypeIndices;
extern std::map<std::string, std::vector<size_t>> Companions;
extern std::string SystemBarycenter;

ObjectBuffer::iterator FindSystemBarycenter(ObjectBuffer& Sys);

void SortSystemType(ObjectBuffer Sys);
void SortParentBody(ObjectBuffer Sys);
std::string GHMDSystemInfoTable(SystemInfo Info);

void composite();

#endif