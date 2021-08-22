#include "search_server.h"

using namespace std;

void SearchServer::AddDocument(int document_id, string_view document, DocumentStatus status, const vector<int>& ratings)
{
    if ((document_id < 0) || (documents_.count(document_id)) || !IsValidWord(document))
    {
        throw invalid_argument("invalid_argument"s);
    }
    const auto [it, inserted] = documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status, string(document) });
    const auto words = SplitIntoWordsNoStop(it->second.text);

    const double inv_word_count = 1.0 / words.size();
    for (const string_view word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
        id_to_document_freqs_[document_id][word] += inv_word_count;
    }
    document_ids_.push_back(document_id);
}

vector<Document> SearchServer::FindTopDocuments(string_view raw_query, DocumentStatus status) const
{
    return FindTopDocuments(execution::seq, raw_query, [status](int document_id, DocumentStatus document_status, int rating)
        {
            return document_status == status;
        });
}

vector<Document> SearchServer::FindTopDocuments(execution::sequenced_policy, string_view raw_query, DocumentStatus status) const
{
    return FindTopDocuments(execution::seq, raw_query, [status](int document_id, DocumentStatus document_status, int rating)
        {
            return document_status == status;
        });
}

vector<Document> SearchServer::FindTopDocuments(execution::parallel_policy, string_view raw_query, DocumentStatus status) const
{
    return FindTopDocuments(execution::par, raw_query, [status](int document_id, DocumentStatus document_status, int rating)
        {
            return document_status == status;
        });
}

vector<Document> SearchServer::FindTopDocuments(string_view raw_query) const
{
    return FindTopDocuments(execution::seq, raw_query, DocumentStatus::ACTUAL);
}

vector<Document> SearchServer::FindTopDocuments(execution::sequenced_policy, string_view raw_query) const
{
    return FindTopDocuments(execution::seq, raw_query, DocumentStatus::ACTUAL);
}

vector<Document> SearchServer::FindTopDocuments(execution::parallel_policy, string_view raw_query) const
{
    return FindTopDocuments(execution::par, raw_query, DocumentStatus::ACTUAL);
}

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const
{
    static const map<string_view, double>null;
    if (id_to_document_freqs_.count(document_id))
    {
        return id_to_document_freqs_.at(document_id);
    }
    return null;
}

void SearchServer::RemoveDocument(int document_id)
{
    for (auto [word, freq] : id_to_document_freqs_[document_id])
    {
        word_to_document_freqs_[word].erase(document_id);
    }
    id_to_document_freqs_.erase(document_id);
    documents_.erase(document_id);
    document_ids_.erase(find(document_ids_.begin(), document_ids_.end(), document_id));
}

void SearchServer::RemoveDocument(std::execution::sequenced_policy, int document_id)
{
    RemoveDocument(document_id);
}

void SearchServer::RemoveDocument(execution::parallel_policy, int document_id)
{
    for (auto [word, freq] : id_to_document_freqs_[document_id])
    {
        word_to_document_freqs_[word].erase(document_id);
    }
    id_to_document_freqs_.erase(document_id);
    documents_.erase(document_id);
    document_ids_.erase(find(document_ids_.begin(), document_ids_.end(), document_id));
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(string_view raw_query, int document_id) const
{
    if (!CorrectUseDashes(raw_query) || !IsValidWord(raw_query))
    {
        throw invalid_argument("invalid_argument"s);
    }
    const auto query = ParseQuery(raw_query);
    vector<string_view> matched_words;
    for (string_view word : query.plus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id))
        {
            matched_words.push_back(word);
        }
    }
    for (string_view word : query.minus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id))
        {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(execution::sequenced_policy, string_view raw_query, int document_id) const
{
    return MatchDocument(raw_query, document_id);
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(execution::parallel_policy, string_view raw_query, int document_id) const
{
    if (!CorrectUseDashes(raw_query) || !IsValidWord(raw_query))
    {
        throw invalid_argument("invalid_argument"s);
    }
    const auto query = ParseQuery(raw_query);
    vector<string_view> matched_words;
    for (const string_view word : query.plus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id))
        {
            matched_words.push_back(word);
        }
    }
    for (const string_view word : query.minus_words)
    {
        if (!CountIt(word_to_document_freqs_, word))
        {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id))
        {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

bool SearchServer::CorrectUseDashes(string_view query) const
{
    bool correct = true;
    if (query.length() > 1)
    {
        for (int i = 1; static_cast<size_t>(i) < query.length(); i++)
        {
            if (query[i - 1] == '-' && (query[i] == ' ' || query[i] == '-'))
            {
                return false;
            }
            else if (query[i] == '-' && query[i] == query.back())
            {
                return false;
            }
        }
    }
    else
    {
        if (query == "-")
        {
            return false;
        }
    }

    return correct;
}

bool SearchServer::IsValidWord(string_view word)
{
    return none_of(word.begin(), word.end(), [](char c)
        {
            return c >= '\0' && c < ' ';
        });
}

vector<string_view> SearchServer::SplitIntoWordsNoStop(string_view text) const
{
    vector<string_view> words;
    for (string_view word : SplitIntoWords(text))
    {
        if (!IsStopWord(word))
        {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings)
{
    if (ratings.empty())
    {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings)
    {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(string_view text) const
{
    bool is_minus = false;
    if (text[0] == '-')
    {
        is_minus = true;
        text = text.substr(1);
    }
    return { text, is_minus, IsStopWord(text) };
}

SearchServer::Query SearchServer::ParseQuery(string_view text) const
{
    Query query;
    for (const string_view word : SplitIntoWords(text))
    {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop)
        {
            if (query_word.is_minus)
            {
                query.minus_words.insert(query_word.data);
            }
            else
            {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

double SearchServer::ComputeWordInverseDocumentFreq(string_view word) const
{
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
