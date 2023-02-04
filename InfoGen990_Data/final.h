#pragma once

#ifndef __FINAL__
#define __FINAL__

#include <vector>
#include <CSE/SCStream.h>
#include <CSE/Object.h>

enum OutputFormat
{
	HTML,
	MD
};

extern OutputFormat OFormat;
extern std::string Final;

using ObjectBuffer = std::vector<cse::Object>;
extern ObjectBuffer System;

#endif