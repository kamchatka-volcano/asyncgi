#pragma once
#include "iapp.h"
#include "requestrouter.h"
#include "errors.h"
#include <string>
#include <memory>

namespace asyncgi {
std::unique_ptr<IApp> makeApp(std::size_t workerThreadCount = 1);

template<typename TRouteContext = detail::EmptyRouteContext>
RequestRouter<TRouteContext> makeRouter()
{
    return RequestRouter<TRouteContext>{};
}

}

