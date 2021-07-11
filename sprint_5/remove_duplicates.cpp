#include "remove_duplicates.h"

#include <iostream>
#include <string>
#include <map>
#include <set>

using namespace std;

void RemoveDuplicates(SearchServer& search_server)
{
    map<set<string>, int> words_id;
    vector<int> to_del;
    for (const int document_id : search_server)
    {
        set<string> words;
        for (auto [word, freq] : search_server.GetWordFrequencies(document_id))
        {
            words.insert(word);
        }
        if (words_id.count(words))
        {
            to_del.push_back(document_id);
        }
        words_id[words] = document_id;
    }

    for (const int id : to_del)
    {
        search_server.RemoveDocument(id);
    }
}