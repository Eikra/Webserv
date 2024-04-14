#include "config.hpp"

void Client::setresponse(const std::string stat, const std::string &contentType, const std::string &content)
{

    response = stat + "\r\n"
                      "Content-Type: " +
               contentType + "\r\n"
                             "Content-Length: " +
              my_to_string(content.size()) + "\r\n"
                                                "\r\n" +
               content;
}

const char *Client::getresponse()
{
    return response.c_str();
}

std::string Client::getfilePath()
{
    size_t pos = resource.find(bestmatch);
    if (pos != std::string::npos)
    {
        return filePath = bestMatchLocation.root + '/' + resource.substr(pos + bestmatch.length());
    }
    return "error";
}

bool Client::sendHredes()
{
    if (isSentHreders)
        return 1;
    isSentHreders = true;
    fileStream.open(getfilePath(), std::ios::binary);
    fileStream.seekg(0, std::ios::end);
    file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string filecontent;
    if (fileStream.is_open())
    {
        filecontent = "HTTP/1.1 200 OK\r\n";
        filecontent += "Content-Type: ";
        filecontent += getContentTypeByExtension(getFileExtension(filePath));
        filecontent += "\r\n";
        filecontent += "Content-Length: ";
        filecontent += my_to_string(file_size);
        filecontent += "\r\n\r\n";
        size_t bytesend = send(clientSocket, filecontent.c_str(), filecontent.size(), 0);
        if (bytesend <= 0)
        {
            fileStream.close();
            status = -1;
            return 0;
        }
        fileStream.close();
        fileDescriptor = open(getfilePath().c_str(), O_RDONLY | O_NONBLOCK, 077);
    }
    else
    {
        fileStream.close();
        status = 500;
        return 0;
    }
    return 0;
}

void Client::sendResponse(server &serv)
{

    if (serverRedirect.size() && (status == 301 || status == 302)) // check redirection for the server
    {
        if (status == 301)
        {
            htmlContent = "HTTP/1.1 301 Moved Permanently\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: 0\r\n"
                          "Location: " +
                          serv.serverRedirect[1] + "\r\n"
                                                   "\r\n";
            size_t bytsend = send(clientSocket, htmlContent.c_str(), htmlContent.size(), 0);
            if (bytsend <= 0 || bytsend == htmlContent.size())
                status = -1;
        }
        else if (status == 302)
        {
            htmlContent = "HTTP/1.1 302 Found\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: 0\r\n"
                          "Location: " +
                          serv.serverRedirect[1] + "\r\n"
                                                   "\r\n";
            int bytsend = send(clientSocket, htmlContent.c_str(), htmlContent.size(), 0);
            if (bytsend <= 0 || htmlContent.size())
                status = -1;
        }
        return;
    }
    if (bestMatchLocation.locationRedirect.size() && (status == 301 || status == 302)) // check redirection for the location
    {
        if (method == "POST" && bytes >= 1024)
            return;
        checkLocationRedirection();
        return;
    }
    hostcopy = serv.host;
    portkeycopy = serv.port;

    if (status == 201)
    {
        setresponse("HTTP/1.1 201 Created", "text/html",
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n"
                    "<h1>Created</h1>\r\n<p>The request has been fulfilled, resulting in the creation of a new resource.</p>\r\n"
                    "</body></html>\r\n");
        size_t bytsend = send(clientSocket, getresponse(), strlen(getresponse()), 0);
        if (bytsend <= 0  || bytsend ==strlen(getresponse()) )
            status = -1;
    }
    if (status == 408)
    {
        responceerr("408 ", "Request Timeout");
    }
    if (status == 400)
    {
        responceerr("400 ", "Bad Request");
    }
    if (status == 501)
    {
        if (method == "HEAD")
        {
            char c[] = "HTTP/1.1 501 Not Implemented\r\n"
                       "Allow: GET, POST , DELETE\r\n" // List of allowed methods
                       "Content-Length: 0\r\n"         // No content in the response body
                       "\r\n";
            size_t bytsend = send(clientSocket, c, strlen(c), 0);
            if (bytsend <= 0 || bytsend == strlen(c))
                status = -1;
        }
        else
            responceerr("501 ", "Not Implemented");
    }
    if(status == 411)
    {
         responceerr("411 ", "Length Required");
    }
    if(status == 504)
    {
        responceerr("504 ", "Gateway Timeout");
    }
    if (status == 500)
    {
        responceerr("500 ", "Internal Server Error");
    }
    if (status == 404)
    {
        responceerr("404 ", "Not Found");
    }
    if (status == 403)
    {
        responceerr("403 ", "Forbidden");
    }
    if (status == 413)
        responceerr("413 ", "Content Too Large");
    if (status == 505)
        responceerr("505 ", "HTTP Version Not Supported");
    if (status == 405)
    {
        responceerr("405 ", "Method Not Allowed");
    }
    if (status == 409)
    {
        responceerr("409  ", "Conflict");
    }
    else if (status == 0 && method == "GET")
    {
        methodGet();
    }
    else if (status == 0 && method == "DELETE")
    {
        checkpermission(getfilePath());
        if (status == 0)
            Delete_method(getfilePath());
    }
    else if (status == 0 && method == "POST")
    {
        if (bestMatchLocation.auto_upload == "on" && isChunked && isLastChunk && chekcgi == 0)
            methodPost();
        else if (chekcgi == 1)
        {
            std::string fileExtension = getFileExtension(getfilePath());
            handle_cgi(getfilePath(), fileNamePost, fileExtension);
        }
    }
}

