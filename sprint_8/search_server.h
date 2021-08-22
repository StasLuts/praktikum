#pragma once

#include "string_processing.h"
#include "document.h"

#include <map>
#include <set>
#include <tuple>
#include <cmath>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <stdexcept>
#include <execution>

using namespace std::string_literals;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
public:

    inline static constexpr int INVALID_DOCUMENT_ID = -1;

    template <typename StringContainer>
    SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words
    {
        if (!std::all_of(stop_words_.begin(), stop_words_.end(), IsValidWord))
        {
            throw std::invalid_argument("Some of stop words are invalid"s);
        }
    }

    explicit SearchServer(const std::string& stop_words_text)
        :SearchServer(std::string_view(stop_words_text)){}

    explicit SearchServer(std::string_view stop_words_text)
        :SearchServer(SplitIntoWords(stop_words_text)){}

    void AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings);

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(std::execution::sequenced_policy, std::string_view raw_query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(std::execution::parallel_policy, std::string_view raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(std::execution::sequenced_policy, std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(std::execution::parallel_policy, std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query) const;

    std::vector<Document> FindTopDocuments(std::execution::sequenced_policy, std::string_view raw_query) const;

    std::vector<Document> FindTopDocuments(std::execution::parallel_policy, std::string_view raw_query) const;

    int GetDocumentCount() const noexcept
    {
        return documents_.size();
    }

    std::vector<int>::const_iterator begin() const
    {
        return document_ids_.cbegin();
    }

    std::vector<int>::const_iterator end() const
    {
        return document_ids_.cend();
    }

    const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;

    void RemoveDocument(int document_id);

    void RemoveDocument(std::execution::sequenced_policy, int document_id);

    void RemoveDocument(std::execution::parallel_policy, int document_id);

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::string_view raw_query, int document_id) const;

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::execution::sequenced_policy, std::string_view raw_query, int document_id) const;

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::execution::parallel_policy, std::string_view raw_query, int document_id) const;

private:

    struct DocumentData
    {
        int rating;
        DocumentStatus status;
        std::string text;
    };

    struct QueryWord
    {
        std::string_view data;
        bool is_minus;
        bool is_stop;
    };

    struct Query
    {
        std::set<std::string_view> plus_words;
        std::set<std::string_view> minus_words;
    };

    const std::set<std::string, std::less<>> stop_words_;
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string_view, double>> id_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::vector<int> document_ids_;

    bool CorrectUseDashes(std::string_view query) const;

    template<typename Container, typename Unit>
    bool CountIt(const Container& container, const Unit& unit) const;

    bool IsStopWord(std::string_view word) const
    {
        return stop_words_.count(word);
    }

    static bool IsValidWord(std::string_view word);

    std::vector<std::string_view> SplitIntoWordsNoStop(std::string_view text) const;

    static int ComputeAverageRating(const std::vector<int>& ratings);

    QueryWord ParseQueryWord(std::string_view text) const;

    Query ParseQuery(std::string_view text) const;

    double ComputeWordInverseDocumentFreq(std::string_view word) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(std::execution::sequenced_policy, const Query& query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(std::execution::parallel_policy, const Query& query, DocumentPredicate document_predicate) const;
};

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const
{
    return FindTopDocuments(std::execution::seq, raw_query, document_predicate);
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(std::execution::sequenced_policy, std::string_view raw_query, DocumentPredicate document_predicate) const
{
    if (!CorrectUseDashes(raw_query) || !IsValidWord(raw_query))
    {
        throw std::invalid_argument("invalid_argument"s);
    }
    const auto query = ParseQuery(raw_query);
    auto matched_documents = FindAllDocuments(std::execution::seq, query, document_predicate);
    sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs)
        {
            if (std::abs(lhs.relevance - rhs.relevance) < 1e-6)
            {
                return lhs.rating > rhs.rating;
            }
            else
            {
                return lhs.relevance > rhs.relevance;
            }
        });
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
    {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(std::execution::parallel_policy, std::string_view raw_query, DocumentPredicate document_predicate) const
{
    if (!CorrectUseDashes(raw_query) || !IsValidWord(raw_query))
    {
        throw std::invalid_argument("invalid_argument"s);
    }
    const auto query = ParseQuery(raw_query);
    auto matched_documents = FindAllDocuments(std::execution::par, query, document_predicate);
    sort(std::execution::par, matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs)
        {
            if (std::abs(lhs.relevance - rhs.relevance) < 1e-6)
            {
                return lhs.rating > rhs.rating;
            }
            else
            {
                return lhs.relevance > rhs.relevance;
            }
        });
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
    {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
}

template<typename Container, typename Unit>
inline bool SearchServer::CountIt(const Container& container, const Unit& unit) const
{
    return (container.count(unit));
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(std::execution::sequenced_policy, const Query& query, DocumentPredicate document_predicate) const
{
    std::map<int, double> document_to_relevance;
    for (std::string_view word : query.plus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word))
        {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating))
            {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }
    for (std::string_view word : query.minus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word))
        {
            document_to_relevance.erase(document_id);
        }
    }
    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance)
    {
        matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
    }
    return matched_documents;
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(std::execution::parallel_policy, const Query& query, DocumentPredicate document_predicate) const
{
    std::map<int, double> document_to_relevance;
    for (std::string_view word : query.plus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word))
        {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating))
            {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }
    for (std::string_view word : query.minus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word))
        {
            document_to_relevance.erase(document_id);
        }
    }
    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance)
    {
        matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
    }
    return matched_documents;
}