#include "search_server.h"
#include "process_queries.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <cassert>

using namespace std;

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
    server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
    server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
    return server;
}

void TestAddDocument()
{
    SearchServer server = GetSearchServer();
    const auto found_docs = server.FindTopDocuments("ухоженный кот"s);
    const Document& doc0 = found_docs[0];
    const Document& doc1 = found_docs[1];
    const Document& doc2 = found_docs[2];
    ASSERT(found_docs.size() == 3);    
}

void TestExcludeStopWordsFromAddedDocumentContent()
{
    SearchServer server("in the"s);
    server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
}

void TestMinusWords()
{
    SearchServer server(""s);
    server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
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
    server.AddDocument(0, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
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

int main() {
    TestSearchServer();

    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    const string query = "curly and funny -not"s;

    {
        const auto [words, status] = search_server.MatchDocument(query, 1);
        cout << words.size() << " words for document 1"s << endl;
        // 1 words for document 1
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
        cout << words.size() << " words for document 2"s << endl;
        // 2 words for document 2
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
        cout << words.size() << " words for document 3"s << endl;
        // 0 words for document 3
    }

    return 0;
}