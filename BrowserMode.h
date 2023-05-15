#ifndef BROWSERMODE_H
#define BROWSERMODE_H

#define _HAS_STD_BYTE 0


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET SocketType;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
typedef int SocketType;
#endif



#include "Engine.h"

// using namespace std;

class BrowserMode
{
private:
    Engine *engine;
public:
    BrowserMode(Engine *engine);
    ~BrowserMode();
    void run();
    string get_content_type(const string& path);
    void serve_file(SocketType client_fd, const string& path);
    void serve_results(SocketType client_fd, vector<Result> results);
    SocketType create_server_socket(int port);
    void listen_for_requests(SocketType server_fd);
    void open_browser(const string& url);
};

#endif