#include "config.hpp"

void    Client::startTimeOut()
{
    if(!timer)
        time(&timer);
    else
    {
        time_t currentTime = 0;
        if(time(&currentTime) > -1 && (currentTime - timer) > TIMEOUT)
            status = 408;
    }
}

void split(const std::string& input, char delimiter, std::string& part1, std::string& part2) 
{
    std::size_t pos = input.find(delimiter);
    if (pos != std::string::npos) {
        part1 = input.substr(0, pos);
        part2 = input.substr(pos + 1);
    } 
    else
    {
        part1 = input;
        part2 = "";
    }
}

bool    matchName(std::string servName, std::vector<std::string> &server_names)
{
    for (size_t i = 0; i < server_names.size(); i++)
    {
        if (servName == server_names[i] && servName.size() == server_names[i].size())
            return true;
    }
    return false;
}

void    Client::matchingServer(server &serv, std::deque<server> &servers)
{
    requestLine.clear();
    if (status == 0 && headers.find("Host") != headers.end())//&& isDupPortHost(servers, serv)
    {
        std::string servName, port;
        split(headers.find("Host")->second, ':', servName, port);
        for (std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
        {
            if (it->host == serv.host && it->port == serv.port && matchName(servName, it->server_names))
            {
                matchingLocation(*it);
                return;
            }
        }
    }
    matchingLocation(serv);
}

void    Client::readRequest(server &serv, std::deque<server> &servers)
{
    if (!isGetAllHeaders && status == 0)
    {
        memset(buffer,0, 2048);
        bytes = recv(clientSocket, buffer, 2047, 0);
        timer = 0;
        if (bytes <= 0)
        {
            isGetAllHeaders = true;//???
            status = -1;
            return;
        }
        buffer[bytes] = '\0';
        requestParser();
        if (isGetAllHeaders || status != 0)
        {
            matchingServer(serv, servers);
            if (status == 0 && method == "POST")
                methodPost();
        }
    }
    else if (status == 0 && isGetAllHeaders && !isGetFullBody)
    {
        memset(bodyBuffer,0, sizeof(bodyBuffer));
        if (chunkSize > 1024 || chunkSize == 0)
            bytes = recv(clientSocket, bodyBuffer, 1024, 0);
        else if (chunkSize < 1024 && chunkSize)
            bytes = recv(clientSocket, bodyBuffer, chunkSize, 0);
        timer = 0;
        if (bytes <= 0)
        {
            status = -1;
            if (isFileCreated && inputfile.is_open())
            {
                inputfile.close();
                remove(fileNamePost.c_str());
            }
            if (fileDescriptor != -1)
            {
                close (fileDescriptor);
                fileDescriptor = -1;
            }
        }
        else if (bytes > 0)
        {
            if (method == "POST" && status == 0)
                methodPost();
        }
    }
}

Client::Client(int socket):clientSocket(socket),status(0), bytes(0), timer(0), isGetAllHeaders(false)
                            , isGetFullBody(false), isFileCreated(false), contentLength(0)
                            , isChunked(false), isLastChunk(false), totalReaded(0)
                            , chunkSize(0), isFirstLine(false), isSentHreders(false)
                            , filePath(""), bestMatchLength(0), bestmatch(""), chekcgi(0)
{
    memset(buffer, 0, 1024);
    memset(bodyBuffer, 0, 1024);
    isSentHreders = false;
    pid = -1;
    out = -1;
    status = 0;
    start_time = 0;
    fileDescriptor = -1;
    name1 = "name1" + generateNewFileName(clientSocket);
    errorname = "errorname" + generateNewFileName(clientSocket);
    chekcgi = 0;
    err = -1;
    headerSize = 0;
    totalbytes = 0;
    flagcgi = 0;
    flag = 0;
    sencgi = 0 ;
    sizetotal = 0;
    condition = 2;
}

Client::Client(const Client &src)
{
    clientSocket = src.clientSocket;
    timer = src.timer;
    status = src.status;
    bytes = src.bytes;
    isGetAllHeaders = src.isGetAllHeaders;
    isGetFullBody = src.isGetFullBody;
    isFileCreated = src.isFileCreated;
    contentLength = src.contentLength;
    bestMatchLength = src.bestMatchLength;
    bestmatch = src.bestmatch;
    isChunked = src.isChunked;
    isLastChunk = src.isLastChunk;
    totalReaded = src.totalReaded;
    filePath = src.filePath;
    chunkSize = src.chunkSize;
    name1 = src.name1;
    chekcgi= src.chekcgi;
    isSentHreders = src.isSentHreders ;
    fileDescriptor= src.fileDescriptor;
    start_time = src.start_time ;
    pid = src.pid;
    out = src.out;
    condition = 2;
    isFirstLine = src.isFirstLine;
    isGetAllHeaders = src.isGetAllHeaders;
    memset(buffer, 0, 1024);
    memset(bodyBuffer, 0, 1024);
    headerSize = src.headerSize;
    err =src.err ;
    totalbytes = src.totalbytes;
    flagcgi = src.flagcgi;
    flag = src.flag;
    sencgi = src.sencgi;
    sizetotal = src.sizetotal;
    errorname = src.errorname;
}

Client &Client::operator=(const Client &src)
{
    if (this != &src)
    {
        clientSocket = src.clientSocket;
        timer = src.timer;
        status = src.status;
        bytes = src.bytes;
        chunkSize = src.chunkSize;
        isGetAllHeaders = src.isGetAllHeaders;
        isGetFullBody = src.isGetFullBody;
        isFileCreated = src.isFileCreated;
        bestMatchLength = src.bestMatchLength;
        bestMatchLocation=src.bestMatchLocation;
        bestmatch = src.bestmatch;
        isChunked = src.isChunked;
        isLastChunk = src.isLastChunk;
        totalReaded = src.totalReaded;
        filePath = src.filePath;
        contentLength = src.contentLength;
        isFirstLine = src.isFirstLine;
        name1 = src.name1;
        condition = 2;
        start_time = src.start_time ;
        isSentHreders = src.isSentHreders ;
        fileDescriptor= src.fileDescriptor;
        isGetAllHeaders = src.isGetAllHeaders;
        chekcgi= src.chekcgi;
        memset(buffer, 0, 1024);
        memset(bodyBuffer, 0, 1024);
        headerSize = src.headerSize;
        err =src.err ;
        totalbytes = src.totalbytes;
        flagcgi = src.flagcgi;
        flag = src.flag;
        sencgi = src.sencgi;
        sizetotal = src.sizetotal;
        errorname = src.errorname;
    }
    return *this;
}

void    server::addClient(int socket)
{
    Client client(socket);
    clients.push_back(client);
}

void    server::dropClients(fd_set  *readfds ,fd_set  *writefds)
{
    int max = 0;
    std::list<Client>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->status == -1)
        {
            close(it->clientSocket);
            FD_CLR(it->clientSocket , readfds);
            FD_CLR(it->clientSocket , writefds);
            it = clients.erase(it);
        }
        else
        {
            if (it->clientSocket > max)
                max = it->clientSocket;
            ++it;
        }
    }
    if (max > serverSocket)
        maxSocket = max;
    else
        maxSocket = serverSocket;
}

void    server::handleClients(std::deque<server> &servers, fd_set *readSetTmp, fd_set *writeSetTmp)
{
    for (std::list<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (FD_ISSET(it->clientSocket, readSetTmp))
            it->readRequest(*this, servers);
        else if (it->status == 0 && (!it->isGetAllHeaders || (it->isGetAllHeaders && it->method == "POST" && !it->isGetFullBody)))
        {
            it->bytes = 0;
            it->startTimeOut();
        }
        else
            it->bytes = 0;
        if (it->status != -1 && FD_ISSET(it->clientSocket, writeSetTmp) && (it->isGetAllHeaders || it->status > 0))
            it->sendResponse(*this);
        else if (it->status != -1 && (it->isGetAllHeaders || it->status > 0))
            it->sendResponse(*this);
    }
}
