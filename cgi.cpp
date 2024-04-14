#include "config.hpp"

void Client::addingenv(std::string file, std::string exten)
{
    std::string execut;
    if (exten == ".php")
        execut = "./cgi-bin/php-cgi";
    else
        execut = "/usr/local/bin/python3";
    av = new char *[3];
    if (av == NULL)
        {status = 500; return;}
    av[0] = strdup(execut.c_str());
    if (av[0] == NULL)
        {status = 500;freeall(); return;}
    av[1] = strdup(file.c_str());
    if (av[1] == NULL)
        {status = 500;freeall(); return;}
    av[2] = NULL;
    std::map<std::string, std::string> mymap;
    mymap["REQUEST_METHOD"] = method;
    if (headers.find("Host") != headers.end())
    {
        mymap["SERVER_NAME"] = headers.find("Host")->first;
        mymap["SERVER_PORT"] = headers.find("Host")->second;
    }
    mymap["GATEWAY_INTERFACE"] = "CGI/1.1";
    mymap["DOCUMENT_ROOT"] = "../";
    mymap["PATH_INFO"] = file;
    mymap["SCRIPT_NAME"] = file;
    mymap["SCRIPT_FILENAME"] = file;
    mymap["REDIRECT_STATUS"] = "200";
    if (method == "GET")
    {
        mymap["QUERY_STRING"] = querystr;
    }
    if (headers.find("Cookie") != headers.end())
    {
        mymap["HTTP_COOKIE"] = headers.find("Cookie")->second;
    }
    if (method == "POST")
    {
        if (headers.find("Content-Type") != headers.end())
        {
            mymap["CONTENT_TYPE"] = headers.find("Content-Type")->second;
        }
        if (headers.find("Content-Length") != headers.end())
        {
            mymap["CONTENT_LENGTH"] = headers.find("Content-Length")->second;
        }
    }
    envp = new char *[mymap.size() + 1];
    if (envp == NULL)
        {status = 500;freeall(); return;}
    hold = mymap.size();
    size_t i = 0;
    for (std::map<std::string, std::string>::iterator iter = mymap.begin(); iter != mymap.end(); iter++)
    {
        std::string envar = iter->first + "=" + iter->second;
        envp[i] = new char[envar.size() + 1];
        if (envp[i] == NULL)
        {status = 500;freeall(); return;}
        std::strcpy(envp[i], envar.c_str());
        i++;
    }
    envp[mymap.size()] = NULL;
}
std::string Client::pathvalid()
{
    std::string path;
    size_t pos = getfilePath().find_last_of('/');
    if (pos != std::string::npos)
    {
        path = getfilePath().substr(0, pos);
    }
    name = path + '/' + name1;
    return name;
}
void Client::childprocess(std::string filis)
{
    out = open(name.c_str(), O_WRONLY | O_CREAT, 0755);
    err = open(errorname.c_str(), O_WRONLY | O_CREAT, 0755);
    if (out < 0 || err < 0)
    {
        kill(getpid(), SIGKILL);
    }
    if (method == "POST")
    {
        int in = open(filis.c_str(), O_RDONLY);
        if (in < 0)
           kill(getpid(), SIGKILL);
        int b = dup2(in, 0);
        if (b < 0)
           kill(getpid(), SIGKILL);
        close(in);
    }
    int d = dup2(err, 2);
    if (d < 0)
    {
        kill(getpid(), SIGKILL);
    }
    close(err);
    int i = dup2(out, 1);
    if (i < 0)
    {
       kill(getpid(), SIGKILL);
    }
    close(out);
    execve(av[0], av, envp);
    kill(getpid(), SIGKILL);
}
void Client::error(std::string filis)
{
    freeall();
    unlink(name.c_str());
    remove(fileNamePost.c_str());
    remove(errorname.c_str());
    unlink(filis.c_str());
}
void Client::parsingcgi(std::size_t pos)
{
    std::string first = sss.str().substr(0, pos + 2);
    std::map<std::string, std::string> myhead;
    size_t start = 0;
    size_t end = first.find("\r\n");
    std::string line;
    while (end != std::string::npos)
    {
        line = first.substr(start, end - start);
        size_t colon_pos = line.find(":");
        if (colon_pos != std::string::npos)
        {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2);
            myhead[key] = value;
        }
        start = end + 2;
        end = first.find("\r\n", start);
    }
    if (myhead.find("Status") != myhead.end())
    {
        found = "Status: " + myhead["Status"] + "\r\n";
        sending = "HTTP/1.1 " + myhead["Status"] + "\r\n";
        sencgi = 1;
    }
    else
    {
        sencgi = 2;
        sending = "HTTP/1.1 200 OK";
    }
}
void Client::parentprocess(std::string filis)
{
    if (flagcgi == 0)
    {
        int waiting = waitpid(pid, &state, WNOHANG);
        if (waiting == 0 && (clock() - start_time) > 10 * CLOCKS_PER_SEC)
        {
            status = 504;
            error(filis);
            kill(pid, SIGKILL);
            waiting = waitpid(pid, &state, 0);
            return;
        }
        if (waiting > 0)
        {   
            if( WIFEXITED(state))
            {
               // remove(errorname.c_str());
                flagcgi = 1;
                std::ifstream input(name.c_str());
                sss << input.rdbuf();
                if(!input.is_open() || sss.fail())
                {
                    status = 500;
                    error(filis);
                    return;
                }
                std::size_t pos = sss.str().find("\r\n\r\n");
                input.close();
                fileDescriptor = open(name.c_str(), O_RDONLY | O_NONBLOCK, 077);
                if(fileDescriptor < 0)
                {
                   status = 500;
                   error(filis);
                   return;
                }
                if (pos != std::string::npos)
                {
                    parsingcgi(pos);
                }
                else
                {
                    sencgi = 3;
                }
            }
            if (WIFSIGNALED(state)) {
            {
                status = 500;
                error(filis);
                return;
            }
            }
        }
        else if (waiting < 0)
        {
            status = 500;
            error(filis);
            kill(pid, SIGKILL);
            return;
        }
    }
}

