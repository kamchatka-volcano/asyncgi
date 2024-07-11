#include <asyncgi/fastcgi/request.h>
#include <fcgi_responder/request.h>
#include <algorithm>
#include <iterator>

namespace asyncgi::fastcgi {

namespace detail {

RequestView::RequestView(const asyncgi::detail::fcgi::Request& fcgiRequest)
    : fcgiRequest_{fcgiRequest}
{
}

std::string_view RequestView::stdIn() const
{
    return fcgiRequest_.get().stdIn();
}

std::string_view RequestView::param(const std::string& name) const
{
    return fcgiRequest_.get().param(name);
}

const std::vector<std::pair<std::string, std::string>>& RequestView::params() const
{
    return fcgiRequest_.get().params();
}

bool RequestView::hasParam(const std::string& name) const
{
    return fcgiRequest_.get().hasParam(name);
}

RequestData::RequestData(std::map<std::string, std::string> params, std::string stdIn)
    : paramData_{
              std::vector<std::pair<std::string, std::string>>{
                      std::make_move_iterator(params.begin()),
                      std::make_move_iterator(params.end())},
              std::move(stdIn)}
{
}

std::string_view RequestData::stdIn() const
{
    return paramData_.stdIn;
}

namespace {
struct ParamLookupComparator {
    bool operator()(const std::string& key, const std::pair<std::string, std::string>& val) const
    {
        return key < val.first;
    }
    bool operator()(const std::pair<std::string, std::string>& val, const std::string& key) const
    {
        return val.first < key;
    }
};
} //namespace

std::string_view RequestData::param(const std::string& name) const
{
    auto itRange = std::equal_range(paramData_.params.begin(), paramData_.params.end(), name, ParamLookupComparator{});
    if (!std::distance(itRange.first, itRange.second))
        return {};
    return itRange.first->second;
}

bool RequestData::hasParam(const std::string& name) const
{
    return std::binary_search(paramData_.params.begin(), paramData_.params.end(), name, ParamLookupComparator{});
}

const std::vector<std::pair<std::string, std::string>>& RequestData::params() const
{
    return paramData_.params;
}

RequestDataParams moveOutRequestData(Request& request)
{
    if (request.isView())
        return {request.params(), std::string{request.stdIn()}};

    return {std::move(std::get<detail::RequestData>(request.data_).paramData_.params),
            std::move(std::get<detail::RequestData>(request.data_).paramData_.stdIn)};
}

} //namespace detail

Request::Request(detail::RequestView requestView)
    : data_{requestView}
{
}

Request::Request(std::map<std::string, std::string> params, std::string stdIn)
    : data_{detail::RequestData{std::move(params), std::move(stdIn)}}
{
}

std::string_view Request::stdIn() const
{
    return std::visit(
            [](const auto& data)
            {
                return data.stdIn();
            },
            data_);
}

std::string_view Request::param(const std::string& name) const
{
    return std::visit(
            [&](const auto& data)
            {
                return data.param(name);
            },
            data_);
}

bool Request::hasParam(const std::string& name) const
{
    return std::visit(
            [&](const auto& data)
            {
                return data.hasParam(name);
            },
            data_);
}

const std::vector<std::pair<std::string, std::string>>& Request::params() const
{
    return std::visit(
            [](const auto& data) -> const std::vector<std::pair<std::string, std::string>>&
            {
                return data.params();
            },
            data_);
}
bool Request::isView() const
{
    return std::holds_alternative<detail::RequestView>(data_);
}

} //namespace asyncgi::fastcgi