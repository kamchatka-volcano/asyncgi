#include <asyncgi/testserver.h>
#include <asyncgi/requestprocessor.h>
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include <asyncgi/timer.h>
#include "requestcontext.h"
#include <fcgi_responder/request.h>
#include <fcgi_responder/response.h>
#include <asio.hpp>
#include <fstream>
#include <iostream>

namespace asyncgi{

namespace {
const auto formBoundary = std::string{"----WebKitFormBoundaryTEST"};

std::string makeForm(const std::map<std::string, TestFormParam>& formParams)
{
    auto result = std::string{};
    for (const auto& paramPair : formParams) {
        auto& [paramName, formParam] = paramPair;
        result += "--" + formBoundary + "\r\n";
        auto header = http::Header{"Content-Disposition", "form-data"};
        header.setParam("name", paramName);
        if (formParam.value)
            result += header.toString() + "\r\n\r\n" + *formParam.value + "\r\n";
        else if (formParam.fileInfo){
            header.setParam("filename", formParam.fileInfo->filePath.filename());
            auto fileHeader = http::Header{"Content-Type", formParam.fileInfo->mimeType};

            auto fileStream = std::ifstream{formParam.fileInfo->filePath};
            if (!fileStream.is_open()) {
                std::cerr << "Can't open form param " + paramName + " file " + formParam.fileInfo->filePath.string() + " for reading";
                continue;
            }
            const auto paramFileContent = std::string{std::istreambuf_iterator<char>{fileStream},
                                                      std::istreambuf_iterator<char>{}};

            result += header.toString() + "\r\n" +
                      fileHeader.toString() + "\r\n\r\n" +
                      paramFileContent + "\r\n";
        }
    }
    result += "--" + formBoundary + "--\r\n";
    return result;
}
}

class Request;
class MsgStdIn;
class MsgParams;

TestServer::TestServer(RequestProcessor& requestProcessor)
    : requestProcessor_(requestProcessor)
{
}

std::string TestServer::process(const std::map<std::string, std::string>& fcgiParams,
                                const std::map<std::string, TestFormParam>& formParams)
{
    auto params = std::vector<std::pair<std::string, std::string>>{};
    std::copy(fcgiParams.begin(), fcgiParams.end(), std::back_inserter(params));
    if (!formParams.empty())
        params.emplace_back("CONTENT_TYPE", "multipart/form-data; boundary=" + formBoundary);
    auto form = makeForm(formParams);

    auto result = std::string{};
    auto fcgiRequest = fcgi::Request{params, form};
    auto fcgiResponse = fcgi::Response{
            [&result](std::string&& data, std::string&&){
                result = data;
            }};
    auto context = std::make_shared<RequestContext>(std::move(fcgiRequest), std::move(fcgiResponse));
    auto request = Request{context};
    auto ioContext = asio::io_context{};
    auto timer = Timer{ioContext};
    auto response = Response{context, timer};
    requestProcessor_.process(request, response);
    return result;
}

//std::tuple<std::string, std::string> readFileParam(std::string paramValue)
//{
//    paramValue = str::replace(paramValue, "file(", "");
//    auto paramFileData = str::split(paramValue, ")").at(0);
//    auto paramFileDataChunks = str::split(paramFileData, "|");
//    if (paramFileDataChunks.size() < 2)
//        return {};
//
//    auto paramFileName = str::trim(paramFileDataChunks.at(0));
//    auto paramFileMimeType = str::trim(paramFileDataChunks.at(1));
//    return std::make_tuple(paramFileName, paramFileMimeType);
//}

/*
    const auto formBoundary = std::string{"----WebKitFormBoundaryTEST"};
    if (!paramNames.isEmpty())
        fcgiParams_["CONTENT_TYPE"] = "multipart/form-data; formBoundary=" + formBoundary;

        auto result = std::string{};
    const auto formBoundary = std::string{"----WebKitFormBoundaryTEST"};
    for (const auto& paramPair : formParams) {
        auto& [paramName, paramValue] = paramPair;
        result += "--" + formBoundary + "\r\n";
        auto header = http::Header{"Content-Disposition", "form-data"};
        header.setParam("name", paramName);
        if (!str::startsWith(paramValue, "file("))
            result += header.toString() + "\r\n\r\n" + paramValue + "\r\n";
        else {
            auto [paramFileName, paramFileMimeType] = readFileParam(paramValue);
            if (!paramFileName.empty() && paramFileMimeType.empty())
                return {};

            header.setParam("filename", paramFileName);
            auto fileHeader = http::Header{"Content-Type", paramFileMimeType};
            auto filePath = QFileInfo(settings.fileName()).path() + "/" + QString::fromStdString(paramFileName);
            QFile paramFile{filePath};
            if (!paramFile.open(QIODevice::ReadOnly)) {
                errorMsg_ = "Can't open form file " + QString::fromStdString(paramFileName) + " for reading";
                return false;
            }
            const auto paramFileContent = paramFile.readAll();

            formData_ += header.toString() + "\r\n" +
                         fileHeader.toString() + "\r\n\r\n" +
                         paramFileContent.toStdString() + "\r\n";
        }
    }
    formData_ += "--" + formBoundary + "--\r\n";
    return true;
*/

}
