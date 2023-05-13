#include "ConsoleMode.h"

#include <iostream>
#include <sstream>

using namespace std;

ConsoleMode::ConsoleMode(Engine *engine)
{
    this->engine = engine;
}

ConsoleMode::~ConsoleMode()
{
}

vector<Result> results;

void show_results()
{
    cout << "\nSearch results:\n\n\n";

    if (results.size() == 0)
    {
        cout << "No results found :(" << endl;
        return;
    }

    for (int i = 0; i < results.size(); i++)
    {
        cout << i + 1 << ". " << results[i].get_url() << endl;
    }
}

void ConsoleMode::run()
{
    string query;

    perform_search();

    bool just_opened = false;

    while (true)
    {
        if (!just_opened)
        {
            cout << "\n1. Open a webpage\n2. Perform a new search\n3. Exit\n\n";
            cout << "Choose an option: ";
        }
        else
        {
            cout << "\n1. Return to search results\n2. Perform a new search\n3. Exit\n\n";
            cout << "Choose an option: ";
        }

        int option;
        cin >> option;

        switch (option)
        {
        case 1:
            if (!just_opened)
            {
                cout << "Enter webpage number: ";
                int webpageNumber;
                cin >> webpageNumber;
                if (webpageNumber > results.size() || webpageNumber < 1)
                {
                    cout << "\n\nInvalid webpage number\n\n";
                    break;
                }
                open_browser(results[webpageNumber - 1].get_url());
                just_opened = true;
                break;
            }
            else
            {
                show_results();
                just_opened = false;
                break;
            }
        case 2:
            just_opened = false;
            perform_search();
            break;
        case 3:
            return;
        default:
            cout << "\n\nInvalid option, try again.\n\n";
            break;
        }
    }
}

void ConsoleMode::perform_search()
{
    cout << "Enter search query: ";
    string query;
    cin.ignore();
    getline(cin, query);

    results = engine->search(query);

    show_results();
}

void ConsoleMode::open_browser(const string &url)
{
    cout << "\n\nOpening " << url << endl << endl;
    string command;

    string url_copy = "https://" + url;

#ifdef _WIN32
    // Windows command
    command = "start " + url_copy;
#elif _WIN64
    command = "start " + url_copy;
#elif __APPLE__
    // macOS command
    command = "open " + url_copy;
#elif __MACH__
    command = "open " + url_copy;
#else
    // Linux command
    command = "xdg-open " + url_copy;
#endif

    int result = system(command.c_str());
    if (result != 0)
    {
        cerr << "Failed to open browser" << endl;
    }
}
