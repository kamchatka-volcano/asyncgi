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

} //namespace asyncgi::detail

#endif //ASYNCGI_UTILS_H
