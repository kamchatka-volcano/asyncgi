#ifndef ASYNCGI_UTILS_H
#define ASYNCGI_UTILS_H

#include <memory>

namespace asyncgi::detail {

template<class TFunc>
auto makeCopyableLambda(TFunc&& f)
{
    auto funcPtr = std::make_shared<std::decay_t<TFunc>>(std::forward<TFunc>(f));
    return [funcPtr](auto&&... args) -> decltype(auto)
    {
        return (*funcPtr)(decltype(args)(args)...);
    };
}

template<typename T>
decltype(auto) refWrapperOrRValue(T&& obj)
{
    if constexpr (std::is_lvalue_reference_v<decltype(obj)>)
        return std::ref(std::forward<T>(obj));
    else
        return std::forward<T>(obj);
}

} //namespace asyncgi::detail

#endif //ASYNCGI_UTILS_H
