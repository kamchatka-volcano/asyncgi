#pragma once
#include "iapp.h"
#include "requestrouter.h"
#include "errors.h"
#include <string>
#include <memory>

namespace asyncgi {
std::unique_ptr<IApp> makeApp(std::size_t workerThreadCount = 1);

template<typename TContext = detail::EmptyContext>
RequestRouter<TContext> makeRouter()
{
    return RequestRouter<TContext>{};
}

}

