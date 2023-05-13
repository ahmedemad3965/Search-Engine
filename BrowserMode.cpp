#include "BrowserMode.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

BrowserMode::BrowserMode(Engine *engine)
{
    this->engine = engine;
}

BrowserMode::~BrowserMode()
{
}

void BrowserMode::run()
{
    cout << "Running Browser Mode on http://localhost:8080/ " << endl;

    int server_fd = create_server_socket(8080);

    if (server_fd == -1)
    {
        cerr << "Failed to create server socket" << endl;
        return;
    }

    open_browser("http://localhost:8080/");

    listen_for_requests(server_fd);

    close(server_fd);
}

int BrowserMode::create_server_socket(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        cerr << "Failed to create socket" << endl;
        return -1;
    }

    // Bind the socket to a port
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (::bind(server_fd, (struct sockaddr *)&address, sizeof address) < 0)
    {
        cerr << "Failed to bind socket" << endl;
        return -1;
    }

    if (listen(server_fd, 10) < 0)
    {
        cerr << "Failed to listen on socket" << endl;
        return -1;
    }

    return server_fd;
}

void BrowserMode::listen_for_requests(int server_fd)
{
    while (true)
    {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            cerr << "Failed to accept incoming connection" << endl;
            continue;
        }

        // Read incoming HTTP request
        stringstream request_stream;
        char buffer[4096];
        int bytes_read;
        do
        {
            bytes_read = read(client_fd, buffer, sizeof(buffer));
            if (bytes_read > 0)
            {
                request_stream.write(buffer, bytes_read);
            }
        } while (bytes_read > 0 && request_stream.str().find("\r\n\r\n") == string::npos);

        string request_str = request_stream.str();
        string method = request_str.substr(0, request_str.find(' '));
        string path = request_str.substr(request_str.find(' ') + 1, request_str.find(' ', request_str.find(' ') + 1) - request_str.find(' ') - 1);
        string headers = request_str.substr(request_str.find("\r\n") + 2);

        size_t content_start = request_str.find("\r\n\r\n") + 4;
        string content = request_str.substr(content_start);

        // cout << "aa Received " << method << " request for " << path << " with content:" << endl;
        // cout << content << endl;

        if (method == "GET")
        {
            if (path.find("search") != string::npos)
            {
                // /search?q=
                string query = path.substr(path.find("q=") + 2);

                vector<Result> results = engine->search(query);
                serve_results(client_fd, results);
            }
            if (path == "/")
            {
                serve_file(client_fd, "public/index.html");
            }
            else
            {
                serve_file(client_fd, "public" + path);
            }
        }
        else
        {
            string response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello World!";
            write(client_fd, response.c_str(), response.size());
        }

        close(client_fd);
    }
}

string getKeywords(vector<string> keywords)
{
    string keywords_str = "";
    for (int i = 0; i < keywords.size(); i++)
    {
        keywords_str += keywords[i];
        if (i != keywords.size() - 1)
        {
            keywords_str += ", ";
        }
    }
    return keywords_str;
}

string make_url_external(string url)
{
    if (url.find("http://") == string::npos)
    {
        return "http://" + url;
    }
    return url;
}

void BrowserMode::serve_results(int client_fd, vector<Result> results)
{
    string path = "public/results.html";
    ifstream file(path, ios::binary | ios::ate);
    if (!file.is_open())
    {
        string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n";
        write(client_fd, response.c_str(), response.size());
        return;
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    string content(size, ' ');
    file.read(&content[0], size);

    string results_str;

    for (int i = 0; i < results.size(); i++)
    {
        results_str += "<li class=\"result\"><a href=\"" + make_url_external(results[i].get_url()) + "\">" + results[i].get_url() + "</a><br><span class=\"keywords\">" + getKeywords(results[i].get_keywords()) + "</span></li>";
    }

    if(results.size() == 0){
        results_str += "<div class=\"no-result\">No results found</div>";
    }

    size_t pos = content.find("{{results}}");
    if (pos != string::npos)
    {
        content.replace(pos, string("{{results}}").size(), results_str);
    }

    // send the modified file to the client
    string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(content.size()) + "\r\n\r\n";
    write(client_fd, response.c_str(), response.size());
    write(client_fd, content.c_str(), content.size());

    file.close();
}

void BrowserMode::serve_file(int client_fd, const string &path)
{
    ifstream file(path, ios::binary | ios::ate);
    if (!file.is_open())
    {
        string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n";
        write(client_fd, response.c_str(), response.size());
        return;
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    string response = "HTTP/1.1 200 OK\r\nContent-Type: " + get_content_type(path) + "\r\nContent-Length: " + to_string(size) + "\r\n\r\n";
    write(client_fd, response.c_str(), response.size());

    char buffer[4096];
    do
    {
        file.read(buffer, sizeof(buffer));
        write(client_fd, buffer, file.gcount());
    } while (file.gcount() > 0);

    file.close();
}

string BrowserMode::get_content_type(const string &path)
{
    string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "html")
    {
        return "text/html";
    }
    else if (extension == "css")
    {
        return "text/css";
    }
    else if (extension == "js")
    {
        return "text/javascript";
    }
    else
    {
        return "application/octet-stream";
    }
}

void BrowserMode::open_browser(const string &url)
{
    string command;

#ifdef _WIN32
    // Windows command
    command = "start " + url;
#elif _WIN64
    command = "start " + url;
#elif __APPLE__
    // macOS command
    command = "open " + url;
#elif __MACH__
    command = "open " + url;
#else
    // Linux command
    command = "xdg-open " + url;
#endif

    int result = system(command.c_str());
    if (result != 0)
    {
        cerr << "Failed to open browser" << endl;
    }
}