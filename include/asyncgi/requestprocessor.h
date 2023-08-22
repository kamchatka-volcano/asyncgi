#ifndef ASYNCGI_REQUESTPROCESSOR_H
#define ASYNCGI_REQUESTPROCESSOR_H

#include "request.h"
#include "response.h"
#include "detail/external/sfun/functional.h"
#include <functional>

namespace asyncgi {

namespace detail {
class ResponseContext;

template<typename TRequestProcessorArgs>
constexpr void checkRequestProcessorSignature(const TRequestProcessorArgs&)
{
    constexpr auto args = TRequestProcessorArgs{};
    static_assert(args.size() == 2);
    static_assert(std::is_same_v<const asyncgi::Request&, typename decltype(sfun::get<args.size() - 2>(args))::type>);
    static_assert(std::is_same_v<asyncgi::Response&, typename decltype(sfun::get<args.size() - 1>(args))::type>);
}
} // namespace detail

class RequestProcessor {
public:
    template<
            typename TRequestProcessorFunc,
            typename std::enable_if_t<
                    !std::is_same_v<std::remove_reference_t<TRequestProcessorFunc>, RequestProcessor>>* = nullptr>
    RequestProcessor(TRequestProcessorFunc&& requestProcessor)
    {
        constexpr auto args = sfun::callable_args<TRequestProcessorFunc>{};
        detail::checkRequestProcessorSignature(args);

        if constexpr (std::is_lvalue_reference_v<TRequestProcessorFunc>) {
            requestProcessorInvoker_ = [&requestProcessor](
                                               const Request& request,
                                               std::shared_ptr<detail::ResponseContext> responseContext)
            {
                auto response = Response{std::move(responseContext)};
                requestProcessor(request, response);
            };
        }
        else {
            requestProcessorInvoker_ = [requestProcessor = std::forward<TRequestProcessorFunc>(requestProcessor)](
                                               const Request& request,
                                               std::shared_ptr<detail::ResponseContext> responseContext)
            {
                auto response = Response{std::move(responseContext)};
                requestProcessor(request, response);
            };
        }
    }

    void operator()(const Request& request, std::shared_ptr<detail::ResponseContext> response)
    {
        requestProcessorInvoker_(request, response);
    }

private:
    std::function<void(const Request& request, std::shared_ptr<detail::ResponseContext> response)>
            requestProcessorInvoker_;
};

} // namespace asyncgi

#endif //ASYNCGI_REQUESTPROCESSOR_H