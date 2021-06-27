#include "request_queue.h"

using namespace std;

vector<Document> RequestQueue::SetterToAddRequest(const vector<Document>& result)
{
    AddRequest(result.size());
    return result;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status)
{
    const auto result = search_server_.FindTopDocuments(raw_query, status);
    return SetterToAddRequest(result);
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query)
{
    const auto result = search_server_.FindTopDocuments(raw_query);
    return SetterToAddRequest(result);
}

void RequestQueue::AddRequest(int results)
{
    ++current_time_;
    while (!requests_.empty() && sec_in_day_ <= current_time_ - requests_.front().timestamp)
    {
        if(requests_.front().result_ == 0)
        {
            --no_result_request_;
        }
        requests_.pop_front();
    }

    requests_.push_back({results, current_time_});
    if (results == 0) {
        ++no_result_request_;
    }
}