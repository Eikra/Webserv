#include "config.hpp"

int countChar(std::string str, char c)
{
    size_t i = 0;
    size_t  j = 0;
    while (i < str.size())
    {
        if (c == str[i])
            j++;
        i++;
    }
    return j;
}

bool my_strpbrk(const char* str, const char* charset) 
{
    while (*str != '\0') {
        const char* ch = charset;
        while (*ch != '\0') {
            if (*str == *ch) {
                return true;
            }
            ++ch;
        }
        ++str;
    }
    return false;
}

char hexToChar(const char * hex) 
{
    return (char)std::strtol(hex, NULL, 16);
}

std::string decodePercentEncoding(const std::string& resource) 
{
    std::string result;

    for (size_t i = 0; i < resource.length(); ++i) 
    {
        if (resource[i] == '%') 
        {
            if (i + 2 < resource.length() && isHexadecimal(resource[i + 1]) && isHexadecimal(resource[i + 2])) 
            {
                char hex[3] = {resource[i + 1], resource[i + 2], '\0'};
                result += hexToChar(hex);
                i += 2;
            }
            else
                result += resource[i];
        } 
        else 
            result += resource[i];
    }

    return result;
}

bool Client::isInvalidURI()
{//*#~
    resource = decodePercentEncoding(resource);
    // Check for unsafe characters
    const char* unsafeChars = "<>%#{}|\\^[]`\"*'&@;";//()$~+ ! * ' ( ) ; : @ & = + $ , / ? # [ ]
    if (my_strpbrk(resource.c_str(), unsafeChars) || countChar(resource, '?') > 1)
        return true;
    return false;
}

void    Client::parseFirstLine()
{
    int pos = 0;
    while (pos < (int)requestLine.size() && std::isspace(requestLine[pos]))
        ++pos;
    if (pos)
    {
        status = 400;
        return;
    }
    ///
    while (pos < (int)requestLine.size() && !std::isspace(requestLine[pos]))
        ++pos;
    int midle = pos;
    while (pos < (int)requestLine.size() && std::isspace(requestLine[pos]))  //method
        ++pos;
    if ((pos - midle) > 1 || (requestLine[midle]) != ' ')
    {
        status = 400;
        return;
    }
    ///
    while (pos < (int)requestLine.size() && !std::isspace(requestLine[pos]))
        ++pos;
    midle = pos;
    while (pos < (int)requestLine.size() && std::isspace(requestLine[pos]))  //resource
        ++pos;
    if ((pos - midle) > 1 || (requestLine[midle]) != ' ')
    {
        status = 400;
        return;
    }
    //
    while (pos < (int)requestLine.size() && !std::isspace(requestLine[pos])) //httpVersion
        ++pos;
    midle = pos;
    while (pos < (int)requestLine.size() && std::isspace(requestLine[pos]))  
        ++pos;
    if (((pos - midle) > 2) || ((midle + 1) < (int)requestLine.size() && (requestLine[midle]) != '\r' && (requestLine[midle + 1]) != '\n'))
    {
        status = 400;
        return;
    }
}

void    Client::checkEmptyChar()
{
    for (int i = 0; i < bytes; i++)
    {
        if ((buffer[i] < 32 && buffer[i] != 10 && buffer[i] != 13) || buffer[i] > 127)
        {
            status = 400;
            return;
        }
    }
}

bool isInvalidContLen(const std::string &size)
{    
    int i = 0;
    while (size[i])
    {
        if (!std::isdigit(size[i]))
            return 1;
        i++;
    }

    size_t result;

    std::istringstream iss(size);
    
    iss >> result;
    if (iss.fail())
        return 1;
    return 0;
}

static  std::string strtrim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start])) {
        ++start;
    }
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }
    return str.substr(start, end - start);
}

bool    Client::checkheader(std::string line)
{
    int pos = line.size() - 1;
    if (line.size() < 5 || std::isspace(line[0]) || line[pos] != '\r')
    {
        
        status = 400;
        return 0;
    }
    size_t i = 0;
    while (i < line.size() && !std::isspace(line[i]))
        i++;
    if (i < 2 && line[i - 1] != ':')
    {
       
        status = 400;
        return 0;
    }
    while (i < line.size() - 1)
    {
        size_t j = i;
        while (i < line.size() && std::isspace(line[i]))
        {
            i++;
        }
        if ((i - j) > 1)
        {
           
            status = 400;
            return 0;
        }
        i++;
    }
    return 1;
}

