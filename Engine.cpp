#include "Engine.h"

Engine::Engine(unordered_map<string, vector<string>> *graph, unordered_map<string, vector<string>> *keywords, unordered_map<string, int> *impressions, unordered_map<string, int> *clicks)
{
    this->graph = graph;
    this->keywords = keywords;
    this->impressions = impressions;
    this->clicks = clicks;
}

Engine::~Engine()
{
}