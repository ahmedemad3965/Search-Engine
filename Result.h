// class for the result object

#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <vector>

using namespace std;

class Result
{
private:
    string url;
    float ctr;
    float pageRank;
    vector<string> keywords;

public:
    Result(string url, float pageRank, float ctr, vector<string> keywords);
    ~Result();

    string get_url();
    float get_ctr();
    float get_pageRank();
    vector<string> get_keywords();

    bool operator<(const Result &other) const
    {
        return (this->ctr * this->pageRank) > (other.ctr * other.pageRank);
    }

    bool operator>(const Result &other) const
    {
        return (this->ctr * this->pageRank) < (other.ctr * other.pageRank);
    }
};

#endif