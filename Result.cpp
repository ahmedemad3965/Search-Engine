
#include "Result.h"

using namespace std;

Result::Result(string url, float pageRank, float ctr, vector<string> keywords)
{
    this->url = url;
    this->ctr = ctr;
    this->pageRank = pageRank;
    this->keywords = keywords;
}

Result::~Result()
{
}

string Result::get_url()
{
    return this->url;
}

float Result::get_ctr()
{
    return this->ctr;
}

float Result::get_pageRank()
{
    return this->pageRank;
}

vector<string> Result::get_keywords()
{
    return this->keywords;
}
