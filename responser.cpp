#include "responser.h"

responser::responser()
{
    not_found = "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>Not Found</title>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Not Found</h1>\n"
            "</body>\n"
            "</html>";
}

std::string responser::get_response(const std::string& request)
{
    std::ifstream fin;
    std::ofstream off("./log.log");
    off << "---------" << request << std::endl;
    off.close();
    //std::smatch match;
    //std::regex_search(request, match, std::regex("GET (.+) ")); JUST BECAUSE STEPIC'S G++ TOOOOOOOOOOO OOOOLD
    int b = request.find(" ");
    b++;
    int e = request.find(" ", b);
    std::string req_file;
    req_file = request.substr(b, e-b);
    //std::string req_file = match.str(1);
    int l = req_file.find('?');
    if (l != std::string::npos)
        req_file = req_file.substr(0, l);
    req_file = "." + req_file;
    fin.open(req_file, std::ios::in | std::ios::binary);
    if (fin.is_open())
    {
        std::string body;
        char c;
        while (fin.get(c))
            body.push_back(c);
        fin.close();
        return get_head(200, request, body.size()) + body;
    }
    else
        return get_head(404, request, not_found.length()); //+ not_found;
}

std::string responser::get_head(int code, const std::string& request, const unsigned long& content_length)
{
    std::stringstream head;

    //head << "HTTP/1.0 " << code;

    if (code == 200)
    {
        head << "HTTP/1.0 200 OK\r\n << Content-Length: " << content_length << "\r\n\r\n";
    }
    else if (code == 404)
    {
        head << "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n";
    }
    /*if (code == 200)
    {
        head << " OK\r\n";
        head << "Server: HttpServer_for_stepic_exam\r\n";
        head << "Content-Type: ";
        if (std::regex_search(request, std::regex("GET.+\\.html")))
            head << "text/html; charset=utf-8\r\n";
        else if (std::regex_search(request, std::regex("GET.+\\.jpg")))
            head << "image/jpeg\r\n";
        else if (std::regex_search(request, std::regex("GET.+\\.css")))
            head << "text/css";
    }
    else if (code == 404)
    {
        head << " Not Found\r\n";
        head << "Content-Type: text/html; charset=utf-8\r\n";
    }
    head << "Content-Length: " << content_length << "\r\n";
    if (std::regex_search(request, std::regex("keep-alive")))
        head << "Connection: keep-alive\r\n";
    else
        head << "Connection: close\r\n";
    head << "\r\n";*/
    return head.str();
}
