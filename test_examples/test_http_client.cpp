#include <asyncgi/asyncgi.h>
#include <asyncgi/http_client.h>
#include <iostream>

http::Request makeRequest(std::string_view command)
{
    if (command == "post")
        return http::Request{
                http::RequestMethod::Post,
                "http://localhost:8091",
                http::RequestBody(http::ContentType::PlainText, "Hello world")};

    if (command == "httpbun")
        return http::Request{http::RequestMethod::Get, "https://httpbun.com/get"};

    return http::Request{
            http::RequestMethod::Get,
            "http://localhost:8091",
            http::RequestCookies{{"foo", "bar"}, {"foo", "baz"}}};
}

int main(int argc, char* argv[])
{
    auto command = argc > 1 ? std::string_view{argv[1]} : std::string_view{};
    auto io = asyncgi::IO{};
    auto client = asyncgi::HttpClient{io};
    client.makeRequest(
            makeRequest(command),
            [&io, &client](std::optional<http::Response> reqResponse)
            {
                if (reqResponse.has_value()) {
                    std::cout << reqResponse->body() << std::endl;
                    for (const auto& cookie : reqResponse->cookies())
                        std::cout << "Cookie: " << cookie.name() << "=" << cookie.value() << std::endl;

                    for (const auto& header : reqResponse->headers())
                        std::cout << "Header: " << header.name() << "=" << header.value() << std::endl;
                }
                else
                    std::cout << "No response. Error: " << client.requestErrorMessage() << std::endl;

                io.stop();
            },
            std::chrono::seconds(5));
    io.run();
    return 0;
}
