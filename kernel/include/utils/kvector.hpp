/*
** Created by doom on 19/11/18.
*/

#ifndef FOROS_UTILS_KVECTOR_HPP
#define FOROS_UTILS_KVECTOR_HPP

#include <vector>
#include <memory/kernel_heap.hpp>

template <typename T>
using kvector = std::vector<T, foros::memory::kheap_allocator<T>>;

#endif /* !FOROS_UTILS_KVECTOR_HPP */
