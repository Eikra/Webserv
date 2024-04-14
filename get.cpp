#include "config.hpp"

void Client::methodGet()
{
    struct stat filinfo ;
    if(stat(getfilePath().c_str(),&filinfo) == 0 )
    {
        if(access(getfilePath().c_str(),F_OK) !=-1)
        {
            if (!((filinfo.st_mode & S_IRUSR) ||
            (filinfo.st_mode & S_IWUSR) ||
            (filinfo.st_mode & S_IXUSR)))
            {
                status = 403;
                return;
            }     
            else if(S_ISREG(filinfo.st_mode))
            {
                if((getFileExtension(filePath) == ".py" || getFileExtension(filePath) == ".php") && bestMatchLocation.cgi == "on")
                    handle_cgi(filePath,"", getFileExtension(filePath));
                else
                    methodGetFile();
            }
            else if(S_ISDIR(filinfo.st_mode))
            {
                 handleDirectoryAutoIndex();
            }
                
        }
        else if(status == 0)
        {
            status = 404;
            return;
        }
    }
    else if(status == 0)
    {
       status = 404; 
    }         
}

void Client::methodGetFile()
{
    int buffersize = 1024;
    char buffer[buffersize];
    memset(buffer,0,buffersize);
    if(sendHredes())
    {
        int bete = read(fileDescriptor,buffer,buffersize-1);
        if(bete <= 0)
        {  
            if (fileDescriptor != -1)
            {close(fileDescriptor);fileDescriptor = -1;}
            status = -1;
            return;
        }
        buffer[bete] = '\0';
        size_t sendbyte = send(clientSocket,buffer,bete,0);
        if(sendbyte <= 0 )
        {  
            if (fileDescriptor != -1)
            {close(fileDescriptor);fileDescriptor = -1;}
            status = -1;
            return ;
        }
        totalbytes += sendbyte;
        if(totalbytes == file_size)
        {
            if (fileDescriptor != -1)
            {close(fileDescriptor);fileDescriptor = -1;}
            status = -1;
            return ;
        }
    }
}

void Client::movedpermanent()
{
    htmlContent = "HTTP/1.1 301 Moved Permanently\r\n"
                  "Content-Type: text/html\r\n"
                  "Location: " + resource + "/" + "\r\n"
                  "\r\n";
        size_t bytsend = send(clientSocket, htmlContent.c_str(), htmlContent.size(), 0);
        if (bytsend <= 0 || bytsend == htmlContent.size() )
        {
            status = -1;
        }    
}

std::string Client::getFileExtension(std::string filePath) 
{
    size_t dotPosition = filePath.find_last_of('.');
    if (dotPosition != std::string::npos && dotPosition < filePath.length() - 1) 
    {
        return "." + filePath.substr(dotPosition + 1);
    }
    return ".txt";
}

bool Client::sendHredes2(std::string  fullPath)
{
    if (isSentHreders)
        return 1;
    fileStream.open(fullPath.c_str(), std::ios::binary);
    fileStream.seekg(0, std::ios::end);
    file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string     fileContent;
    if(fileStream.is_open())
    {
        std::stringstream ss;
        ss << file_size;
        std::string sizo = ss.str();
        std::string fileExtension = getFileExtension(fullPath);
        std::string contentType =getContentTypeByExtension(fileExtension);
        fileContent = "HTTP/1.1 200 OK";
        fileContent += "\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + sizo + "\r\n\r\n";
        int bytsend = send(clientSocket, fileContent.c_str(), fileContent.size(), 0);
        if(bytsend == 0 || bytsend == -1)
           {
                fileStream.close();
                status = -1;
           }
        fileStream.close();
        fileDescriptor = open(fullPath.c_str(), O_RDONLY | O_NONBLOCK);
    }
    else 
    {
        status = 500;
        return 0;
    }
    isSentHreders = 1;
    return 0;

}

void Client::indexfile()
{
    std::string fulPath = filePath  +'/' +bestMatchLocation.index;
 
    if(access(fulPath.c_str(),F_OK) != -1)
    {

        if ((getFileExtension(fulPath) == ".py" || getFileExtension(fulPath) == ".php")
                    && bestMatchLocation.cgi =="on") {
                    handle_cgi(fulPath,"",getFileExtension(fulPath));
                    }
       else  if(sendHredes2(fulPath))
        {  
            int bufferSize = 1024;
            char buffer[bufferSize];
            memset(buffer, 0, bufferSize);
            int bytes = read(fileDescriptor,buffer, bufferSize - 1);
            if ( bytes <= 0)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
                return;
            }
            buffer[bytes] = '\0';
            int sendbytes = send(clientSocket, buffer, bytes, 0);
            if(sendbytes <= 0)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
                return;
            } 
            totalbytes += sendbytes;
             if(totalbytes == file_size)
            {
                if (fileDescriptor != -1)
                {close(fileDescriptor);fileDescriptor = -1;}
                status = -1;
                return;
            }
        }
    }
    else
    {
       status = 404;
       return;
    }
}

void Client::handleDirectoryAutoIndex()
{
    int sendbytes;
    if(resource[resource.length() - 1] != '/')
    {
         movedpermanent();
    }
    else if(!bestMatchLocation.index.empty())
        indexfile();
    else
    {
        std::vector<std::string> fileslist;
        DIR* directory = opendir(filePath.c_str());
        if(directory != NULL)
        {
            if (bestMatchLocation.auto_index == "on")
            {
                struct dirent* entry;
                while ((entry = readdir(directory))!= NULL)
                {
                    fileslist.push_back(entry->d_name);
                }
                closedir(directory);
                std::string htmlContent = "<!DOCTYPE html>\n<html>\n<head><title>Index of " +resource + "</title></head>\n<body>\n<h1>Index of "+ resource + "</h1>\n<ul>\n";
                std::vector <std::string>::iterator file;
                for (file = fileslist.begin(); file!=fileslist.end() ;++file)
                {
                    htmlContent += "<li><a href=\"" + concatenatePaths(resource, *file)  + "\">" + *file +"</a></li>\n";
                }
                setresponse("HTTP/1.1 200 OK", "text/html",htmlContent);
                sendbytes=  send(clientSocket, getresponse(), strlen(getresponse()), 0);
                if (sendbytes <= 0)
                {
                    status = -1;
                    return ;
                }
                totalbytes += sendbytes;
                if (totalbytes == strlen(getresponse()))
                {
                    status =-1;
                    return ;
                }
            }
            else //if (bestMatchLocation.auto_index == "off")
            {
                closedir(directory);
                status = 403;
                return;
            }
        } 
        else 
        {
            closedir(directory);
            status = 404;
            return;
        }  
    }
}
