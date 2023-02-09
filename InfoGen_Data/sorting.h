#pragma once

#ifndef __SORTING__
#define __SORTING__

#include <vector>
#include <memory>
#include <CSE/Object.h>
#include <functional>
#include <xutility>

_EXTERN_C

void __INSERTION_SORT_OBJECT
(
	std::shared_ptr<cse::Object>* const First,
	std::shared_ptr<cse::Object>* const Last,
	std::function<bool(std::shared_ptr<cse::Object>, std::shared_ptr<cse::Object>)> Cmp
);

void __HEAP_SORT_OBJECT
(
	std::shared_ptr<cse::Object>* First,
	std::shared_ptr<cse::Object>* Last,
	std::function<bool(std::shared_ptr<cse::Object>, std::shared_ptr<cse::Object>)> Cmp
);

void __Smart_Sort_Engine
(
	std::shared_ptr<cse::Object>* First,
	std::shared_ptr<cse::Object>* Last,
	std::_Iter_diff_t<std::vector<std::shared_ptr<cse::Object>>::iterator> Ideal,
	std::function<bool(std::shared_ptr<cse::Object>, std::shared_ptr<cse::Object>)> Cmp
);

_END_EXTERN_C

#endif