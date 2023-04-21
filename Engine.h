#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Engine
{
private:
    unordered_map<string, vector<string>> *graph;
    unordered_map<string, vector<string>> *keywords;
    unordered_map<string, int> *impressions;
    unordered_map<string, int> *clicks;
public:
    Engine(unordered_map<string, vector<string>> *graph, unordered_map<string, vector<string>> *keywords, unordered_map<string, int> *impressions, unordered_map<string, int> *clicks);
    ~Engine();
};

#endif