void Client::checkLocationRedirection()
{
    if (bestMatchLocation.locationRedirect.size())
    {
        if (atoi(bestMatchLocation.locationRedirect[0].c_str()) == 301)
        {
            htmlContent = "HTTP/1.1 301 Moved Permanently\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: 0\r\n"
                          "Location: " +
                          bestMatchLocation.locationRedirect[1] + "\r\n"
                                                                  "\r\n";
            size_t bytsend = send(clientSocket, htmlContent.c_str(), htmlContent.size(), 0);
            if (bytsend <= 0 || bytsend == htmlContent.size())
            {
                status = -1;
                return;
            }
        }
        else if (atoi(bestMatchLocation.locationRedirect[0].c_str()) == 302)
        {
            htmlContent = "HTTP/1.1 302 Found\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: 0\r\n"
                          "Location: " +
                          bestMatchLocation.locationRedirect[1] + "\r\n"
                                                                  "\r\n";
            size_t bytsend = send(clientSocket, htmlContent.c_str(), htmlContent.size(), 0);
            if (bytsend <= 0 || bytsend == htmlContent.size())
            {
                status = -1;
            }
        }
    }
}

void Client::responceerr(std::string statuse, std::string name)
{
    std::string total = "HTTP/1.1 " + statuse + name;
    handleErrorResponse(statuse, name);
    if (status != -1 && flag == 1)
    {
        setresponse(total, "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>" + statuse + name + "</title>\r\n</head><body>\r\n<h1>" + statuse + name + "</h1>\r\n<p> Oops! try again.</p>\r\n</body></html>\r\n");
        size_t bytesend = send(clientSocket, getresponse(), strlen(getresponse()), 0);
        if (bytesend <= 0 || bytesend == strlen(getresponse()))
        {
            status = -1;
            return;
        }
    }
}
bool Client::sendheadererror(std::string errorCode, std::string title, std::string filerr)
{
    if (isSentHreders == 1)
        return 1;

    fileStream.open(filerr.c_str(), std::ios::binary);
    fileStream.seekg(0, std::ios::end);
    file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string filecontent;
    if (fileStream.is_open())
    {
        std::string contentType = getContentTypeByExtension(getFileExtension(filerr.c_str()));
        if (contentType != "text/html")
        {
            fileStream.close();
            flag = 1;
            return 0;
        }
        filecontent = "HTTP/1.1 " + errorCode + title;
        filecontent += "\r\n";
        filecontent += "Content-Type: " + contentType + "\r\n";
        filecontent += "Content-Length: " + my_to_string(file_size) + "\r\n\r\n";
        int bytesend = send(clientSocket, filecontent.c_str(), filecontent.size(), 0);
        if (bytesend <= 0)
        {
            fileStream.close();
            status = -1;
            flag = 1;
        }
        fileStream.close();
        fileDescriptor = open(filerr.c_str(), O_RDONLY | O_NONBLOCK);
        if (fileDescriptor < 0)
        {
            flag = 1;
        }
    }
    else
    {
        fileStream.close();
        flag = 1;
    }
    isSentHreders = 1;
    return 0;
}
void Client::handleErrorResponse(std::string errorCode, std::string title)
{
    int i = 0;
    std::map<std::string, std::string>::iterator page;
    for (page = err_pagecopy.begin(); page != err_pagecopy.end(); ++page)
    {
        if (page->first + " " == errorCode)
        {
            i = 1;
            break;
        }
    }
    if (i == 1)
    {
        int buffersize = 1024;
        char buffer[buffersize];
        memset(buffer, 0, buffersize);
        size_t sendbyt = 0;
        if (access(page->second.c_str(), F_OK) == -1)
            flag = 1;
        if (sendheadererror(errorCode, title, page->second))
        {
            int bete = read(fileDescriptor, buffer, buffersize - 1);
            if (bete < 0)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
                flag = 1;
            }
            if (bete == 0)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
            }
            buffer[bete] = '\0';
            sendbyt = send(clientSocket, buffer, bete, 0);
            if (sendbyt <= 0)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
                flag = 1;
            }
            totalbytes += sendbyt;
            if (totalbytes == file_size)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
            }
        }
    }
    if (i == 0)
    {
        flag = 1;
    }
}

