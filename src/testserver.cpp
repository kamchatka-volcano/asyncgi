//#include <asyncgi/testserver.h>
//#include <asyncgi/requestprocessor.h>
//#include <asyncgi/request.h>
//#include <asyncgi/response.h>
//#include <fcgi_responder/request.h>
//
//namespace asyncgi{
//
//class Request;
//class MsgStdIn;
//class MsgParams;
//
////class RequestMaker
////{
////public:
////    static Request makeRequest(const std::vector<std::pair<std::string, std::string>>& params,
////                               const std::string& formData = {})
////    {
////        auto request = Request{};
////        request.addParams(params);
////        request.stdIn_ = formData;
////        return request;
////    }
////};
////class ResponseMaker
////{
////public:
////    static Response makeResponse(const std::function<void(std::string&&, std::string&&)> sender)
////    {
////        return Response{sender};
////    }
////};
////}
//
//TestServer::TestServer(RequestProcessor& requestProcessor)
//    : requestProcessor_(requestProcessor)
//{
//}
//
//std::string TestServer::process(const std::map<std::string, std::string>& fcgiParams,
//                                const std::map<std::string, std::string>& formParams)
//{
//    auto params = std::vector<std::pair<std::string, std::string>>{};
//    for (const auto& paramPair : fcgiParams)
//        params.push_back(paramPair);
//    auto fcgiRequest = fcgi::RequestMaker::makeRequest(params, formData_);
//    auto fcgiResponse = fcgi::ResponseMaker::makeResponse(
//            [this](std::string&& data, std::string&&){
//                QFile resultFile{responseResultPath_};
//                if (!resultFile.open(QIODevice::WriteOnly)){
//                    errorMsg_ = "Can't open result file " + responseResultPath_ + "for writing";
//                    return;
//                }
//                resultFile.write(data.c_str(), static_cast<int>(data.size()));
//            });
//    requestProcessor_.processFcgiRequest(std::move(fcgiRequest), std::move(fcgiResponse));
//    return errorMsg_.isEmpty();
//}
//
//
//namespace{
//std::tuple<std::string, std::string> readFileParam(QString paramValue, bool& ok)
//{
//    paramValue.replace("file(", "");
//    auto paramFileData = paramValue.split(")", QString::SkipEmptyParts)[0];
//    auto paramFileDataChunks = paramFileData.split("|");
//    if (paramFileDataChunks.size() < 2){
//        ok = false;
//        return {};
//    }
//    auto paramFileName = paramFileDataChunks[0].trimmed();
//    auto paramFileMimeType = paramFileDataChunks[1].trimmed();
//    ok = true;
//    return std::make_tuple(paramFileName.toStdString(), paramFileMimeType.toStdString());
//}
//}
//
//bool QCgiTestServer::readRequestConfig(QString configPath)
//{
//    QSettings settings{configPath, QSettings::IniFormat};
//    settings.setIniCodec("UTF-8");
//
//    return readFcgiParamsConfig(settings) &&
//           readFormConfig(settings);
//}
//
//bool QCgiTestServer::readFcgiParamsConfig(QSettings& settings)
//{
//    settings.beginGroup("fcgi_params");
//    {
//        auto paramNames = settings.allKeys();
//        if (paramNames.isEmpty()){
//            errorMsg_ = "Settings fcgi params is empty";
//            return false;
//        }
//
//        for (const auto& paramName : paramNames)
//            fcgiParams_[paramName.toStdString()] = settings.value(paramName).toString().toStdString();
//    }
//    settings.endGroup();
//    return true;
//}
//
//bool QCgiTestServer::readFormConfig(QSettings& settings)
//{
//    settings.beginGroup("form");
//    {
//        auto paramNames = settings.allKeys();
//        const auto boundary = std::string{"----WebKitFormBoundaryTEST"};
//        if (!paramNames.isEmpty())
//            fcgiParams_["CONTENT_TYPE"] = "multipart/form-data; boundary=" + boundary;
//        for (const auto& paramName : paramNames){
//            auto paramValue = settings.value(paramName).toString();
//            formData_ += "--" + boundary + "\r\n";
//            auto header = fcgiapp::Header{"Content-Disposition", "form-data"};
//            header.setParam("name", paramName.toStdString());
//            if (!paramValue.startsWith("file("))
//                formData_ += header.toString() + "\r\n\r\n" + paramValue.toStdString() + "\r\n";
//            else{
//                auto paramFileName = std::string{};
//                auto paramFileMimeType = std::string{};
//                auto ok = false;
//                std::tie(paramFileName, paramFileMimeType) = readFileParam(paramValue, ok);
//                if (!ok){
//                    errorMsg_ = "form file parameter is invalid";
//                    return false;
//                }
//                header.setParam("filename", paramFileName);
//                auto fileHeader = fcgiapp::Header{"Content-Type", paramFileMimeType};
//                auto filePath = QFileInfo(settings.fileName()).path() + "/" + QString::fromStdString(paramFileName);
//                QFile paramFile{filePath};
//                if (!paramFile.open(QIODevice::ReadOnly)){
//                    errorMsg_ = "Can't open form file " + QString::fromStdString(paramFileName) + " for reading";
//                    return false;
//                }
//                const auto paramFileContent = paramFile.readAll();
//
//                formData_ += header.toString() + "\r\n" +
//                             fileHeader.toString() + "\r\n\r\n" +
//                             paramFileContent.toStdString() + "\r\n";
//            }
//        }
//        formData_ += "--" + boundary + "--\r\n";
//    }
//    settings.endGroup();
//    return true;
//}
//
//
//QString QCgiTestServer::errorMsg() const
//{
//    return errorMsg_;
//}
//
//}
//
