#ifndef HTTPSERVER_RESPONSER_H
#define HTTPSERVER_RESPONSER_H


#include <string>
#include <regex>
#include <fstream>
#include <sstream>

/**
 * @class Responser
 *
 * @brief generate response content
 *
 * It is a class which generate response content
 * Now it is HTTP content, but it might be replaced
 */
class Responser final
{
private:
    /**
     * @brief not found page
     */
    std::string not_found;

    /**
     * @param code is HTTP status code
     * @param request is a GET string
     * @param content_length is a size of incoming message
     * @return HTTP header which meets the passed parameters
     */
    std::string get_head(int code, const std::string& request, const unsigned long& content_length);

public:

    Responser();

    /**
     * @param request incoming message
     * @return content which depends on @ref request
     */
    std::string get_response(const std::string& request);
};


#endif //HTTPSERVER_RESPONSER_H
