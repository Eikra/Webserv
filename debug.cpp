#include "config.hpp"

// void            Client::printparsedRequest()
// {
//     std::cout << resource << std::endl; 
//     for (std::map<std::string, std::string>::iterator it = headers.begin();it != headers.end(); ++it)
//     {
//         std::cout << it->first << ": " << it->second << std::endl;
//     }
// }

void    printRequest(const char *c)
{
    int i = 0;
    while(c[i])
    {
        if (c[i] == '\r')
            std::cout << "\\r";
        else if (c[i] =='\n')
            std::cout << "\\n\n";
        else
            std::cout << c[i];
        i++;
    }
}

void    printBody(char *c)
{
    int i = 0;
    while(i < 1024 && c[i])
    {
        if (c[i] == '\r')
            std::cout << "\\r";
        else if (c[i] =='\n')
            std::cout << "\\n\n";
        else
            std::cout << c[i];
        i++;
    }
    //std::cout << std::endl;
}

void    printRequest(char *c, int bytes)
{
    int i = 0;
    while(i < bytes)
    {
        if (c[i] == '\r')
            std::cout << "\\r";
        else if (c[i] =='\n')
            std::cout << "\\n\n";
        else if (isdigit(c[i])  || isalpha(c[i]))
            std::cout << c[i];
        i++;
    }
    //std::cout << std::endl;
}

void    printServers(std::deque<server> &serv)
{
    size_t i = 0;
    while (i < serv.size())
    {
        std::cout << "host: " << serv[i].host << "\n";
        std::cout << "port: " << serv[i].portKey << "\n";
        std::cout << "server_names: ";
        for(size_t j = 0 ; j < serv[i].server_names.size(); j++)
            std::cout << serv[i].server_names[j] << " ";
        std::cout << "\n";
        std::cout << "root: " << serv[i].root << "\n";
        if (serv[i].serverRedirect.size())
            std::cout << "redirection: " << serv[i].serverRedirect[0] << " " << serv[i].serverRedirect[1] << "\n";
        for (std::map<std::string, std::string>::iterator it = serv[i].err_pages.begin(); it != serv[i].err_pages.end(); ++it)
            std::cout << "error page: " << it->first << " " << it->second << "\n";
        std::cout << "max client body size: " << serv[i].mx_cl_bd_size << "\n";
        size_t j = 0;
        while (j < serv[i].locations.size())
        {
            std::cout << "location: " << serv[i].locations[j].path_location << "\n";
            std::cout << "     root: " << serv[i].locations[j].root << "\n";
            std::cout << "     auto_upload: " << serv[i].locations[j].auto_upload << "\n";
            std::cout << "     cgi: " << serv[i].locations[j].cgi << "\n";
            std::cout << "     auto_index: " << serv[i].locations[j].auto_index << "\n";
            std::cout << "     index: " << serv[i].locations[j].index << "\n";
            std::cout << "     upload_path: " << serv[i].locations[j].upload_path << "\n";
            
            if (serv[i].locations[j].locationRedirect.size())
                std::cout << "     redirection: " << serv[i].locations[j].locationRedirect[0] << " " << serv[i].locations[j].locationRedirect[1] << "\n";
            std::cout << "     allowd methods:";
            for(size_t k = 0; k < serv[i].locations[j].allow_methods.size(); k++)
                std::cout << " " <<  serv[i].locations[j].allow_methods[k];
            std::cout << std::endl;
            j++;
        }
        i++;
        std::cout << "=================================================\n";
    }
}
