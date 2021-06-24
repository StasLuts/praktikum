#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server)
:search_server_(search_server), no_result_request_(0), current_time_(0)
{

}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status)
{
    const auto result = search_server_.FindTopDocuments(raw_query, status);
    AddRequest(result.size());
    return result;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query)
{
    const auto result = search_server_.FindTopDocuments(raw_query);
    AddRequest(result.size());
    return result;
}

int RequestQueue::GetNoResultRequests() const
{
    return no_result_request_;
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