void Client::sendcgiresponce(std::string filis)
{
    std::string buffers(1024, '\0');
    int bete = read(fileDescriptor, &buffers[0], 1023);
    if (bete < 0)
    {
        error(filis);
        if (fileDescriptor != -1)
        {close(fileDescriptor);fileDescriptor = -1;}
        status = -1;
        return;
    }
    buffers[bete] = '\0';
    if (sencgi == 1)
    {
        size_t pos = buffers.find(found);
        if (pos != std::string::npos)
        {
            buffers.erase(pos, found.length());
        }
        buffers.insert(0, sending);
        bete += sending.size();
        sencgi = 0;
    }
    else if (sencgi == 2)
    {
        buffers.insert(0, sending);
        bete += sending.size();
        sencgi = 0;
    }
    else if (sencgi == 3)
    {
        std::stringstream ss;
        ss << sss.str().size();
        std::string sizo = ss.str();
        std::string first = "HTTP/1.1 200 OK\r\n";
        first += "Content-Type: text/html\r\n";
        first += "Content-Length: " + sizo + "\r\n\r\n";
        buffers.insert(0, first);
        bete += first.size();
        sencgi = 0;
    }
    if (bete <= 0)
    {
        error(filis);
        if (fileDescriptor != -1)
        {close(fileDescriptor);fileDescriptor = -1;}
        status = -1;
        return;
    }
    size_t sendbyte = send(clientSocket, buffers.c_str(), bete, 0);
    if (sendbyte <= 0)
    {
        error(filis);
        if (fileDescriptor != -1)
        {close(fileDescriptor);fileDescriptor = -1;}
        status = -1;
        return;
    }
}

void Client::handle_cgi(std::string file, std::string filis, std::string exten)
{
    if (chekcgi == 0)
    {
        chekcgi = 1;
        addingenv(file, exten);
        if (status != 0)
            return;
        pathvalid();
        pid = fork();
        start_time = clock();
        if (pid < 0)
        {
            error(filis);
            status = 500;
            return;
        }
        if (pid == 0)
        {
            childprocess(filis);
        }
    }
    if (chekcgi == 1)
    {
        parentprocess(filis);
        if (flagcgi == 1)
        {
            sendcgiresponce(filis);
        }
    }
}

void Client::freeall()
{
    if (envp)
    {
        for (int j = 0; j <= hold; ++j)
        {
            if (envp[j])
            delete[] envp[j];
            else
                break;
        }        
    }

    if (envp)
        delete[] envp;
    if (av)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (av[i])
                delete[] av[i];
            else
                break;
        }        
    }

    if (av)
        delete[] av;
}