#pragma once

#ifndef __COMPOSITE1__
#define __COMPOSITE1__

#include <memory>
#include <vector>
#include <stack>
#include <queue>
#include <CSE/Object.h>

struct SystemStruct
{
	struct Elem
	{
		std::shared_ptr<cse::Object> Pointer;
		std::shared_ptr<SystemStruct> SubSystem;
	};

	std::vector<Elem> Catalog;
};

extern std::shared_ptr<SystemStruct> SystemStructure;
extern std::stack<SystemStruct::Elem> ParentBodyStack;
extern std::shared_ptr<SystemStruct> CurrentSubSystem;

void composite1();

#endif