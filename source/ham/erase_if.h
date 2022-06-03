#ifndef ERASE_IF_H
#define ERASE_IF_H

#include <vector>
#include <algorithm>
#include <set>

#if __cplusplus > 201703L

using std::erase_if;

#else  // __cplusplus

// https://en.cppreference.com/w/cpp/container/vector/erase2
template<class T, class Alloc, class Pred>
constexpr typename std::vector<T,Alloc>::size_type erase_if(std::vector<T,Alloc>& c, Pred pred) {
	auto it = std::remove_if(c.begin(), c.end(), pred);
	auto r = std::distance(it, c.end());
	c.erase(it, c.end());
	return r;
}

// https://en.cppreference.com/w/cpp/container/set/erase_if
template<class Key, class Compare, class Alloc, class Pred>
typename std::set<Key,Compare,Alloc>::size_type erase_if(std::set<Key,Compare,Alloc>& c, Pred pred) {
	auto old_size = c.size();
	for (auto i = c.begin(), last = c.end(); i != last; ) {
		if (pred(*i)) {
			i = c.erase(i);
		} else {
			++i;
		}
	}
	return old_size - c.size();
}

#endif  // __cplusplus
#endif  // ERASE_IF_H
