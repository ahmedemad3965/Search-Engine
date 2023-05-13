#ifndef CONSOLEMODE_H
#define CONSOLEMODE_H

#include "Engine.h"

using namespace std;

class ConsoleMode
{
private:
    Engine *engine;
public:
    ConsoleMode(Engine *engine);
    ~ConsoleMode();
    void perform_search();
    void run();
    void open_browser(const string& url);
};

#endif