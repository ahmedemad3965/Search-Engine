#include "Engine.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>

Engine::Engine(unordered_map<string, vector<string>> *graph, unordered_map<string, vector<string>> *keywords, unordered_map<string, int> *impressions, unordered_map<string, int> *clicks)
{
    this->graph = graph;
    this->keywords = keywords;
    this->impressions = impressions;
    this->clicks = clicks;

    this->pageRanks = unordered_map<string, float>();
}

Engine::~Engine()
{
    delete this->graph;
    delete this->keywords;
    delete this->impressions;
    delete this->clicks;
}

std::string url_decode(const std::string &str)
{
    std::ostringstream oss;
    for (std::string::size_type i = 0; i < str.length(); ++i)
    {
        if (str[i] == '+')
        {
            oss << ' ';
        }
        else if (str[i] == '%' && i + 2 < str.length() &&
                 isxdigit(str[i + 1]) && isxdigit(str[i + 2]))
        {
            char c = std::stoi(str.substr(i + 1, 2), nullptr, 16);
            oss << c;
            i += 2;
        }
        else
        {
            oss << str[i];
        }
    }
    return oss.str();
}

vector<string> splitQuery(const string &query, char delimiter)
{
    // uncode the formurlencoded query

    string decodedQuery = url_decode(query);

    stringstream ss(decodedQuery);
    string word;
    vector<string> words;

    while (getline(ss, word, delimiter))
    {
        words.push_back(word);
    }

    return words;
}

bool containsKeywords(const string &url, const vector<string> &keywords)
{
    for (const auto &keyword : keywords)
    {
        if (url.find(keyword) == string::npos)
        {
            return false;
        }
    }

    return true;
}

float Engine::calculatePageRank(string url)
{
    if (pageRanks.find(url) != pageRanks.end())
    {
        return pageRanks[url];
    }

    float dampingFactor = 0.85f;
    float initialPageRank = 1.0f / this->graph->size();
    float minDifference = 0.001f;
    bool hasConverged = false;

    // Initialize all page ranks to an equal value
    for (const auto &pair : *this->graph)
    {
        pageRanks[pair.first] = initialPageRank;
    }

    while (!hasConverged)
    {
        hasConverged = true;
        unordered_map<string, float> newPageRanks;

        // Compute the new PageRank for each page
        for (const auto &pair : *this->graph)
        {
            const string &url = pair.first;
            const vector<string> &outboundLinks = pair.second;

            float sum = 0.0f;
            for (const auto &outboundLink : outboundLinks)
            {
                sum += pageRanks[outboundLink] / this->graph->at(outboundLink).size();
            }

            float newPageRank = (1 - dampingFactor) + dampingFactor * sum;
            newPageRanks[url] = newPageRank;

            // Check if the PageRank has converged
            if (abs(newPageRank - pageRanks[url]) > minDifference)
            {
                hasConverged = false;
            }
        }

        pageRanks = newPageRanks;
    }

    return pageRanks[url];
}

float Engine::calculateCTR(string url)
{
    return (float)(*this->clicks)[url] / (float)(*this->impressions)[url];
}

vector<string> Engine::getKeyWordsinURL(string url)
{
    vector<string> keywords;
    for (const auto &pair : *this->keywords)
    {
        if (find(pair.second.begin(), pair.second.end(), url) != pair.second.end())
        {
            keywords.push_back(pair.first);
        }
    }

    return keywords;
}

vector<Result> Engine::search(string query)
{
    cout << "Searching for: " << query << endl;
    vector<Result> results;
    vector<string> words = splitQuery(query, ' ');

    for (size_t i = 0; i < words.size(); ++i)
    {
        if (words[i] != "AND" && words[i] != "OR")
            transform(words[i].begin(), words[i].end(), words[i].begin(), ::tolower);

        words[i].erase(remove_if(words[i].begin(), words[i].end(), [](char c)
                                 { return !isalnum(c); }),
                       words[i].end());

        if (words[i] == "AND")
        {
            if (i > 0 && i < words.size() - 1)
            {
                vector<string> urlsForKeyword1 = (*this->keywords)[words[i - 1]];
                vector<string> urlsForKeyword2 = (*this->keywords)[words[i + 1]];
                for (const auto &url : urlsForKeyword1)
                {
                    if (find(urlsForKeyword2.begin(), urlsForKeyword2.end(), url) != urlsForKeyword2.end())
                    {
                        Result result(url, calculatePageRank(url), calculateCTR(url), getKeyWordsinURL(url));
                        results.push_back(result);
                    }
                }
                ++i;
            }
        }
        else if (words[i] == "OR")
        {
            if (i > 0 && i < words.size() - 1)
            {
                vector<string> urlsForKeyword1 = (*this->keywords)[words[i - 1]];
                vector<string> urlsForKeyword2 = (*this->keywords)[words[i + 1]];
                for (const auto &url : urlsForKeyword1)
                {
                    Result result(url, calculatePageRank(url), calculateCTR(url), getKeyWordsinURL(url));
                    results.push_back(result);
                }
                for (const auto &url : urlsForKeyword2)
                {
                    Result result(url, calculatePageRank(url), calculateCTR(url), getKeyWordsinURL(url));
                    results.push_back(result);
                }
                ++i;
            }
        }
        else if (words[i][0] == '"' && words[i][words[i].size() - 1] == '"')
        {
            string phrase = words[i].substr(1, words[i].size() - 2);

            vector<string> phraseWords = splitQuery(phrase, ' ');
            for (const auto &pair : *this->keywords)
            {
                if (pair.second.size() > 0)
                {
                    vector<string> urlsForKeyword = pair.second;

                    for (const auto &url : urlsForKeyword)
                    {
                        if (containsKeywords(url, phraseWords))
                        {
                            Result result(url, calculatePageRank(url), calculateCTR(url), getKeyWordsinURL(url));
                            results.push_back(result);
                        }
                    }
                }
            }
        }
        else
        {
            // if there is AND or OR  after the word, skip it
            if (i < words.size() - 1 && (words[i + 1] == "AND" || words[i + 1] == "OR"))
                continue;
            
            vector<string> urlsForKeyword = (*this->keywords)[words[i]];
            cout << "Keyword: " << words[i] << "  .   " << urlsForKeyword.size() << endl;
            for (const auto &url : urlsForKeyword)
            {
                Result result(url, calculatePageRank(url), calculateCTR(url), getKeyWordsinURL(url));
                results.push_back(result);
            }
        }
    }

    sort(results.begin(), results.end());

    cout << "Results:" << endl;
    for (auto &result : results)
    {
        cout << result.get_url() << endl;
    }

    return results;
}

void Engine::increment_impressions(string url)
{
    if (impressions->find(url) == impressions->end())
    {
        (*impressions)[url] = 1;
    }
    else
    {
        (*impressions)[url] += 1;
    }

    // update the file
    ofstream file;
    file.open("impressions.txt");
    for (auto it = impressions->begin(); it != impressions->end(); ++it)
    {
        file << it->first << ", " << it->second << endl;
    }
    file.close();
}

void Engine::increment_clicks(string url)
{
    if (clicks->find(url) == clicks->end())
    {
        (*clicks)[url] = 1;
    }
    else
    {
        (*clicks)[url] += 1;
    }

    // update the file
    ofstream file;
    file.open("clicks.txt");
    for (auto it = clicks->begin(); it != clicks->end(); ++it)
    {
        file << it->first << ", " << it->second << endl;
    }
    file.close();
}