void    Client::setQuery(std::string qstr)
{
    querystr = qstr;
}

void    Client::lineParser()
{
    std::istringstream requestStream;
    requestStream.str(requestLine);
    std::string         line;
    if (method.size() == 0)//!isGetFirstLine && 
    {
        parseFirstLine();
        if (status != 0)
            return;
        std::getline(requestStream, line);
        std::istringstream lineStream(line);
        if (!(lineStream >> method) || !(lineStream >> resource) || !(lineStream >> httpVersion))
        {
            status = 400;
            return;
        }
        if (httpVersion != "HTTP/1.1" || httpVersion.size() != 8)
            {status = 505; return;}//HTTP Version Not Supported
        if (!((method == "POST" &&  method.size() == 4) || (method == "GET" && method.size() == 3)
        || (method == "DELETE" && method.size() == 6)))
        {
            status = 501;//501 Not Implemented
            return;
        }
        if(isInvalidURI())
        {
            status = 400;
            return;
        }
        if (resource.size() > 2048)
        {
            status = 414;
            return;
            //414 Request-URI Too Large
        }
        if (this->resource.find('?') != std::string::npos)
        {
            setQuery(this->resource.substr(this->resource.find('?') + 1));
            this->resource = this->resource.substr(0, this->resource.find('?'));
        }
        //return;
    }
    while (!status && !requestStream.eof()) 
    {
        std::getline(requestStream, line);
        if (line == "\r" && line.size() == 1)
        {
            isGetAllHeaders = 1;
            break;
        }
        if(!line.size())
            break;
        size_t separatorPos = line.find(':');
        if (separatorPos != std::string::npos && checkheader(line))
        {
            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);
            std::map<std::string, std::string>::const_iterator  host = headers.find("Host");
            std::string strTmp = strtrim(key);
            if (strTmp == "Host" && strTmp.size() == 4 && host != headers.end())
                {status = 400; return;}
            headers[strtrim(key)] = strtrim(value);
        }
        else
            {status = 400; return;}
    }

    std::map<std::string, std::string>::const_iterator  it;
    it = headers.find("Host");
    if (isGetAllHeaders && it == headers.end())
        {status = 400;return;}

    if (isGetAllHeaders && method == "POST")
    {
        it = headers.find("Transfer-Encoding");
        if (it != headers.end() && it->second != "chunked" && it->second.size() != 7)
            {status = 501;return;}//Not Implemented
        if (it != headers.end() && it->second == "chunked")
            isChunked = true;
        it = headers.find("Content-Length");
        if (it == headers.end() && !isChunked)
            {status = 411;return;}//length_required

        if (!isChunked && isInvalidContLen(it->second))
            {status = 400;return;}

        std::istringstream iss(it->second);
        iss >> contentLength;
        if (!isChunked && contentLength == 0)
            {status = 411;}
    }
}

void    Client::requestParser()
{
    int i = 0;
    while (i < bytes && !isGetAllHeaders)
    {
        if ((buffer[i] < 32 && buffer[i] != 10 && buffer[i] != 13) || buffer[i] > 127)
        {
            status = 400;
            return;
        }
        if (buffer[i] == '\n')
        {
            requestLine += buffer[i];
            lineParser();
            if (status != 0)
                return;
            requestLine.clear();
            i++;
            continue;
        }
        requestLine += buffer[i];
        i++;
    }
    headerSize += i;
    if (headerSize > HEADERSIZE)
        {status = 400; return;}
    if (isGetAllHeaders)
    {
        memset(bodyBuffer,0, sizeof(bodyBuffer));
        int j = 0;
        while (i < bytes)
        {
            bodyBuffer[j] = buffer[i];
            ++i;
            ++j;
        }
        bodyBuffer[j] = 0;
        bytes = j;

        std::map<std::string, std::string>::const_iterator it = headers.find("Transfer-Encoding");
        if ((isGetAllHeaders && it != headers.end() && it->second == "chunked"))
        {
            std::map<std::string, std::string>::const_iterator conType = headers.find("Content-Type");
            if(conType != headers.end() && conType->second.find("multipart/form-data; boundary=") == 0)
                status = 501;
        }
    }
}

