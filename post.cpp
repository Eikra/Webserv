#include "config.hpp"

void            Client::methodPost()
{
    if (status != 0)
        return;
    if (bestMatchLocation.auto_upload == "on")
    {
        if (createFile())
        {
            if (isChunked)
            {
                parseChunked();
                if (isLastChunk)
                {
                    if (bestMatchLocation.cgi == "on" && chekcgi == 0) 
                    {
                        inputfile.close();
                        std::string fileExtension = getFileExtension(getfilePath());
                        if (fileExtension == ".py" || fileExtension == ".php")
                        {
                            handle_cgi(getfilePath(),fileNamePost, fileExtension);
                        }
                        else
                        {
                            std::map<std::string, std::string>::const_iterator conType = headers.find("Content-Type");
                            if(conType != headers.end() && conType->second.find("multipart/form-data; boundary=") == 0)
                            {
                                status = 501;
                                isGetFullBody = 1;
                            }
                            else
                            {
                                status = 201;
                                isGetFullBody = 1;
                                inputfile.close();
                            }
                        }
                    }
                    else
                    {
                        std::map<std::string, std::string>::const_iterator conType = headers.find("Content-Type");
                        if(conType != headers.end() && conType->second.find("multipart/form-data; boundary=") == 0)
                        {
                            status = 501;
                            isGetFullBody = 1;
                        }
                        else
                        {
                            status = 201;
                            isGetFullBody = 1;
                            inputfile.close();
                        }
                    }
                }
            }
            else
            {
                inputfile.write(bodyBuffer, bytes);
                totalReaded += bytes;
                if(contentLength <= totalReaded)
                {
                    if (bestMatchLocation.cgi == "on" && chekcgi == 0) 
                    {
                        inputfile.close();
                        std::string fileExtension = getFileExtension(getfilePath());
                        if (fileExtension == ".py" || fileExtension == ".php")
                        {
                            handle_cgi(getfilePath(),fileNamePost, fileExtension);
                            //isGetFullBody = 1;
                        }
                        else
                        {
                            std::map<std::string, std::string>::const_iterator conType = headers.find("Content-Type");
                            if(conType != headers.end() && conType->second.find("multipart/form-data; boundary=") == 0)
                            {
                                status = 501;
                                isGetFullBody = 1;
                                inputfile.close();
                                remove(fileNamePost.c_str());
                            }
                            else
                            {
                                status = 201;
                                isGetFullBody = 1;
                                inputfile.close();
                            }
                        }
                    }
                    else
                    {
                        std::map<std::string, std::string>::const_iterator conType = headers.find("Content-Type");
                        if(conType != headers.end() && conType->second.find("multipart/form-data; boundary=") == 0)
                        {
                            status = 501;
                            isGetFullBody = 1;
                            inputfile.close();
                            remove(fileNamePost.c_str());
                        }
                        else
                        {
                            status = 201;
                            isGetFullBody = 1;
                            inputfile.close();
                        }
                    }
                }
            }
        }
    }
    else
        {status = 403;} // forbidden
}

std::string generateUniqueId() {
    std::ostringstream oss;
    oss << std::time(0);
    return oss.str();
}

std::string     generateNewFileName(int i) {
    return (generateUniqueId() + my_to_string(rand()) + my_to_string(i));
}

bool    Client::createFile()
{
    if (isFileCreated)
        return true;
    std::string fileExtension;
    if (headers.find("Content-Type") != headers.end())
        fileExtension = getExtensionByContentType(headers.find("Content-Type")->second);
    else
        fileExtension = "";
    fileNamePost = bestMatchLocation.upload_path + generateNewFileName(clientSocket) + fileExtension;
    inputfile.open(fileNamePost, std::ios::binary);     
    if (!inputfile.is_open())
    {
        status = 501;
        return false;
    }
    isFileCreated = true;
    return true;
}

bool    isHexadecimal(char c)
{
    char str[] = "0123456789abcdefABCDEF";
    int i = 0;
    while (str[i])
    {
        if (c == str[i])
        {
            return true;
        }
        ++i;
    }
    return false;
}

