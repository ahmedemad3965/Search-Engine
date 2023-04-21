#ifndef BROWSERMODE_H
#define BROWSERMODE_H

#include "Engine.h"

using namespace std;

class BrowserMode
{
private:
    Engine *engine;
public:
    BrowserMode(Engine *engine);
    ~BrowserMode();
    void run();
    string get_content_type(const string& path);
    void serve_file(int client_fd, const string& path);
    void serve_results(int client_fd, vector<string> results);
    int create_server_socket(int port);
    void listen_for_requests(int server_fd);
    void open_browser(const string& url);
};

#endif