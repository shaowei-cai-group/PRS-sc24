#include "basekissat.hpp"
#include "../sharer.hpp"
#include <thread>
#include <condition_variable>

extern "C" {
   #include "src/application.h"
   #include "src/parse.h"
   #include "src/internal.h"
   #include "src/witness.h"
   #include "src/import.h"
}

void basekissat::add(int l) {
    kissat_add(solver, l);
}

int basekissat::configure(const char* name, int val) {
    // printf("c %d set %s to %d\n", id, name, val);
    return kissat_set_option(solver, name, val);
}

int  basekissat::solve() {
    return kissat_solve(solver);
}

void basekissat::terminate() {
    kissat_terminate(solver);
}

int  basekissat::val(int l) {
    int v = abs(l);
    int tmp = kissat_value(solver, v);
    if (!tmp) tmp = v;
    if (l < 0) tmp = -tmp;
    return tmp;
}

void basekissat::exp_clause(void* cl, int lbd) {
    cvec *c = (cvec *) cl;
    shared_ptr<clause_store> cls = std::make_shared<clause_store>(c->sz);
    for (int i = 0; i < c->sz; i++) {
        int v = cvec_data(c, i);
        int eidx = PEEK_STACK(solver->exportk, (v >> 1));
        cls->data[i] = v & 1 ? -eidx : eidx;
    }
    // ++S->x2;
    // if (S->id == 0) puts("");
    cls->lbd = lbd;

    // printf("clasue: [ ");
    // for(int i=0; i<cls->size; i++) {
    //     printf("%d " , cls->data[i]);
    // }
    // printf("]\n");

    export_clause.push(cls);
}


void call_back_out(void *solver, int lbd, cvec *c) {
    basekissat* S = (basekissat *) solver;
    if (lbd <= S->good_clause_lbd) {
        S->exp_clause(c, lbd); 
    }
}

bool basekissat::imp_clause(shared_ptr<clause_store>cls, void *cl) {
    cvec *c = (cvec *) cl;
    for (int i = 0; i < cls->size; i++) {
        // S->outimport << cls->data[i] << std::endl;
        int eidx = abs(cls->data[i]);

        assert(eidx > 0);

        if (eidx >= SIZE_STACK(solver->import)) printf("c wrong %d   sizeo_stack %d\n", eidx, SIZE_STACK(solver->import));
        import *import = &PEEK_STACK (solver->import, eidx);

        if (import->eliminated) return false;
        else {
            int ilit = import->lit;
            if (cls->data[i] < 0) ilit = ilit ^ 1;
            cvec_push(c, ilit);
        }
    }
    return true;
}

int call_back_in(void *solver, int *lbd, cvec *c) {
    basekissat* S = (basekissat *) solver;
    shared_ptr<clause_store> cls = NULL;
    if (S->import_clause.pop(cls) == false) return -1;
    *lbd = cls->lbd;
    bool res = S->imp_clause(cls, c); 
    if (!S->solver->dps) {
        cls->free_clause();
    }
    if (!res) return -10;
    return 1;
}

basekissat::basekissat(int id, light* light) : basesolver(id, light) {
    solver = kissat_init();
    solver -> issuer = this;
    solver -> cbkImportClause = NULL;
    solver -> cbkExportClause = NULL;
    solver -> cbk_start_new_period  = NULL;
    if (OPT(share)) {
        solver -> cbkImportClause       = call_back_in;
        solver -> cbkExportClause       = call_back_out;
        solver -> dps                   = OPT(DPS);
        solver -> dps_period            = OPT(DPS_period);
    }
}

basekissat::~basekissat(){
    delete solver;
}


void basekissat::parse_from_MEM(char* instance) {
    int vars, clauses;
    vec<vec<int>> clause;
    readinstance(instance, &vars, &clauses, clause);
    maxvar = vars;
    kissat_reserve(solver, vars);
    for (int i = 1; i <= clauses; i++) {
        int l = clause[i].size();
        for (int j = 0; j < l; j++)
            add(clause[i][j]);
        add(0);
    }
}

int basekissat::get_conflicts() {
    return solver->nconflict;
}

