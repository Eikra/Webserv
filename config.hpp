/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iecharak <iecharak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 20:27:15 by iecharak          #+#    #+#             */
/*   Updated: 2024/04/08 16:58:34 by iecharak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <deque>
#include <map>
#include <string>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <iterator>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <fstream>
#include <sys/select.h>
#include <arpa/inet.h>
#include <iostream>
#include <deque>
#include <map>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <sys/stat.h>
#include <filesystem>
#include <dirent.h>
#include <stdexcept>
#include <fstream>
#include <fcntl.h>
#include <sys/wait.h>

#define HEADERSIZE 8000

#define TIMEOUT 30 // in seconds
struct server;
struct location
{
    //std::map<std::string, bool> encountered_keys;
    std::string path_location, root, auto_upload, auto_index, redirection, cgi, index, upload_path;
    
    std::vector<std::string> locationRedirect;
    std::deque<std::string> allow_methods;
    int     matcheLentgh;
};

class Client
{
    public :

        Client(int socket);
        int             clientSocket;
        int             status;
        int             bytes;
        time_t          timer;
        bool            isGetAllHeaders;
        bool            isGetFullBody;
        bool            isFileCreated;
        size_t          contentLength;
        bool            isChunked;
        std::string     hostcopy;
        int             portkeycopy;
        size_t          file_size;
        bool            isLastChunk;
        size_t          totalReaded;
        int             chunkSize;
        bool            isFirstLine;
        bool            isSentHreders;
        std::map<std::string, std::string> err_pagecopy;
        std::string     filePath;
        size_t          bestMatchLength;
        std::string     bestmatch;
        std::string     method;
        std::string     resource;
        std::string     response;
        std::map<std::string, std::string>          headers;
        std::string     httpVersion;
        std::string     querystr;
        std::string     htmlContent;
        int             fileDescriptor;
        std::ofstream   inputfile;//post method
        std::ifstream   fileStream;//get method
        location        bestMatchLocation;
        std::istringstream bodyStream;
        clock_t         start_time;
        int             flagcgi;
        int             flag;
        int             chekcgi;
        int             state;
        int             pid;
        int             out;
        int             hold;
        std::string     found;
        size_t          lastPos;
        std::ifstream   fileerror;
        char**          envp;
        char**          av;
        std::string     name;
        std::string     name1;
        //std::map<std::string, std::string> err_page;
        std::string     fileNamePost;
        std::vector<std::string> serverRedirect;
        std::string sending;

        std::stringstream sss;
        std::string     body;
        std::string     errorname;
        size_t          totalbytes;
        int err;
        int     sencgi;
        size_t   sizetotal;
        size_t          mx_cl_bd_size;
        int             condition;
        void            readRequest(server &serv, std::deque<server> &servers);
        void            matchingServer(server &serv, std::deque<server> &servers);
        void            methodPost();
        void            matchingLocation(server &serv);
        void            startTimeOut();
        std::string     toLowerCase(const std::string& str);
        std::string     getContentTypeByExtension(const std::string& extension);
        std::string     getExtensionByContentType(const std::string& contentType);
        bool            createFile();
        void            parseChunked();
        void            sendResponse(server &serv);
        void            methodGet();
        std::string     getfilePath();
        void            methodGetFile();
        bool            sendHredes();
        void            freeall();
        void            checkLocationRedirection();
        std::string     concatenatePaths(const std::string& path1, const std::string& path2);
        void            setQuery( std::string );
        void            checkEmptyChar();
        bool            checkheader(std::string line);
        bool            isInvalidURI();
        //void            parseRequest();
        std::string getFileExtension( std::string filePath); 
        void            parseFirstLine();
        char            buffer[2048];
        char            bodyBuffer[2048];
        Client(const Client &src);
        Client          &operator=(const Client &src);
        void            handle_cgi(std::string file,std::string filis,std::string exten );
        void            addingenv(std::string file,std::string exten);
        void            responceerr(std::string statuse ,std::string name);
        void            handleErrorResponse(std::string errorCode, std::string title);
        void            setresponse(const std::string stat,const std::string& contentType,const std::string& content);
        const           char* getresponse();
        void            movedpermanent();
        void            indexfile();
        bool            sendHredes2(std::string  fullPath);
        void            handleDirectoryAutoIndex();
        int             hadelremovedir(std::string path);
        void            Delete_method(std::string path);
        //void            printparsedRequest();
        void            requestParser();
        void            lineParser();
        int             headerSize;
        std::string     requestLine;
        void childprocess(std::string filis);
        void error(std::string filis);
        void parsingcgi(std::size_t pos );
        void parentprocess(std::string filis);
        void sendcgiresponce(std::string filis);
        std::string    pathvalid();
        bool  sendheadererror(std::string errorCode,std::string title,std::string filerr);
        void checkpermission(std::string path);
};

struct server
{
    std::string host, server_name, portKey;
    std::vector<std::string> server_names;
    int port;
    std::string root;
    std::vector<std::string> serverRedirect;
    
    size_t mx_cl_bd_size;
    std::map<std::string, std::string> err_pages;
    std::deque<location> locations;
    int serverSocket;
    int maxSocket;
    std::list<Client> clients;
    void    addClient(int socket);
    void    handleClients(std::deque<server> &servers, fd_set *readSetTmp, fd_set *writeSetTmp);
    void    dropClients(fd_set  *readfds ,fd_set  *writefds);
};

void    parseConfigFile(std::deque<server> &servers, std::string fileName);
void    printRequest(char *c);

bool    isHexadecimal(char c);
int     hexToDecimal(const std::string& hexString);
bool    isDupPortHost(std::deque<server> &servers, server &serv);

std::string     generateNewFileName(int i);
std::string     generateUniqueId();
void    printRequest(const char *c);
void    printRequest(char *c, int bytes);
void    printServers(std::deque<server> &serv);
template <typename T>
std::string my_to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

class MyRuntimeError : public std::exception 
{
    std::string message;
    public:
        MyRuntimeError(std::string str):message(str){}
        virtual ~MyRuntimeError() throw() {}
        virtual const char* what() const throw() {
            return message.c_str();
        }
};

template<typename Iterator, typename T>
Iterator my_find(Iterator begin, Iterator end, const T& value) {
    while (begin != end) {
        if (*begin == value) {
            return begin; // Found, return the iterator
        }
        ++begin;
    }
    return end; // Not found, return end iterator
}
#endif
