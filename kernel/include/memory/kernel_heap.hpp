/*
** Created by doom on 10/11/18.
*/

#ifndef FOROS_KERNEL_HEAP_HPP
#define FOROS_KERNEL_HEAP_HPP

#include <cstddef>
#include <utils/singleton.hpp>
#include <utils/optional.hpp>
#include <memory/definitions.hpp>
#include <memory/physical_frame.hpp>

namespace foros::memory
{
    class kernel_heap : public utils::singleton<kernel_heap>
    {
    public:
        void initialize(const multiboot2::boot_information &boot_info,
                        virtual_address start_address,
                        virtual_address end_address) noexcept;

        void *allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept;

        void deallocate(void *ptr, std::size_t size, std::size_t align = alignof(std::max_align_t)) noexcept;

    private:
        std::optional<memory::physical_frame_allocator> _frame_allocator;
    public:
        virtual_address _start_addr{0};
        virtual_address _end_addr{0};
        virtual_address _current_addr{0};
    };

    template <typename T>
    class kheap_allocator
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;

        template <class U>
        struct rebind
        {
            using other = kheap_allocator<U>;
        };

        using is_always_equal = std::true_type;

        T *allocate(std::size_t n)
        {
            return reinterpret_cast<T *>(kernel_heap::instance().allocate(sizeof(T) * n, alignof(T)));
        }

        T *allocate(std::size_t n, [[maybe_unused]] const void *hint)
        {
            return allocate(n);
        }

        void deallocate(T *p, std::size_t n)
        {
            kernel_heap::instance().deallocate((void *)p, sizeof(T) * n, alignof(T));
        }

        template <typename U, typename ...Args>
        void construct(U *p, Args &&...args)
        {
            ::new((void *)p) U(std::forward<Args>(args)...);
        }

        template <typename U>
        void destroy(U *p)
        {
            p->~U();
        }

        pointer address(reference x) const
        {
            return std::addressof(x);
        }

        const_pointer address(const_reference x) const
        {
            return std::addressof(x);
        }
    };
}

#endif /* !FOROS_KERNEL_HEAP_HPP */
