#ifndef ASYNCGI_SERVICE_HOLDER_H
#define ASYNCGI_SERVICE_HOLDER_H

#include <memory>

namespace asyncgi::detail {

template<typename T>
class ServiceHolder {
public:
    explicit ServiceHolder(std::unique_ptr<T>);
    explicit ServiceHolder(T&);
    ~ServiceHolder();
    ServiceHolder(const ServiceHolder<T>&) = delete;
    ServiceHolder<T>& operator=(const ServiceHolder<T>&) = delete;
    ServiceHolder(ServiceHolder<T>&&) noexcept = default;
    ServiceHolder<T>& operator=(ServiceHolder<T>&&) noexcept = default;

    T& get();

private:
    std::unique_ptr<T> service_;
    T* servicePtr_ = nullptr;
};

} //namespace asyncgi::detail

#endif //ASYNCGI_SERVICE_HOLDER_H
