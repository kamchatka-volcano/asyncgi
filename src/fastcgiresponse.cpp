#include <asyncgi/fastcgi/response.h>

namespace asyncgi::fastcgi {
Response::Response(detail::ResponseView responseView)
    : data_{responseView}
{
}

Response::Response(std::string data, std::string errorMsg)
    : data_{detail::ResponseData{std::move(data), std::move(errorMsg)}}
{
}

std::string_view Response::data() const
{
    return std::visit(
            [](const auto& data) -> std::string_view
            {
                return data.data;
            },
            data_);
}

std::string_view Response::errorMsg() const
{
    return std::visit(
            [](const auto& data) -> std::string_view
            {
                return data.errorMsg;
            },
            data_);
}

bool Response::isView() const
{
    return std::holds_alternative<detail::ResponseView>(data_);
}

namespace detail {
ResponseData moveOutResponseData(Response& response)
{
    if (response.isView())
        return {std::string{response.data()}, std::string{response.errorMsg()}};
    return {std::move(std::get<detail::ResponseData>(response.data_).data),
            std::move(std::get<detail::ResponseData>(response.data_).errorMsg)};
}

} //namespace detail

} //namespace asyncgi::fastcgi