#ifndef _clause_hpp_INCLUDED
#define _clause_hpp_INCLUDED
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include "utils/vec.hpp"

using std::shared_ptr;

struct clause_store {
    int size, lbd;
    int *data;
    std::atomic<int> refs;
    clause_store(int sz) {
        //printf("c new clause_store\n");
        size = sz;
        data = (int*) malloc(sizeof(int) * sz);
        lbd = 0;
        refs = 1;
    }
    void increase_refs(int inc) {
        refs += inc;
    }

    ll __hash(int B, int MOD) {
        std::sort(data, data+size);

        ll res = 0;
        ll b = 1;

        for (int i=0; i < size; i++) {
            int d = (data[i] % MOD + MOD) % MOD;
            res = (res + d * b) % MOD;
            b = (b * B) % MOD;
        }

        return res;
    }

    int hash_code() {
        return __hash(31, 1000000007) ^ __hash(17, 1000000009);
    }

    bool free_clause() {
        // int ref = refs.fetch_sub(1);
        // if (ref <= 1) {
        //     free(data);
        //     return true;
        // }
        return false;
    }
    ~clause_store() {
        //printf("c free clause_store\n");
        free(data);
    }
};

#endif