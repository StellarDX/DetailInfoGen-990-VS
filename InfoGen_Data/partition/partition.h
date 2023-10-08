#pragma once

#ifndef __PARTITION__
#define __PARTITION__

#include <string>
#include <vector>
#include <CSE/SCStream.h>

extern bool PartitionReload;

void partition(cse::ISCStream& SystemIn, std::vector<std::string> args);

#endif