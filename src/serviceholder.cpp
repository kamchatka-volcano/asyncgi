#include "asiodispatcherservice.h"
#include "clientservice.h"
#include "timerservice.h"
#include <asyncgi/detail/serviceholder.h>

namespace asyncgi::detail {

template<typename T>
ServiceHolder<T>::ServiceHolder(std::unique_ptr<T> service)
    : service_{std::move(service)}
{
}

template<typename T>
ServiceHolder<T>::ServiceHolder(sfun::optional_ref<T> service)
    : serviceOptionalRef_{service}
{
}

template<typename T>
ServiceHolder<T>::~ServiceHolder() = default;

template<typename T>
T& ServiceHolder<T>::get()
{
    if (service_)
        return *service_;
    return *serviceOptionalRef_;
}

template<typename T>
bool ServiceHolder<T>::has_value() const
{
    return service_ || serviceOptionalRef_;
}

template class ServiceHolder<AsioDispatcherService>;
template class ServiceHolder<ClientService>;
template class ServiceHolder<TimerService>;

} //namespace asyncgi::detail