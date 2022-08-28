#pragma once
#include <optional>
#include <functional>

namespace asyncgi::detail{

template<typename T>
class LazyInitialized{
public:
    explicit LazyInitialized(std::function<T()> initializer)
        : initializer_{std::move(initializer)}
    {}
    operator const T&() const
    {
        return get();
    }
    const T& get() const
    {
        if (!value)
            value = initializer_();
        return *value;
    }

    operator T&()
    {
        return get();
    }
    T& get()
    {
        if (!value)
            value = initializer_();
        return *value;
    }
private:
    mutable std::optional<T> value;
    std::function<T()> initializer_;
};

}