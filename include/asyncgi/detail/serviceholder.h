#ifndef ASYNCGI_SERVICE_HOLDER_H
#define ASYNCGI_SERVICE_HOLDER_H

#include <asyncgi/detail/external/sfun/optional_ref.h>
#include <memory>

namespace asyncgi::detail {

template<typename T>
class ServiceHolder {
public:
    explicit ServiceHolder(std::unique_ptr<T>);
    explicit ServiceHolder(sfun::optional_ref<T>);
    ~ServiceHolder();
    ServiceHolder(const ServiceHolder<T>&) = delete;
    ServiceHolder<T>& operator=(const ServiceHolder<T>&) = delete;
    ServiceHolder(ServiceHolder<T>&&) noexcept = default;
    ServiceHolder<T>& operator=(ServiceHolder<T>&&) noexcept = default;

    bool has_value() const;
    T& get();

private:
    std::unique_ptr<T> service_;
    sfun::optional_ref<T> serviceOptionalRef_;
};

} //namespace asyncgi::detail

#endif //ASYNCGI_SERVICE_HOLDER_H
