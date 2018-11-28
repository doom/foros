//
// Created by doom on 05/05/18.
//

#ifndef UTILS_CONSTEXPR_MAP
#define UTILS_CONSTEXPR_MAP

#include <functional>
#include <utils/details/details.hpp>

namespace utils
{
    template <typename Key, typename Value, size_t Size, typename Less = std::less<Key>>
    class map
    {
    private:
        using Array = details::array<std::pair<Key, Value>, Size>;

    public:
        using value_type = typename Array::value_type;
        using key_type = Key;
        using mapped_type = Value;
        using reference = typename Array::reference;
        using const_reference = typename Array::const_reference;
        using pointer = typename Array::pointer;
        using const_pointer = const value_type *;

        using iterator = typename Array::iterator;
        using const_iterator = typename Array::const_iterator;
        using reverse_iterator = typename Array::reverse_iterator;
        using const_reverse_iterator = typename Array::const_reverse_iterator;

        using size_type = typename Array::size_type;
        using difference_type = typename Array::difference_type;

        constexpr map(const value_type (&arr)[Size], const Less &less = {}) noexcept :
            _isLess{less},
            _arr(details::sort(details::array<value_type, Size>(arr), [this](const auto &p1, const auto &p2) {
                return _isLess(p1.first, p2.first);
            }))
        {
        }

        constexpr map(std::initializer_list<value_type> l, const Less &less = {}) noexcept :
            _isLess{less},
            _arr(details::sort(details::array<value_type, Size>(l), [this](const auto &p1, const auto &p2) {
                return _isLess(p1.first, p2.first);
            }))
        {
        }

        constexpr auto begin() noexcept
        {
            return _arr.begin();
        }

        constexpr auto end() noexcept
        {
            return _arr.end();
        }

        constexpr auto cbegin() const noexcept
        {
            return _arr.cbegin();
        }

        constexpr auto cend() const noexcept
        {
            return _arr.cend();
        }

        constexpr auto begin() const noexcept
        {
            return _arr.begin();
        }

        constexpr auto end() const noexcept
        {
            return _arr.end();
        }

        constexpr auto rbegin() noexcept
        {
            return _arr.rbegin();
        }

        constexpr auto rend() noexcept
        {
            return _arr.end();
        }

        constexpr auto crbegin() const noexcept
        {
            return _arr.crbegin();
        }

        constexpr auto crend() const noexcept
        {
            return _arr.crend();
        }

        constexpr auto rbegin() const noexcept
        {
            return _arr.rbegin();
        }

        constexpr auto rend() const noexcept
        {
            return _arr.rend();
        }

        constexpr auto find(const Key &value) const noexcept
        {
            return details::binary_search(_arr.begin(), _arr.end(), value, [this](const auto &p1, const auto &p2) {
                using T1 = std::decay_t<decltype(p1)>;
                using T2 = std::decay_t<decltype(p2)>;

                if constexpr (!std::is_same_v<T1, value_type> && std::is_same_v<T2, value_type>)
                    return _isLess(p1, p2.first);
                else if constexpr (std::is_same_v<T1, value_type> && !std::is_same_v<T2, value_type>)
                    return _isLess(p1.first, p2);
                else
                    return _isLess(p1.first, p2.first);
            });
        }

        constexpr auto &at(const Key &value) const
        {
            const auto it = find(value);

            if (it == end())
                throw std::out_of_range("doom::constexpr_map: out of range");
            return it->second;
        }

        constexpr auto &operator[](const Key &value) const
        {
            const auto it = find(value);

            return it->second;
        }

        constexpr bool contains(const Key &value) const noexcept
        {
            return find(value) != _arr.end();
        }

        constexpr size_type count(const Key &value) const noexcept
        {
            return contains(value) ? 1 : 0;
        }

        constexpr size_type size() const noexcept
        {
            return _arr.size();
        }

        constexpr bool empty() const noexcept
        {
            return _arr.empty();
        }

    private:
        Less _isLess;
        Array _arr;
    };
}

#endif //UTILS_CONSTEXPR_MAP
