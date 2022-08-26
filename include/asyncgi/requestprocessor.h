#pragma once
#include "request.h"
#include "response.h"
#include "detail/irequestprocessor.h"

namespace asyncgi{

template <typename TRouteContext = detail::EmptyRouteContext>
class RequestProcessor : public detail::IRequestProcessor{
public:
    virtual void process(const Request&, Response<TRouteContext>&) = 0;

private:
    void processRequest(const Request& request, detail::ResponseContext& response) override
    {
        auto contextualResponse = Response<TRouteContext>{response};
        process(request, contextualResponse);
    };
};

}
