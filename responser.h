#ifndef HTTPSERVER_RESPONSER_H
#define HTTPSERVER_RESPONSER_H


#include <string>
#include <regex>
#include <fstream>
#include <sstream>

class responser final
{
private:
    std::string not_found;
public:
    responser();
    std::string get_response(const std::string& request);
    std::string get_head(int code, const std::string& request, const unsigned long& content_length);
};


#endif //HTTPSERVER_RESPONSER_H
