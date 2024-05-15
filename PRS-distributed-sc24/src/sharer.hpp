#ifndef _sharer_hpp_INCLUDED
#define _sharer_hpp_INCLUDED
#include "paras.hpp"
#include <boost/thread.hpp>
#include "utils/vec.hpp"
#include "clause.hpp"

class basesolver;
class sharer {
public:
    vec<std::vector<shared_ptr<clause_store>>> bucket[64];
    vec<basesolver *> producers, consumers;
    std::vector<shared_ptr<clause_store>> cls;

    sharer(light *S):S(S) {}

    void clause_sharing_init(std::vector<std::vector<int>> &sharing_groups);

    void do_clause_sharing();

    void clause_sharing_end();

    // next node in circle mode
    int next_node;

    // son of this node in tree broadcast mode
    // left  son -> son[root][0]
    // right son -> son[root][1]
    // when son[root][x] == -1, no son
    int ***son;

    void init_circular_transmission(std::vector<std::vector<int>> &sharing_groups);
    void init_tree_transmission(std::vector<std::vector<int>> &sharing_groups);

    void share_clauses_to_other_node(int from, const std::vector<shared_ptr<clause_store>> &cls);
    int receive_clauses_from_other_node(std::vector<shared_ptr<clause_store>> &clauses, int &transmitter);
    
    int sort_clauses(int x);

private:
    light* S;
};

#endif