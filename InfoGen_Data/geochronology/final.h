#pragma once

#ifndef __GCL_FINAL__
#define __GCL_FINAL__

#include <vector>
#include <CSE/SCStream.h>

extern bool CustomModel;

void geochronology(cse::ISCStream& SystemIn, std::vector<std::string> args);

#endif