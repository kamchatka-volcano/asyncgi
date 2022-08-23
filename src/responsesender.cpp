#include "responsesender.h"

namespace asyncgi::detail{

ResponseSender::ResponseSender(fcgi::Response response)
    : response_{std::move(response)}
{}

void ResponseSender::send(std::string data)
{
    if (!response_.isValid())
        return;

    response_.setData(std::move(data));
    response_.send();
}

bool ResponseSender::isSent() const
{
    return !response_.isValid();
}

}