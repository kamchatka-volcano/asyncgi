#pragma once
#include "errors.h"
#include "iapp.h"
#include "requestrouter.h"
#include <memory>
#include <string>

namespace asyncgi {
std::unique_ptr<IApp> makeApp(std::size_t workerThreadCount = 1);

template<typename TRouteContext = _>
RequestRouter<TRouteContext> makeRouter()
{
    return RequestRouter<TRouteContext>{};
}

} // namespace asyncgi
