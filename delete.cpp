#include "config.hpp"

int Client::hadelremovedir(std::string path)
{
    DIR *dir =opendir(path.c_str());
   if(dir != NULL)
   {
            struct  dirent* entry;
             while ((entry = readdir(dir)) != NULL) 
            {
                std::string file_name = entry->d_name;
                if (file_name != "." && file_name != "..")
                {
                    std::string fileremove = path +"/"+ file_name;
                     if (entry->d_type == DT_DIR) 
                     {
                        Delete_method(fileremove);
                        if(status == 0)
                        {
                       int res = rmdir(fileremove.c_str());
                    if(res == 0 )
                    {
                        setresponse("HTTP/1.1 204 No Content", "text/html",
                        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                        "<html><head>\r\n<title>204 No Content</title>\r\n</head><body>\r\n"
                        "<h1>No Content</h1>\r\n<p>The request was successful, but there is no content to send in the response.</p>\r\n"
                        "</body></html>\r\n");
                        size_t sendReturn = send(clientSocket, getresponse(), strlen(getresponse()), 0);
                        if(sendReturn <= 0  || sendReturn == strlen(getresponse()))
                        {
                            status = -1;
                        }
                    }
                        }
                     }
                     else
                     {
                        Delete_method(fileremove);
                     }
                }
            }
   }
   else 
       {
        status = 404;
        return 1;
       }
    closedir(dir);
    return 1;
}
void Client::checkpermission(std::string path)
{
    struct stat fileinfo;
    if(stat(path.c_str(),&fileinfo) == 0 )
    {
        if (!((fileinfo.st_mode & S_IRUSR) ||
            (fileinfo.st_mode & S_IWUSR) ||
            (fileinfo.st_mode & S_IXUSR)))
            {
                status = 403;
                return;
            }
    }
    DIR *dir =opendir(path.c_str());
   if(dir != NULL)
   {
            struct  dirent* entry;
             while ((entry = readdir(dir)) != NULL)
            {
                std::string file_name = entry->d_name;
                if (file_name != "." && file_name != "..")
                {
                    std::string fileremove = path +"/"+ file_name;
                    checkpermission(fileremove);
                }
            }
   }
   else
        return ;
    closedir(dir);
}
void Client::Delete_method(std::string path)
{
    if (bestMatchLocation.locationRedirect.size())
    {
        checkLocationRedirection();
        return;
    }
    struct stat fileinfo;
    if(stat(path.c_str(),&fileinfo) == 0 )
    {
        if( access(path.c_str(),F_OK) !=-1)
        { 
             if(S_ISREG(fileinfo.st_mode))
            {
                if(status == 0)
                {
                    int res = unlink(path.c_str());
                if(res == 0 )
                {
                setresponse("HTTP/1.1 204 No Content", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                "<html><head>\r\n<title>204 No Content</title>\r\n</head><body>\r\n"
                "<h1>No Content</h1>\r\n<p>The request was successful, but there is no content to send in the response.</p>\r\n"
                "</body></html>\r\n");
                    size_t sendReturn = send(clientSocket, getresponse(), strlen(getresponse()), 0);
                    if(sendReturn <= 0  || sendReturn == strlen(getresponse()))
                    {
                        status = -1;
                    }
                }
                }
                else if(status == 0)
                {
                    
                    status = 404;
                    return;
                }
                    //responceerr("404 ","Not Found");
            }
            else if(S_ISDIR(fileinfo.st_mode))
            {
                if(status == 0)
                    {
                if(getfilePath().back() !='/' && status == 0)
                {
                    status = 409;
                    return;
                } 
                else
                {
                    hadelremovedir(path);
                    
                        int res =rmdir(path.c_str());
                    if(res == 0 )
                    {
                        setresponse("HTTP/1.1 204 No Content", "text/html",
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n<title>204 No Content</title>\r\n</head><body>\r\n"
                    "<h1>No Content</h1>\r\n<p>The request was successful, but there is no content to send in the response.</p>\r\n"
                    "</body></html>\r\n");
                        size_t sendReturn = send(clientSocket, getresponse(), strlen(getresponse()), 0);
                        if(sendReturn == 0 || sendReturn == strlen(getresponse()))
                            {
                            status = -1;
                            }
                        }
                    
                    else 
                    {
                        status = 404;
                        return;
                    }
                }
                }
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
        return;
    }
}