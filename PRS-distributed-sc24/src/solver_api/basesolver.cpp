#include "basesolver.hpp"
#include "../sharer.hpp"

void basesolver::broaden_export_limit() {
    ++good_clause_lbd;
}

void basesolver::restrict_export_limit() {
    if (good_clause_lbd > 2)
        --good_clause_lbd;
}

void basesolver::export_clauses_to(std::vector<shared_ptr<clause_store>> &clauses) {
    shared_ptr<clause_store> cls;
    
    while (export_clause.pop(cls)) 
        clauses.push_back(cls);
}

void basesolver::import_clauses_from(std::vector<shared_ptr<clause_store>> &clauses) {
    for (int i = 0; i < clauses.size(); i++) {
        import_clause.push(clauses[i]);
    }
}

void basesolver::get_model(vec<int> &model) {
    model.clear();
    for (int i = 1; i <= maxvar; i++) {
        model.push(val(i));
    }   
}