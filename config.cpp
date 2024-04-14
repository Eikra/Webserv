/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iecharak <iecharak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 20:27:19 by iecharak          #+#    #+#             */
/*   Updated: 2024/04/08 17:00:50 by iecharak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

// template <typename T>
// std::string my_to_string(T value) {
//     std::ostringstream os;
//     os << value;
//     return os.str();
// }

bool    InvalidPath(std::string &root)
{
    char resolved_path[PATH_MAX];
    memset(resolved_path, 0, PATH_MAX);
    if (realpath(root.c_str(), resolved_path) == NULL)
        return true;
    root = resolved_path;
    struct stat filinfo ;
    if (stat(root.c_str(),&filinfo) == 0 && !(S_ISDIR(filinfo.st_mode)))
        return true;
    if (root.size() && root[root.size() - 1] != '/')
        root += "/";
    return false;
}

bool    InvalidFile(std::string &root)
{
    char resolved_path[PATH_MAX];
    memset(resolved_path, 0, PATH_MAX);
    if (realpath(root.c_str(), resolved_path) == NULL)
        return true;
    root = resolved_path;
    struct stat filinfo ;
    if (stat(root.c_str(),&filinfo) == 0 && !(S_ISREG(filinfo.st_mode)))
        return true;
    if (root.size() && root[root.size() - 1] != '/')
        root += "/";
    return false;
}

size_t  characterCount(const std::string& line, char c)
{
    size_t characterCount = 0;
    
    for (size_t i = 0; i < line.length(); i++) {
        if(line[i] == c)
            characterCount++;
    }
    return (characterCount);
}

bool    checkUnsafeCharInPath(std::string &string)
{
    int i = 0;
    while (string[i])
    {
        if (!std::isdigit(string[i]) && !isalpha(string[i]) && string[i] != '/' && string[i] != '.' && string[i] != '-' && string[i] != '_')
            return 1;
        i++;
    }
    return 0;
}

bool isLocationDuplicated(std::deque<location>& locations,std::string& path) {
    for (std::deque<location>::iterator it = locations.begin(); it != locations.end(); ++it) {
        if (it->path_location == path) {
            return true;  // Duplicate found
        }
    }
    return false;  // No duplicate found
}

void    checkServerErrors(server &serv)
{
    //check non-configuered directives
    if (!serv.root.size())
        throw MyRuntimeError("Error no root directive specified for the server!");
    if (!serv.host.size())
        throw MyRuntimeError("Error no host directive specified for the server!");
    if (!serv.portKey.size())
        throw MyRuntimeError("Error no port directive specified for the server!");
    serv.port = atoi(serv.portKey.c_str());
    size_t i = 0;
    while (i < serv.locations.size())
    {
        if (!serv.locations[i].root.size())
            serv.locations[i].root = serv.root;
        if (!serv.locations[i].auto_upload.size())
            serv.locations[i].auto_upload = "off";
        if (!serv.locations[i].auto_index.size())
            serv.locations[i].auto_index = "off";
        if (!serv.locations[i].cgi.size())
            serv.locations[i].cgi = "off";
        if (!serv.locations[i].upload_path.size())
            serv.locations[i].upload_path = serv.locations[i].root;
        i++;
    }
    (void)serv;
}

static std::string strtrim(const std::string& input, int *i) {
    size_t start = 0;
    size_t end = input.length();

    while (start < end && std::isspace(input[start])) {
        ++start;
    }
    while (end > start && std::isspace(input[end - 1])) {
        --end;
    }
    if (start > 8 || (input.length() - end) > 8)
        throw MyRuntimeError("Please do not enter more then 5 consecutive spaces! : " + my_to_string(*i));
    return input.substr(start, end - start);
}

std::map<std::string, std::string> getKeyValue(std::string &line, int* j)
{
    std::string key, value;
    std::stringstream ss(line);
    std::map<std::string, std::string> keyValue;
    int i = 0;
    while (line[i] && !std::isspace(line[i]))
        i++;
    while (line[i] && std::isspace(line[i]))
        i++;
    if (!line[i])
        throw MyRuntimeError("Error in directive! : " + my_to_string(*j));
    while (line[i] && !std::isspace(line[i]))
        i++;
    if (line[i])
        throw MyRuntimeError("Error in directive! : " + my_to_string(*j) + line[i]);
    ss >> key;
    ss >> value;
    keyValue[key] = value;
    return keyValue;
}

void    checkPort(std::string &port, int *i)
{
    int j = 0;
    if (port.size() > 5)
        throw MyRuntimeError("Error in port number! : " + my_to_string(*i));
    while (port[j] && std::isdigit(port[j]))
    {
        j++;
    }
    if (!j || j != (int)port.size())
        throw MyRuntimeError("Error in port number! : " + my_to_string(*i));
    std::istringstream line(port);
    j = -1;
    if (!(line >> j) || j > 65535 || j <= 0)
        throw MyRuntimeError("Error in port number! : " + my_to_string(*i));
}

size_t checkSize(std::string &size, int *j)
{    
    int i = 0;
    while (size[i])
    {
        if (!std::isdigit(size[i]))
            throw MyRuntimeError("invalid number for max_client_body_size directive! : " + my_to_string(*j));
        i++;
    }

    size_t result;

    std::istringstream iss(size);
    
    iss >> result;

    if (iss.fail() || result == 0)
        throw MyRuntimeError("invalid number for max_client_body_size directive! : " + my_to_string(*j));
    return result;
}

void    checkHost(std::string &host, int *i)
{
    if (host == "localhost" && host.size() == 9)
    {
        host = "127.0.0.1";
        return;
    }
    if (characterCount(host, '.') != 3)
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    int j = 0;
    while (host[j])
    {
        if (host[j] != '.' && !std::isdigit(host[j]))
            throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
        j++;
    }
    j = 0;
                    //*********
    if (!std::isdigit(host[j]))
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    int k = j;
    while (std::isdigit(host[j]))
        j++;
    if (atoi(host.substr(k, j).c_str()) > 255)
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    k = j;
                    //*********
    if (host[j++] != '.')
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    if (!std::isdigit(host[j]))
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    while (std::isdigit(host[j]))
        j++;
    if (atoi(host.substr(k, j).c_str()) > 255)
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
                    //*********
    if (host[j++] != '.')
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    if (!std::isdigit(host[j]))
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    k = j;
    while (std::isdigit(host[j]))
        j++;
    if (atoi(host.substr(k, j).c_str()) > 255)
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
        //***********
    if (host[j++] != '.')
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    if (!std::isdigit(host[j]))
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i));
    k = j;
    while (std::isdigit(host[j]))
        j++;
    if (atoi(host.substr(k, j).c_str()) > 255)
        throw MyRuntimeError("Error in host directive! : " + my_to_string(*i)); 
}

void    checkServer_Name(std::string &serverName, int *i)
{
    int j = 0;
    while (serverName[j])
    {
        if (!std::isdigit(serverName[j]) && !isalpha(serverName[j]) && serverName[j] != '.' && serverName[j] != '-' && serverName[j] != '_')
            throw MyRuntimeError("Error in server name directive! : " + my_to_string(*i));
        j++;
    }
}

void    checkServer_Names(std::vector<std::string> &server_names, std::string line, int *i)
{
    size_t j = 0;
    while (j < line.size())
    {
        size_t k = 0;
        while (j < line.size() && std::isspace(line[j]))
        {
            k++;
            j++;
        }
        if (k > 5)
            throw MyRuntimeError("Please do not enter more then 5 consecutive spaces! : " + my_to_string(*i));
        if (!k)
            j++;
    }
    j = 0;
    std::string server_name;
    std::istringstream iss(line);
    
    while (iss >> server_name) {
        if (std::find(server_names.begin(), server_names.end(), server_name) != server_names.end())
            throw MyRuntimeError("Error duplicate server name in server! : " + my_to_string(*i));
        server_names.push_back(server_name);
        if (server_names.size() > 3)
            throw MyRuntimeError("Too many server names! : " + my_to_string(*i));
    }
    (void)server_names;
}

bool startsWith(const std::string& line, const std::string& prefix) {
    return (line.substr(0, prefix.length()) == prefix && line.substr(0, prefix.length()).size() == prefix.size());
}
bool endsWith(const std::string& line, const std::string& sufix) {
    return (line.substr(line.size() - sufix.size(), sufix.length()) == sufix && line.substr(line.size() - sufix.size(), sufix.length()).size() == sufix.size());
}

std::map<std::string, std::string>  parseErrorpage(std::string &line, int *j)
{
    std::map<std::string, std::string> err;

    line = line.substr(10, line.size());
    line = strtrim(line, j);

    std::string key, value;
    std::stringstream ss(line);
    std::map<std::string, std::string> keyValue;
    int i = 0;
    while (line[i] && !std::isspace(line[i]))
        i++;
    while (line[i] && std::isspace(line[i]))
        i++;
    if (!line[i])
        throw MyRuntimeError("Error in directive! : " + my_to_string(*j));
    while (line[i] && !std::isspace(line[i]))
        i++;
    if (line[i])
        throw MyRuntimeError("Error in directive! : " + my_to_string(*j));
    ss >> key;
    ss >> value;
    keyValue[key] = value;
    return keyValue;
}

std::string parseLocationPath(std::string &line, int *i)
{
    line = line.substr(9, line.size());
    line = line.substr(0, line.size() - 2);
    line = strtrim(line, i);
    if (!line.size())
        throw MyRuntimeError("Error in the location's path directive! : " + my_to_string(*i));
    if (line[0] != '/')
        throw MyRuntimeError("Error: please make sure the location path start with a / : " + my_to_string(*i));
    if (line.size() > 1 && line[line.size() -1 ] == '/')
        throw MyRuntimeError("Error: please make sure the location path does not end with a / : " + my_to_string(*i));
    return line;
}

std::string checkIfOnOff(std::string prefix ,std::string line,int *i)
{
    line = strtrim(line, i);
    if ((!line.find("on") && line.size() == 2) || (!line.find("off") && line.size() == 3))
        return line;
    else
        throw MyRuntimeError("Error the " + prefix + "'s value should be on or off! : " + my_to_string(*i));
    return line;
}

std::string getPath(std::string line,int *i)
{
    line = strtrim(line, i);
    if (!line.size() || checkUnsafeCharInPath(line))
        throw MyRuntimeError("Error invalid path! : " + my_to_string(*i));
    return line;
}

std::deque<std::string> parseMethod(std::string line,int *i)
{
    size_t j = 0;
    while (j < line.size())
    {
        size_t k = 0;
        while (j < line.size() && std::isspace(line[j]))
        {
            k++;
            j++;
        }
        if (k > 5)
            throw MyRuntimeError("Please do not enter more then 5 consecutive spaces! : " + my_to_string(*i));
        if (!k)
            j++;
    }
    std::deque<std::string> methods;
    std::string method;
    std::istringstream iss(line);
    
    while (iss >> method)
    {
        if (!((method == "POST" &&  method.size() == 4) || (method == "GET" && method.size() == 3)
            || (method == "DELETE" && method.size() == 6)))
            throw MyRuntimeError("Error: method not allowed! : " + my_to_string(*i));
        if (std::find(methods.begin(), methods.end(), method) != methods.end())
            throw MyRuntimeError("Error duplicate method in location! : " + my_to_string(*i));
        methods.push_back(method);
    }
    if (!methods.size())
        throw MyRuntimeError("Error no allowed method specified in location! : " + my_to_string(*i));
    return methods;
}

bool    isServerDuplicated(std::deque<server> &servers, server &serv)
{
    for (std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->host == serv.host && it->port == serv.port && it->server_names == serv.server_names)
        return true;
    }
    return 0;
}
std::vector<std::string>    parseRedirection(std::string value, int *i)
{
    std::vector<std::string> redirect;
    std::map<std::string, std::string> keys = getKeyValue(value, i);
    redirect.push_back(keys.begin()->first);
    redirect.push_back(keys.begin()->second);
    if (redirect[0].size() != 3 || (redirect[0].size() == 3 &&  strcmp(redirect[0].c_str(), "301") && strcmp(redirect[0].c_str(), "302")))
        throw MyRuntimeError("Please ensure that status code is 301 or 302 for the redirection! : " + my_to_string(*i));
    return redirect;
}

void parseConfigFile(std::deque<server> &servers, std::string fileName)
{
    int     i = 0;
    int     j = 0;
    std::fstream file(fileName.c_str());
    if (!file.is_open()) 
        throw MyRuntimeError("the configuration file can't be opened, check permissions please!");
        
    while (!file.eof())
    {
        if (j > 1)
            throw MyRuntimeError("Please do not enter more then 2 consecutive empty lines! : " + my_to_string(i));
        server      serv;
        std::string line;
        std::getline(file, line);i++;
        line = strtrim(line, &i);
        if (!line.size())
            {j++;continue;}
        j = 0;
        if (line != "server {" && line.size() != 8)
            throw MyRuntimeError("Error in the server's block opening! : " + my_to_string(i));
                            //parsing the host, port, server name and the root
        serv.mx_cl_bd_size = 0;
        while (!file.eof())
        {
            if (j > 1)
                throw MyRuntimeError("Please do not enter more then 2 consecutive empty lines! : " + my_to_string(i));
            std::getline(file, line);i++;
            line = strtrim(line, &i);
            if (!line.size())
                {j++;continue;}
            j = 0;
            
            if (line.size() && (startsWith(line , "error_page ") || startsWith(line , "location ")))
                break;
            else if (line.size() && (startsWith(line , "return ")))
            {
                if (serv.serverRedirect.size())
                    throw MyRuntimeError("duplicated return directive! : " + my_to_string(i));
                else
                    serv.serverRedirect = parseRedirection(strtrim(line.substr(7, line.size()), &i), &i);
                continue;
            }
            else if (line.size() && (startsWith(line , "server_name ")))
            {
                if (serv.server_names.size())
                    throw MyRuntimeError("duplicated server_name directive! : " + my_to_string(i));
                checkServer_Names(serv.server_names, strtrim(line.substr(12, line.size()), &i), &i);
                continue;
            }
            std::map<std::string, std::string> keys = getKeyValue(line, &i);
            if (keys.begin()->first == "port" && keys.begin()->first.size() == 4)
            {
                if (serv.portKey.size())
                    throw MyRuntimeError("duplicated listen directive! : " + my_to_string(i));
                else
                    serv.portKey = keys.begin()->second;
                checkPort(serv.portKey, &i);
            }
            else if (keys.begin()->first == "host" && keys.begin()->first.size() == 4)
            {
                if (serv.host.size())
                    throw MyRuntimeError("duplicated host directive! : " + my_to_string(i));
                else
                    serv.host = keys.begin()->second;
                checkHost(serv.host, &i);
            }
            // else if (keys.begin()->first == "server_name" && keys.begin()->first.size() == 11)
            // {
            //     if (serv.server_name.size())
            //         throw MyRuntimeError("duplicated server_name directive! : " + my_to_string(i));
            //     else
            //         serv.server_name = keys.begin()->second;
            //     checkServer_Name(serv.server_name, &i);
            // }
            else if (keys.begin()->first == "root" && keys.begin()->first.size() == 4)
            {
                if (serv.root.size())
                    throw MyRuntimeError("duplicated root directive! : " + my_to_string(i));
                else if (checkUnsafeCharInPath(keys.begin()->second))
                    throw MyRuntimeError("unsafe character in the root path directive! : " + my_to_string(i));
                else if (InvalidPath(keys.begin()->second))
                    throw MyRuntimeError("Invalid path in the root path directive! : " + my_to_string(i));
                else
                    serv.root = keys.begin()->second;
            }
            else if (keys.begin()->first == "max_client_body_size" && keys.begin()->first.size() == 20)
            {
                if (serv.mx_cl_bd_size != 0)
                    throw MyRuntimeError("duplicated max_client_body_size directive! : " + my_to_string(i));
                
                serv.mx_cl_bd_size = checkSize(keys.begin()->second, &i);
            }
            else
                throw MyRuntimeError("uknowen directive! : " + my_to_string(i));
        }
                            //parcing error pages
        while (!file.eof())// && (startsWith(line , "error_page ") || (!line.size() && !file.eof()))
        {
            if (!line.size())
            {
                std::getline(file, line);i++;
                line = strtrim(line, &i);
                j++;continue;
            }
            
            if (j > 1)
                throw MyRuntimeError("Please do not enter more then 2 consecutive empty lines! : " + my_to_string(i - 1));
            if (!startsWith(line , "error_page "))
                break;
            std::map<std::string, std::string> err = parseErrorpage(line, &i);
            serv.err_pages[err.begin()->first] = err.begin()->second;
            if (InvalidFile(err.begin()->second))
                throw MyRuntimeError("Invalid path for the error page directive! : " + my_to_string(i));
            std::getline(file, line);i++;
            line = strtrim(line, &i);
            j = 0;
        }
                            //parsing the location directives
        while (!file.eof())
        {
            if (j > 1)
                throw MyRuntimeError("Please do not enter more then 2 consecutive empty lines! : " + my_to_string(i));
            if (!line.size())
            {
                std::getline(file, line);i++;
                line = strtrim(line, &i);
                j++;
                continue;
            }
            j = 0;
            if (!startsWith(line , "location ") || !endsWith(line , " {"))
                throw MyRuntimeError("Error in the location's block opening! : " + my_to_string(i));
            location    loc;
            //************************* parsing the location directive's atributes ******************************************
            loc.path_location = parseLocationPath(line, &i);
            if (checkUnsafeCharInPath(loc.path_location))
                throw MyRuntimeError("unsafe character in the location's path directive! : " + my_to_string(i));
            if (isLocationDuplicated(serv.locations, loc.path_location))
                throw MyRuntimeError("Error duplicated location directive! : " + my_to_string(i));
            while (!file.eof())
            {
                if (j > 1)
                    throw MyRuntimeError("Please do not enter more then 2 consecutive empty lines! : " + my_to_string(i));
                std::getline(file, line);i++;
                line = strtrim(line, &i);
                if (!line.size())
                    {j++;continue;}
                j = 0;
                if (line == "}" && line.size() == 1)
                    break;
                            // check directives
                if (startsWith(line , "root ") && !loc.root.size())
                {
                    if ((startsWith(line , "root") && line.size() == 4))
                        throw MyRuntimeError("Error: no information for " + line +" specified in location! : " + my_to_string(i));
                    loc.root = getPath(line.substr(5 ,line.size()),&i);
                    if (InvalidPath(loc.root))
                        throw MyRuntimeError("Invalid path in the root path directive! : " + my_to_string(i));
                }
                else if (startsWith(line , "upload_path ") && !loc.upload_path.size())
                {
                    loc.upload_path = getPath(line.substr(12 ,line.size()),&i);
                    if (InvalidPath(loc.upload_path))
                        throw MyRuntimeError("Invalid path in the upload_path path directive! : " + my_to_string(i));
                }
                else if (startsWith(line , "method ") && !loc.allow_methods.size())
                    loc.allow_methods = parseMethod(line.substr(7 ,line.size()),&i);
                else if (startsWith(line , "index ") && !loc.index.size())
                    loc.index = getPath(line.substr(6 ,line.size()),&i);
                else if (startsWith(line , "autoindex ") && !loc.auto_index.size())
                    loc.auto_index  = checkIfOnOff("autoindex" ,line.substr(10, line.size()),&i);
                else if (startsWith(line , "client_body_in_file_only ") && !loc.auto_upload.size())
                    loc.auto_upload  = checkIfOnOff("client_body_in_file_only" ,line.substr(25, line.size()),&i);
                else if (startsWith(line , "cgi ") && !loc.cgi.size())
                    loc.cgi  = checkIfOnOff("cgi" ,line.substr(3, line.size()),&i);
                else if (startsWith(line , "return ") && !loc.locationRedirect.size())// add after 
                    loc.locationRedirect = parseRedirection(strtrim(line.substr(7, line.size()), &i), &i);
                            // check duplicate directives
                else if (startsWith(line , "root ") && loc.root.size())
                    throw MyRuntimeError("Error duplicate root directive! : " + my_to_string(i));
                else if (startsWith(line , "method ") && loc.allow_methods.size())
                    throw MyRuntimeError("Error duplicate method directive! : " + my_to_string(i));
                else if (startsWith(line , "index ") && loc.index.size())
                    throw MyRuntimeError("Error duplicate index directive! : " + my_to_string(i));
                else if (startsWith(line , "autoindex ") && loc.auto_index.size())
                    throw MyRuntimeError("Error: duplicate autoindex directive! : " + my_to_string(i));
                else if (startsWith(line , "client_body_in_file_only ") && loc.auto_upload.size())
                    throw MyRuntimeError("Error: duplicate client_body_in_file_only directive! : " + my_to_string(i));
                else if (startsWith(line , "cgi ") && loc.cgi.size())
                    throw MyRuntimeError("Error: duplicate cgi directive! : " + my_to_string(i));
                else if (startsWith(line , "return ") && loc.locationRedirect.size())
                    throw MyRuntimeError("duplicated return directive! : " + my_to_string(i));
                else if (startsWith(line , "upload_path ") && loc.upload_path.size())
                    throw MyRuntimeError("duplicated upload_path directive! : " + my_to_string(i));
                            //chack atributes 
                else if ((startsWith(line , "root") && line.size() == 4)
                        || ((startsWith(line , "method") && line.size() == 6))
                        || ((startsWith(line , "index") && line.size() == 5))
                        || ((startsWith(line , "autoindex") && line.size() == 9))
                        || ((startsWith(line , "client_body_in_file_only") && line.size() == 24))
                        || ((startsWith(line , "cgi") && line.size() == 3))
                        || (startsWith(line , "return") && line.size() == 6)
                        || (startsWith(line , "upload_path") && line.size() == 11))
                    throw MyRuntimeError("Error: no information for the " + line +" parameter specified in location! : " + my_to_string(i));
                else throw MyRuntimeError("uknowen directive! : " + my_to_string(i));
            }
            //*******************************************************************
            serv.locations.push_back(loc);
            std::getline(file, line);i++;
            line = strtrim(line, &i);
            while (!file.eof() && !line.size())
            {
                std::getline(file, line);i++;
                line = strtrim(line, &i);
                j++;
            }
            if (j > 1)
                throw MyRuntimeError("Please do not enter more then 2 consecutive empty lines! : " + my_to_string(i - 1));
            //checking the server block closing
            j = 0;
            if ((!file.eof() && line == "}" && line.size() == 1))
                break;
            else if (file.eof() && line != "}" && line.size() != 1)
                throw MyRuntimeError("Error in the server's block closing! : " + my_to_string(i));
        }
        serv.serverSocket = -1;
        checkServerErrors(serv);
        if (isServerDuplicated(servers, serv))
            throw MyRuntimeError("Error: Duplicate server found with host: " + serv.host + " and port: " + my_to_string(serv.port));
        servers.push_back(serv);
        j = 0;
    }
    (void)servers;
}