int hexToDecimal(const std::string& hexString) 
{
    int decimalValue = 0;

    if (hexString.empty()) {
        return 0;
    }
    size_t start = 0;
    if (hexString.size() >= 2 && hexString.substr(0, 2) == "0x") 
    {
        start = 2;
    }
    for (size_t i = start; i < hexString.size(); ++i) 
    {
        decimalValue *= 16;  
        if (hexString[i] >= '0' && hexString[i] <= '9') 
        {
            decimalValue += hexString[i] - '0';
        } 
        else if (hexString[i] >= 'A' && hexString[i] <= 'F') 
        {
            decimalValue += hexString[i] - 'A' + 10;
        } 
        else if (hexString[i] >= 'a' && hexString[i] <= 'f') 
        {
            decimalValue += hexString[i] - 'a' + 10;
        } 
        else 
        {
            return 0;
        }
    }
    return decimalValue;
}

void Client::parseChunked()
{
    int i = 0;
    while (i < bytes && !isLastChunk)
    {
        if (!chunkSize && !isFirstLine)
        {
            while (i < bytes && isHexadecimal(bodyBuffer[i]))
            {
                requestLine += bodyBuffer[i];
                i++;
            }
            if (i < bytes && requestLine.size() && bodyBuffer[i] == '\r' && condition == 2)
            {
                chunkSize = hexToDecimal(requestLine);
                isFirstLine = true;
                requestLine.clear();
                if (chunkSize == 0)
                {
                    isLastChunk = true;
                    status = 411;
                    isGetFullBody = true;
                    inputfile.close();
                    remove(fileNamePost.c_str());
                    return;
                }
                i++;
                condition++;
            }
            else if (i < bytes && condition == 2)
            {
                status = 400;
                isGetFullBody = true;
                inputfile.close();
                remove(fileNamePost.c_str());
                return;
            }
            if (i < bytes && bodyBuffer[i] == '\n' && condition == 3)
            {
                condition = 5;
                i++;
            }
            else if (i < bytes && condition == 3)
            {
                status = 400;
                isGetFullBody = true;
                inputfile.close();
                remove(fileNamePost.c_str());
                return;
            }
        }
        else if (condition <= 4 && isFirstLine)
        {
            if (i < bytes && bodyBuffer[i] == '\r' && condition == 1)
            {
                i++;
                condition++;
            }
            else if (i < bytes && condition == 1)
            {
                status = 400;
                isGetFullBody = true;
                inputfile.close();
                remove(fileNamePost.c_str());
                return;         
            }
            if (i < bytes && bodyBuffer[i] == '\n' && condition == 2)
            {
                i++;
                condition++;
            }
            else if (i < bytes && condition == 2)
            {
                status = 400;
                isGetFullBody = true;
                inputfile.close();
                remove(fileNamePost.c_str());
                return; 
            }
            while (i < bytes && isHexadecimal(bodyBuffer[i]))
            {
                requestLine += bodyBuffer[i];
                i++;
            }
            if (i < bytes && requestLine.size() && bodyBuffer[i] == '\r'  && condition == 3)
            {
                chunkSize = hexToDecimal(requestLine);
                if (chunkSize == 0)
                {
                    isLastChunk = true;
                    isGetFullBody = true;
                    return;
                }
                requestLine.clear();
                i++;
                condition++;
            }
            else if (i < bytes && condition == 3)
            {
                status = 400;
                isGetFullBody = true;
                inputfile.close();
                remove(fileNamePost.c_str());
                return;
            }
            if (i < bytes && bodyBuffer[i] == '\n' && condition == 4)
            {
                i++;
                condition++;
            }
            else if (i < bytes && condition == 4)
            {
                status = 400;
                isGetFullBody = true;
                inputfile.close();
                remove(fileNamePost.c_str());
                return;
            }
        }
        while (chunkSize && i < bytes && condition > 4)
        {
            if (chunkSize > (bytes - i))
            {
                inputfile.write(bodyBuffer + i, bytes - i);
                totalReaded += (bytes - i);
                chunkSize -= (bytes - i);
                i += (bytes - i);
            }
            else if (chunkSize <= (bytes - i))
            {
                inputfile.write(bodyBuffer + i, chunkSize);
                totalReaded += chunkSize;
                i += chunkSize;
                chunkSize = 0;
                condition = 1;
                if (totalReaded > mx_cl_bd_size)
                {
                    status = 413;
                    isGetFullBody = true;
                    inputfile.close();
                    remove(fileNamePost.c_str());
                    return;
                }
            }
        }
    }
}


