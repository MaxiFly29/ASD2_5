#include <functional>
#include <forward_list>
#include <vector>
#include <algorithm>

#include "profile.h"

using namespace std;

template <typename Type, typename Hasher = hash<Type>>
class HashSet {
public:
    using BucketList = forward_list<Type>;

public:
    explicit HashSet(
        size_t num_buckets,
        const Hasher& hasher = {}
    ) : un_map(num_buckets) {}

    void Add(const Type& value) {
        if (!Has(value))
            un_map[Hasher()(value) % un_map.size()].push_front(value);
    }
    bool Has(const Type& value) const {
        auto hassh = Hasher()(value) % un_map.size();
        return find(begin(un_map[hassh]), end(un_map[hassh]), value) != un_map[hassh].end();
    }
    void Erase(const Type& value) {
        auto hassh = Hasher()(value) % un_map.size();
        if (Has(value))
            un_map[hassh].remove(value);
    }

    vector<Type> orderedSequence() const {
        vector<Type> v;
        for (const auto& buck : un_map) {
            for (auto val : buck) {
                v.push_back(val);
            }
        }
        sort(begin(v), end(v));
        return v;
    }
    
private:
    vector<BucketList> un_map;
};

int main() {
    vector<int> rvec(1e7);
    for (int i = 0; i < 10; i++) {
        for (int& x : rvec)
            x = rand() % 1'000'000;
        HashSet<int> hs(1e7);
        {
            LOG_DURATION("method Add - " + to_string(i + 1));
            for (int j = 0; j < 1e7; j++) {
                hs.Add(rvec[j]);
            }
        }

        {
            LOG_DURATION("method Find (exists) - " + to_string(i + 1));
            for (int j = 0; j < 1e7; j++) {
                volatile auto f = hs.Has(rvec[j]);
            }
        }

        {
            LOG_DURATION("method Find (not exists) - " + to_string(i + 1));
            for (int j = 0; j < 1e7; j++) {
                volatile auto f = hs.Has(rvec[j] + 1e7);
            }
        }

        {
            LOG_DURATION("method orderedSequence - " + to_string(i + 1));
            volatile auto res = hs.orderedSequence();
        }

        {
            LOG_DURATION("method Erase - " + to_string(i + 1));
            for (int j = 0; j < 1e7; j++) {
                hs.Erase(rvec[j]);
            }
        }
    }
    return 0;
}