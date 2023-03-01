#include <algorithm>
#include "sorting.h"

using namespace std;
using namespace cse;

// Optimized algorithms reference:
// https://zhuanlan.zhihu.com/p/20652569

_EXTERN_C

// Insertion sort
// https://en.wikipedia.org/wiki/Insertion_sort
void __INSERTION_SORT_OBJECT
(
	shared_ptr<Object>* const First,
	shared_ptr<Object>* const Last,
	function<bool(shared_ptr<Object>, shared_ptr<Object>)> Cmp
)
{
	if (First != Last)
	{
		for (auto it = First; it != Last; ++it) 
		{
			auto const insertion = upper_bound(First, it, *it, Cmp);
			rotate(insertion, it, next(it));
			assert(is_sorted(First, next(it), Cmp));
		}
	}
}

// Heapsort
// https://en.wikipedia.org/wiki/Heapsort
void __HEAP_SORT_OBJECT
(
	shared_ptr<Object>* First,
	shared_ptr<Object>* Last,
	function<bool(shared_ptr<Object>, shared_ptr<Object>)> Cmp
)
{
	// Make heap
	for (auto it = First; it != Last;)
	{
		push_heap(First, ++it, Cmp);
		assert(is_heap(First, it, Cmp));
	}
	assert(is_heap(First, Last, Cmp));

	// Sort heap
	for (auto it = Last; it != First;)
	{
		pop_heap(First, it--, Cmp);
		assert(is_heap(First, it, Cmp));
	}
}

// A hybrid sorting engine by Miceosoft, taken from <algorithm> header file (Apache lic.)
void __Smart_Sort_Engine
(
	shared_ptr<Object>* First, 
	shared_ptr<Object>* Last,
	_Iter_diff_t<vector<shared_ptr<Object>>::iterator> Ideal, 
	function<bool(shared_ptr<Object>, shared_ptr<Object>)> Cmp
)
{
	while (1)
	{
		if (Last - First <= _ISORT_MAX)
		{
			__INSERTION_SORT_OBJECT(First, Last, Cmp);
			return;
		}

		if (Ideal <= 0)
		{
			__HEAP_SORT_OBJECT(First, Last, Cmp);
			return;
		}

		auto Mid = _Partition_by_median_guess_unchecked(First, Last, Cmp);
		Ideal = (Ideal >> 1) + (Ideal >> 2);

		if (Mid.first - First < Last - Mid.second)
		{
			__Smart_Sort_Engine(First, Mid.first, Ideal, Cmp);
			First = Mid.second;
		}
		else
		{
			__Smart_Sort_Engine(Mid.second, Last, Ideal, Cmp);
			Last = Mid.first;
		}
	}
}

_END_EXTERN_C