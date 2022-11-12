#pragma once
#include "request.h"
#include "response.h"
#include "detail/responsecontext.h"
#include "detail/utils.h"
#include <functional>

namespace asyncgi{

class RequestProcessor{
public:
	template<typename TRequestProcessorFunc, typename std::enable_if_t<!std::is_same_v<TRequestProcessorFunc, RequestProcessor>>* = nullptr>
	RequestProcessor(TRequestProcessorFunc& requestProcessor)
	{
        using args = detail::callable_args<TRequestProcessorFunc>;
        detail::checkRequestProcessorSignature<args>();
        auto list = args{};
        constexpr auto argsSize = detail::type_list_size<args>;
        [[maybe_unused]] auto responseTypeElement = std::get<argsSize - 1>(list);
        using responseType = std::decay_t<detail::unwrap_type<decltype(responseTypeElement)>>;
		requestProcessorInvoker_ = [&requestProcessor](const Request& request, detail::ResponseContext& response)
		{
            auto contextualResponse = responseType{response};
            requestProcessor(request, contextualResponse);
		};
	}

	void operator()(const Request& request, detail::ResponseContext& response)
	{
        requestProcessorInvoker_(request, response);
	}

private:
	std::function<void(const Request& request, detail::ResponseContext& response)> requestProcessorInvoker_;
};

}
