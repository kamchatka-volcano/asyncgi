#ifndef ASYNCGI_REQUESTPROCESSOR_H
#define ASYNCGI_REQUESTPROCESSOR_H

#include "request.h"
#include "response.h"
#include "detail/external/sfun/functional.h"
#include <functional>

namespace asyncgi {

namespace detail {
class ResponseContext;

template<typename TRequestProcessorReturnType, typename TRequestProcessorArgs>
constexpr void checkRequestProcessorSignature()
{
    if constexpr (std::is_same_v<TRequestProcessorReturnType, void>) {
        constexpr auto args = TRequestProcessorArgs{};
        static_assert(args.size() == 2);
        static_assert(std::is_same_v<const asyncgi::Request&, typename decltype(sfun::get<0>(args))::type>);
        static_assert(std::is_same_v<asyncgi::Response&, typename decltype(sfun::get<1>(args))::type>);
    }
    else {
        static_assert(
                std::is_same_v<TRequestProcessorReturnType, http::Response> ||
                std::is_same_v<TRequestProcessorReturnType, fastcgi::Response>);
        constexpr auto args = TRequestProcessorArgs{};
        static_assert(args.size() == 1);
        static_assert(std::is_same_v<const asyncgi::Request&, typename decltype(sfun::get<0>(args))::type>);
    }
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
        detail::checkRequestProcessorSignature<
                sfun::callable_return_type<TRequestProcessorFunc>,
                sfun::callable_args<TRequestProcessorFunc>>();

        if constexpr (std::is_lvalue_reference_v<TRequestProcessorFunc>) {
            requestProcessorInvoker_ = [&requestProcessor](
                                               const Request& request,
                                               std::shared_ptr<detail::ResponseContext> responseContext)
            {
                if constexpr (std::is_same_v<sfun::callable_return_type<TRequestProcessorFunc>, void>) {
                    auto response = Response{std::move(responseContext)};
                    requestProcessor(request, response);
                }
                else {
                    auto response = requestProcessor(request);
                    Response{std::move(responseContext)}.send(response);
                }
            };
        }
        else {
            requestProcessorInvoker_ = [requestProcessor = std::forward<TRequestProcessorFunc>(requestProcessor)](
                                               const Request& request,
                                               std::shared_ptr<detail::ResponseContext> responseContext)
            {
                if constexpr (std::is_same_v<sfun::callable_return_type<TRequestProcessorFunc>, void>) {
                    auto response = Response{std::move(responseContext)};
                    requestProcessor(request, response);
                }
                else {
                    auto response = requestProcessor(request);
                    Response{std::move(responseContext)}.send(response);
                }
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