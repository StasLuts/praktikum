#include <cstdlib>
#include <future>
#include <map>
#include <string>
#include <mutex>
#include <vector>

#pragma once

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap
{
private:

    struct Bucket_
    {
        std::mutex map_mutex_;
        std::map<Key, Value> map_;
    };

    std::vector<Bucket_> buckets_;

public:

    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);


    struct Access
    {
        Access(const Key& key, Bucket_& bucket)
            :value_mutex(bucket.map_mutex_), ref_to_value(bucket.map_[key]) {}

        std::lock_guard<std::mutex> value_mutex;
        Value& ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count)
        :buckets_(bucket_count) {}

    void erase(const Key& key)
    {
        Bucket_& bucket = buckets_[static_cast<uint64_t>(key) % buckets_.size()];
        std::lock_guard lock(bucket.map_mutex_);
        bucket.map_.erase(key);
    }

    Access operator[](const Key& key)
    {

        return { key,  buckets_[static_cast<uint64_t>(key) % buckets_.size()] };
    }

    std::map<Key, Value> BuildOrdinaryMap()
    {
        std::map<Key, Value> result;
        for (auto& [mutex, map] : buckets_)
        {
            std::lock_guard lock_map(mutex);
            result.insert(map.begin(), map.end());
        }
        return result;
    }
};