void Client::matchingLocation(server &serv)
{
    if (method != "POST")
        isGetFullBody = true;
    err_pagecopy = serv.err_pages;
    if (status != 0)
        return;
    if (serv.serverRedirect.size())
    {
        serverRedirect = serv.serverRedirect;
        status = atoi(serv.serverRedirect[0].c_str());
        return;
    }
    mx_cl_bd_size = serv.mx_cl_bd_size;
    if (contentLength > serv.mx_cl_bd_size)
    {
        status = 413;
        return;
    }

    int i = 0;
    std::string tmp;
    std::string tmresource;

    for (std::deque<location>::iterator loc = serv.locations.begin(); loc != serv.locations.end(); ++loc)
    {
        if ((*loc).path_location.back() != '/')
        {
            tmp = (*loc).path_location + '/';
        }
        else
            tmp = (*loc).path_location;
        if (resource.back() != '/')
            tmresource = resource + '/';
        else
            tmresource = resource;
        if (tmresource.find(tmp) == 0)
        {
            if ((*loc).path_location.length() > bestMatchLength)
            {
                i++;
                bestmatch = (*loc).path_location;

                bestMatchLocation = *loc;
                bestMatchLength = (*loc).path_location.length();
            }
        }
    }
    if (i == 0)
    {
        status = 404;
        return;
    }
    if (i && bestMatchLocation.locationRedirect.size())
    {
        status = atoi(bestMatchLocation.locationRedirect[0].c_str());
        return;
    }
    if (i != 0 && bestMatchLocation.root.empty())
    {
        bestMatchLocation.root = (serv.root);
    }
    if (i != 0)
    {
        int find = 0;
        for (std::deque<std::string>::iterator met = bestMatchLocation.allow_methods.begin(); met != bestMatchLocation.allow_methods.end(); ++met)
        {
            if ((*met) == method)
            {
                find = 1;
                break;
            }
        }
        if (find != 1)
        {
            status = 405;
            return;
        }
        char resolved_path[PATH_MAX];

        if (realpath(getfilePath().c_str(), resolved_path) != NULL)
        {
            std::string validpath = std::string(resolved_path) + "/";
            std::string t;
            if(bestMatchLocation.root.back() != '/')
                    t = bestMatchLocation.root + '/';
            size_t n = validpath.find(t); // add root path
            if (n == std::string::npos)
            {
                status = 403;
                return;
            }
        }
        else
        {
            status = 404;
            return;
        }
    }
}