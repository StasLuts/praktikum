#include <algorithm>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <set>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine()
{
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber()
{
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text)
{
    vector<string> words;
    string word;
    for (const char c : text)
    {
        if (c == ' ')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }
        else
        {
            word += c;
        }
    }
    if (!word.empty())
    {
        words.push_back(word);
    }
    return words;
}

struct Document
{
    Document() = default;

    Document(int id, double relevance, int rating)
    :id(id), relevance(relevance), rating(rating)
    {

    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings)
{
    set<string> non_empty_strings;
    for (const string& str : strings)
    {
        if (!str.empty())
        {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

enum class DocumentStatus
{
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:

    inline static constexpr int INVALID_DOCUMENT_ID = -1;

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
    {
        for(const string& word : stop_words)
        {
            if(!IsValidWord(word))
            {
                throw invalid_argument("the string contains special characters!"s);
            }
        }
        stop_words_ = MakeUniqueNonEmptyStrings(stop_words);
    }

    explicit SearchServer(const string& stop_words_text)
    :SearchServer(SplitIntoWords(stop_words_text))
    {
        
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings)
    {
        if ((document_id < 0) || (documents_.count(document_id) > 0) || !IsValidWord(document))
        {
            throw invalid_argument("invalid_argument"s);
        }
        vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words)
        {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        document_ids_.push_back(document_id);
    }

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const
    {
        if(!CorrectUseDashes(raw_query) || !IsValidWord(raw_query))
        {
            throw invalid_argument("invalid_argument"s);
        }
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, document_predicate);
        sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs)
        {
            if (abs(lhs.relevance - rhs.relevance) < 1e-6)
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

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const
    {
        return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating)
        {
            return document_status == status;
        });
    }

    vector<Document> FindTopDocuments(const string& raw_query) const
    {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int GetDocumentCount() const
    {
        return documents_.size();
    }

    int GetDocumentId(int index) const
    {
        if (index >= 0 && index < GetDocumentCount())
        {
            return document_ids_.at(index);
        }
        throw out_of_range("out_of_range"s);
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const
    {
        if(!CorrectUseDashes(raw_query) || !IsValidWord(raw_query))
        {
            throw invalid_argument("invalid_argument"s);
        }
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return {matched_words, documents_.at(document_id).status};
    }

private:

    struct DocumentData
    {
        int rating;
        DocumentStatus status;
    };

    struct QueryWord
    {
        string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query
    {
        set<string> plus_words;
        set<string> minus_words;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    vector<int> document_ids_;

    bool CorrectUseDashes(const string& query) const
    {
        bool correct = true;
        for(int i = 0; static_cast<size_t>(i) < query.length(); i++)
        {
            if(query[i] == '-' && (query[i + 1] == ' ' || query[i + 1] == '-' || static_cast<size_t>(i+1) == query.length()))
            {
                return false;
            }
        }
        return correct;
    }

    template<typename Container, typename Unit>
    bool CountIt(const Container& container, const Unit& unit) const
    {
        return (container.count(unit));
    }

    bool IsStopWord(const string& word) const
    {
        return stop_words_.count(word) > 0;
    }

    static bool IsValidWord(const string& word)
    {
        return none_of(word.begin(), word.end(), [](char c)
        {
            return c >= '\0' && c < ' ';
        });
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const
    {
        vector<string> words;
        for (const string& word : SplitIntoWords(text))
        {
            if (!IsStopWord(word))
            {
                words.push_back(word);
            }
        }
        return words;
    }

    static int ComputeAverageRating(const vector<int>& ratings)
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

    QueryWord ParseQueryWord(string text) const
    {
        bool is_minus = false;
        if (text[0] == '-')
        {
            is_minus = true;
            text = text.substr(1);
        }
        return {text, is_minus, IsStopWord(text)};
    }

    Query ParseQuery(const string& text) const
    {
        Query query;
        for (const string& word : SplitIntoWords(text))
        {
            const QueryWord query_word = ParseQueryWord(word);
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

    double ComputeWordInverseDocumentFreq(const string& word) const
    {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const
    {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words)
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

        for (const string& word : query.minus_words)
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

        vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance)
        {
            matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }
};


template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file, const string& func, unsigned line, const string& hint)
{
    if (t != u)
    {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty())
        {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line, const string& hint)
{
    if (!value)
    {
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty())
        {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

template <typename T>
void RunTestImpl(const T& t)
{
    t();
}

#define RUN_TEST(func) RunTestImpl((func));

SearchServer GetSearchServer()
{
    SearchServer server("и в на"s);
    server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
    server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
    server.AddDocument(3, "ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});
    return server;
}

void TestAddDocument()
{
    SearchServer server(""s);
    server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, {8, -3});
    const auto found_docs = server.FindTopDocuments("модный"s);
    ASSERT(found_docs.size() == 1);
    const Document& doc0 = found_docs[0];
    ASSERT_EQUAL(doc0.id, 0);
}

void TestExcludeStopWordsFromAddedDocumentContent()
{
    SearchServer server("in the"s);
    server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, {1, 2, 3});
    ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
}

void TestMinusWords()
{
    SearchServer server(""s);
    server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, {1, 2, 3});
    ASSERT(server.FindTopDocuments("in -cat"s).empty());
}

void TestFindToRelevance()
{
    SearchServer server = GetSearchServer();
    const auto found_docs = server.FindTopDocuments("пёс пушистый ухоженный кот"s);
    const bool etalon = (found_docs[0].relevance > found_docs[1].relevance && found_docs[1].relevance > found_docs[2].relevance);
    ASSERT(etalon);
}

void TestDocumentRating()
{
    SearchServer server(""s);
    server.AddDocument(0, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
    const auto found_docs = server.FindTopDocuments("глаза"s);
    const Document& doc0 = found_docs[0];
    ASSERT_EQUAL(doc0.rating, -1);
}

void TestCustomLambdaSorting()
{
    SearchServer server = GetSearchServer();
    const auto predicate = [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; };
    const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, predicate);
    ASSERT_EQUAL(found_docs[0].id, 0);
    ASSERT_EQUAL(found_docs[1].id, 2);
}

void TestFindToStatus()
{
    SearchServer server = GetSearchServer();
    const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::BANNED); 
    const auto etalonId = 3;
    ASSERT_EQUAL(found_docs[0].id, etalonId);
}

void TestRelevanceCount()
{
    SearchServer server = GetSearchServer();
    const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s);
    ASSERT(fabs(found_docs[0].relevance - found_docs[0].relevance) < numeric_limits<double>::epsilon());
}

void TestSearchServer()
{
    RUN_TEST(TestAddDocument);
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestMinusWords);
    RUN_TEST(TestFindToRelevance);
    RUN_TEST(TestDocumentRating);
    RUN_TEST(TestCustomLambdaSorting);
    RUN_TEST(TestFindToStatus);
    RUN_TEST(TestRelevanceCount);
}

void PrintDocument(const Document& document)
{
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating
         << " }"s << endl;
}

int main()
{
    TestSearchServer();
    cout << "Search server testing finished"s << endl;

    try
    {
        SearchServer search_server = GetSearchServer();
        cout << "ACTUAL by default:"s << endl;
        for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s))
        {
            PrintDocument(document);
        }

        cout << "BANNED:"s << endl;
        for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::BANNED))
        {
            PrintDocument(document);
        }

        cout << "Even ids:"s << endl;
        const auto predicate = [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; };
        for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, predicate))
        {
            PrintDocument(document);
        }
    }
    catch(const invalid_argument& e)
    {
        cerr << e.what() << '\n';
    }
    catch(const out_of_range& e)
    {
        cerr << e.what() << '\n';
    }

    return 0;
}