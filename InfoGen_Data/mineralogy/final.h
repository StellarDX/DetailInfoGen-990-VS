#pragma once

#ifndef __MIN_FINAL__
#define __MIN_FINAL__

#include <set>
#include <CSE/SCStream.h>

extern std::set<std::string> OreDict;

void minerals(cse::ISCStream& SystemIn, std::vector<std::string> args);

#endif