#ifndef _basesolver_hpp_INCLUDED
#define _basesolver_hpp_INCLUDED

#include "../light.hpp"
#include "../utils/vec.hpp"
#include "../clause.hpp"
#include <fstream>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>

class sharer;
class basesolver {
public:
    virtual void add(int l) = 0;
    virtual int  solve() = 0;
    virtual int  val(int l) = 0;
    virtual void terminate() = 0;
    virtual int configure(const char* name, int id) = 0;
    virtual int  get_conflicts() = 0;

    virtual void parse_from_MEM(char* instance) = 0;
    virtual void exp_clause(void *cl, int lbd) = 0;
    virtual bool imp_clause(shared_ptr<clause_store>cls, void *cl) = 0;
    
    void export_clauses_to(std::vector<shared_ptr<clause_store>> &clauses);
    void import_clauses_from(std::vector<shared_ptr<clause_store>> &clauses);

    void get_model(vec<int> &model);
    void broaden_export_limit();
    void restrict_export_limit();

    int good_clause_lbd = 0;
    light * controller;
    int id;
    vec<int> model;

    int maxvar, terminated = 0;

    boost::lockfree::spsc_queue<shared_ptr<clause_store>, boost::lockfree::capacity<10240000>> import_clause;
    boost::lockfree::spsc_queue<shared_ptr<clause_store>, boost::lockfree::capacity<10240000>> export_clause;

    basesolver(int sid, light* light) : id(sid), controller(light) {
        good_clause_lbd = 2;
    }

    ~basesolver() {
        if (controller) {
            controller = NULL;
        }
    }
};

#endif