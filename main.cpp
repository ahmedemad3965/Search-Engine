#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "Engine.h"
#include "ConsoleMode.h"
#include "BrowserMode.h"

using namespace std;

unordered_map<string, vector<string>> *graph;
unordered_map<string, vector<string>> *keywords;
unordered_map<string, int> *impressions;
unordered_map<string, int> *clicks;

void build_webgraph(string filename)
{
    ifstream webgraph_file(filename);

    if (webgraph_file.is_open())
    {
        string line;
        while (getline(webgraph_file, line))
        {
            stringstream ss(line);
            string url1, url2;
            getline(ss, url1, ',');
            getline(ss, url2, ',');
            (*graph)[url1].push_back(url2);
        }
        webgraph_file.close();
    }
    else
    {
        cerr << "Unable to open file" << endl;
    }
}

void add_keywords(string filename)
{
    ifstream keywords_file(filename);

    if (keywords_file.is_open())
    {
        string line;
        while (getline(keywords_file, line))
        {
            stringstream ss(line);
            string url;
            getline(ss, url, ',');
            string keyword;
            while (getline(ss, keyword, ','))
            {
                (*keywords)[keyword].push_back(url);
            }
        }
        keywords_file.close();
    }
    else
    {
        cerr << "Unable to open file" << endl;
    }
}

void add_impressions_count(string filename)
{
    ifstream impressions_file(filename);

    if (impressions_file.is_open())
    {
        string line;
        while (getline(impressions_file, line))
        {
            stringstream ss(line);
            string url;
            getline(ss, url, ',');
            int count;
            ss >> count;
            (*impressions)[url] = count;
        }
        impressions_file.close();
    }
    else
    {
        cerr << "Unable to open file" << endl;
    }
}

void add_clicks_count(string filename)
{
    ifstream clicks_file(filename);

    if (clicks_file.is_open())
    {
        string line;
        while (getline(clicks_file, line))
        {
            stringstream ss(line);
            string url;
            getline(ss, url, ',');
            int count;
            ss >> count;
            (*clicks)[url] = count;
        }
        clicks_file.close();
    }
    else
    {
        cerr << "Unable to open file" << endl;
    }
}

int main()
{

    cout << endl
         << endl
         << "=============================================================" << endl;
    cout << "Welcome to Goologolo, the best AI Search Engine in the world!" << endl;
    cout << "=============================================================" << endl
         << endl
         << endl;

    graph = new unordered_map<string, vector<string>>;
    keywords = new unordered_map<string, vector<string>>;
    impressions = new unordered_map<string, int>;
    clicks = new unordered_map<string, int>;

    string webgraph_filename = "webgraph.txt";

    build_webgraph(webgraph_filename);

    string keywords_filename = "keywords.txt";

    add_keywords(keywords_filename);

    string impressions_filename = "impressions.txt";

    add_impressions_count(impressions_filename);

    string clicks_filename = "clicks.txt";
    add_clicks_count(clicks_filename);

    Engine *engine = new Engine(graph, keywords, impressions, clicks);

    cout << endl
         << endl
         << "=============================================================" << endl
         << endl
         << endl;
    cout << "Goologolo Engine Initialized, and Ready for Launch!" << endl
         << endl
         << endl;

    cout << "Do you want to start searching in Browser mode, or in Command Line mode?" << endl
         << endl;
    cout << "Enter 1 for Browser mode, or 2 for Command Line mode" << endl << endl;

    int mode;
    cin >> mode;

    if (mode == 1)
    {
        cout << "=============================================================" << endl
             << endl
             << endl;
        cout << "Welcome to Browser Mode!" << endl
             << endl
             << endl;

        BrowserMode browser_mode(engine);
        browser_mode.run();
    }
    else if (mode == 2)
    {
        cout << "=============================================================" << endl
             << endl
             << endl;
        cout << "Welcome to Console Mode!" << endl
             << endl
             << endl;

        // ConsoleMode console_mode(engine);
        // console_mode.run();
    }
    else
    {
        cerr << "Invalid mode, please run again and enter 1 or 2" << endl;
    }

    return 0;
}