#pragma once

#include "search_server.h"

#include <cstdint>
#include <deque>

class RequestQueue
{
public:

    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate)
    {
        const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(result.size());
        return result;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:

    struct QueryResult
    {
        int result_;
        uint64_t timestamp;
    };

    const SearchServer& search_server_;
    int no_result_request_;
    uint64_t current_time_;
    std::deque<QueryResult> requests_;
    const static int sec_in_day_ = 1440;
    
    void AddRequest(int results);
};