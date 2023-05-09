#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Result.h"

using namespace std;

class Engine
{
private:
    unordered_map<string, vector<string>> *graph;
    unordered_map<string, vector<string>> *keywords;
    unordered_map<string, int> *impressions;
    unordered_map<string, int> *clicks;
    unordered_map<string, float> pageRanks;

public:
    Engine(unordered_map<string, vector<string>> *graph, unordered_map<string, vector<string>> *keywords, unordered_map<string, int> *impressions, unordered_map<string, int> *clicks);
    ~Engine();

    vector<Result> search(string query);
    void increment_impressions(string url);
    void increment_clicks(string url);
    float calculatePageRank(string url);
    float calculateCTR(string url);
    vector<string> getKeyWordsinURL(string url);

};

#endif