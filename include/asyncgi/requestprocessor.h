#pragma once
#include "request.h"
#include "response.h"

namespace asyncgi{

class RequestProcessor{
public:
    virtual ~RequestProcessor() = default;
    virtual void process(const Request&, Response&) = 0;
};

template <typename TResponseContext>
class ContextualRequestProcessor{
public:
    virtual ~ContextualRequestProcessor() = default;
    virtual void process(const Request&, ContextualResponse<TResponseContext>&) = 0;
};


}
