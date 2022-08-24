#pragma once

namespace asyncgi{
template<class TRouteContext>
class RequestProcessor;
class Request;
class ResponseContext;

namespace detail{

class IRequestProcessor {
public:
    IRequestProcessor() = default;
    virtual ~IRequestProcessor() = default;
    IRequestProcessor(const IRequestProcessor&) = delete;
    IRequestProcessor& operator=(const IRequestProcessor&) = delete;
    IRequestProcessor(IRequestProcessor&&) = delete;
    IRequestProcessor&& operator=(IRequestProcessor&&) = delete;

    virtual void process(const Request&, ResponseContext&) = 0;

private:
    template<class TRouteContext>
    friend class asyncgi::RequestProcessor;
};

}
}