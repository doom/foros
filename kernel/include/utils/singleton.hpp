/*
** Created by doom on 08/10/18.
*/

#ifndef UTILS_SINGLETON_HPP
#define UTILS_SINGLETON_HPP

namespace utils
{
    template <typename T>
    class singleton
    {
    public:
        static T &instance() noexcept
        {
            static T instance;

            return instance;
        }
    };
}

#endif /* !UTILS_SINGLETON_HPP */
