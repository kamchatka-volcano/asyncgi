#include "responsesender.h"

namespace asyncgi::detail {

ResponseSender::ResponseSender(fcgi::Response response)
    : response_{std::move(response)}
{
}

void ResponseSender::send(std::string data)
{
    send(data, {});
}

void ResponseSender::send(std::string data, std::string errorMsg)
{
    if (!response_.isValid())
        return;

    response_.setData(std::move(data));
    response_.setErrorMsg(std::move(errorMsg));
    response_.send();
}

bool ResponseSender::isSent() const
{
    return !response_.isValid();
}

} // namespace asyncgi::detail