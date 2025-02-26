//
// Created by abdel_or7j875 on 2/25/2025.
//
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <regex>

class HttpResponse {
public:
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseResponse(const std::string& rawResponse) {
        std::stringstream ss(rawResponse);
        std::string line;

        // Parse status line
        std::getline(ss, line);
        std::regex statusLineRegex(R"(HTTP/\d\.\d\s(\d+)\s(.*))");
        std::smatch match;

        if (std::regex_match(line, match, statusLineRegex)) {
            statusCode = std::stoi(match[1].str());
            statusMessage = match[2].str();
        } else {
            std::cerr << "Invalid status line: " << line << std::endl;
            return;
        }

        // Parse headers
        while (std::getline(ss, line) && !line.empty() && line != "\r") { // Check for empty line
            std::regex headerRegex(R"(([^:]+):\s(.*))");
            if (std::regex_match(line, match, headerRegex)) {
                headers[match[1].str()] = match[2].str();
            }
        }

        // Parse body
        std::stringstream bodyStream;
        while (std::getline(ss, line)) {
            bodyStream << line << "\n";
        }
        body = bodyStream.str();
        //remove the last newline char.
        if(!body.empty()){
          body.pop_back();
        }

    }

    void printResponse() const {
        std::cout << "Status Code: " << statusCode << std::endl;
        std::cout << "Status Message: " << statusMessage << std::endl;
        std::cout << "Headers:" << std::endl;
        for (const auto& header : headers) {
            std::cout << "  " << header.first << ": " << header.second << std::endl;
        }
        std::cout << "Body:\n" << body << std::endl;
    }
};

int main() {
    std::string rawResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 36\r\n"
        "Date: Tue, 04 Jul 2023 12:00:00 GMT\r\n"
        "\r\n"
        "{\"message\": \"Hello, World!\", \"status\": \"ok\"}";

    HttpResponse response;
    response.parseResponse(rawResponse);
    response.printResponse();
    std::string rawResponse2 =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "Date: Tue, 04 Jul 2023 12:00:00 GMT\r\n"
        "\r\n"
        "Page Not Found";
    HttpResponse response2;
    response2.parseResponse(rawResponse2);
    response2.printResponse();

    return 0;
}
