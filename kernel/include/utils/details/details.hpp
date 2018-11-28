//
// Created by doom on 05/05/18.
//

#ifndef UTILS_CONSTEXPR_MAP_DETAILS_HPP
#define UTILS_CONSTEXPR_MAP_DETAILS_HPP

#include <cstddef>
#include <utility>
#include <iterator>
#include <initializer_list>

namespace utils
{
    namespace details
    {
        template <class InputIt, class UnaryPredicate>
        constexpr InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q) noexcept
        {
            for (; first != last; ++first) {
                if (!q(*first)) {
                    return first;
                }
            }
            return last;
        }

        template <typename Iterator>
        constexpr void iter_swap(Iterator a, Iterator b) noexcept
        {
            auto tmp = std::move(*a);
            *a = std::move(*b);
            *b = std::move(tmp);
        }

        template <typename Iterator>
        constexpr auto distance(Iterator first, Iterator last) noexcept
        {
            return last - first;
        }

        template <typename Iterator>
        constexpr auto next(Iterator first, size_t n = 1) noexcept
        {
            return first + n;
        }

        template <class Iterator, class Pred>
        constexpr Iterator partition(Iterator first, Iterator last, Pred p) noexcept
        {
            first = details::find_if_not(first, last, p);
            if (first == last)
                return first;

            for (Iterator i = details::next(first); i != last; ++i) {
                if (p(*i)) {
                    iter_swap(i, first);
                    ++first;
                }
            }
            return first;
        }

        template <typename T, size_t Size>
        class array
        {
        private:
            template <size_t ...indexes>
            constexpr array(const T (&arr)[Size], std::index_sequence<indexes...>) noexcept : _arr{arr[indexes]...}
            {
            }

            template <typename Iterator, size_t ...indexes>
            constexpr array(Iterator it, std::index_sequence<indexes...>) noexcept : _arr{((void)indexes, *it++)...}
            {
            }

        public:
            using value_type = T;
            using reference = value_type &;
            using const_reference = const value_type &;
            using pointer = value_type *;
            using const_pointer = const value_type *;

            using iterator = pointer;
            using const_iterator = const_pointer;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;

            constexpr array(const T (&arr)[Size]) noexcept : array(arr, std::make_index_sequence<Size>{})
            {
            }

            constexpr array(std::initializer_list<T> l) noexcept : array(l.begin(), std::make_index_sequence<Size>{})
            {
            }

            constexpr iterator begin() noexcept
            {
                return std::begin(_arr);
            }

            constexpr iterator end() noexcept
            {
                return std::end(_arr);
            }

            constexpr const_iterator cbegin() const noexcept
            {
                return std::cbegin(_arr);
            }

            constexpr const_iterator cend() const noexcept
            {
                return std::cend(_arr);
            }

            constexpr const_iterator begin() const noexcept
            {
                return cbegin();
            }

            constexpr const_iterator end() const noexcept
            {
                return cend();
            }

            constexpr reverse_iterator rbegin() noexcept
            {
                return std::reverse_iterator(end());
            }

            constexpr reverse_iterator rend() noexcept
            {
                return std::reverse_iterator(begin());
            }

            constexpr const_reverse_iterator crbegin() const noexcept
            {
                return std::reverse_iterator(cend());
            }

            constexpr const_reverse_iterator crend() const noexcept
            {
                return std::reverse_iterator(cbegin());
            }

            constexpr const_reverse_iterator rbegin() const noexcept
            {
                return crbegin();
            }

            constexpr const_reverse_iterator rend() const noexcept
            {
                return crend();
            }

            constexpr size_type size() const noexcept
            {
                return Size;
            }

            constexpr bool empty() const noexcept
            {
                return size() == 0;
            }

            constexpr const T &operator[](size_t idx) const noexcept
            {
                return _arr[idx];
            }

        private:
            T _arr[Size];
        };

        template <typename Iterator, typename Less>
        constexpr void qsort(Iterator begin, Iterator end, Less isLess) noexcept
        {
            const auto len = details::distance(begin, end);
            if (len <= 1)
                return;
            const auto pivot = *details::next(begin, len / 2);
            const auto middle1 = details::partition(begin, end, [isLess, pivot](const auto &elem) {
                return isLess(elem, pivot);
            });
            const auto middle2 = details::partition(middle1, end, [isLess, pivot](const auto &elem) {
                return !isLess(pivot, elem);
            });
            qsort(begin, middle1, isLess);
            qsort(middle2, end, isLess);
        }

        template <typename T, size_t size, typename Less>
        constexpr auto sort(details::array<T, size> arr, Less isLess) noexcept
        {
            auto sorted = arr;
            details::qsort(sorted.begin(), sorted.end(), isLess);
            return sorted;
        }

        template <typename Iterator, typename Value, typename Less>
        constexpr Iterator binary_search(Iterator begin, Iterator end, const Value &val, Less less)
        {
            const auto real_end = end;

            while (begin != end) {
                if (!less(*begin, val) && !less(val, *begin))
                    return begin;
                const auto len = details::distance(begin, end);
                if (len <= 1)
                    return real_end;
                const auto pivot_it = details::next(begin, len / 2);
                if (less(val, *pivot_it))
                    end = pivot_it;
                else
                    begin = pivot_it;
            }
            return real_end;
        }
    };
}

#endif //UTILS_CONSTEXPR_MAP_DETAILS_HPP
