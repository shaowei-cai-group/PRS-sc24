#include "utils/System.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"

#include "baselgl.hpp"

void baselgl::add(int l) {
   puts("wrong");
}

int baselgl::configure(const char* name, int val) {
   // if (strcmp(name, "worker_index") == 0) solver->worker_index = val;
   // if (strcmp(name, "worker_seed") == 0) solver->worker_seed = val;
   // if (strcmp(name, "worker_number") == 0) solver->worker_number = val;
   // printf("c maple configure %d\n", id);
   // solver->GE = 1;
   // if (id == 1) solver->GE = 1;
   // else solver->GE = 0;
   // if (id % 2) solver->VSIDS = false;
   // else solver->VSIDS = true;
   // if (id % 4 >= 2) solver->verso = false;
   // else solver->verso = true;
   return 1;
}

int baselgl::solve() {
   int res = lglsat(solver);
   return res;
}

int baselgl::val(int l) {
   int res = lglderef(solver, l);
   if(res > 0) return l;
   else return -l;
}

void baselgl::terminate() {
   terminated = 1;
}
 

int cbCheckTerminate(void* solverPtr) {
   return terminated;
}

// void cbkLstechExportClause(void * issuer, int lbd, MapleCOMSPS::vec<MapleCOMSPS::Lit> &cls) {
// 	baselgl* mp = (baselgl*)issuer;
// 	if (lbd > mp->good_clause_lbd) return;

//    shared_ptr<clause_store> ncls = std::make_shared<clause_store>(cls.size());  
// 	for (int i = 0; i < cls.size(); i++) {
// 		ncls->data[i] = INT_LIT(cls[i]);
//    }

//    ncls->lbd  = lbd;
//    mp->export_clause.push(ncls);
// }

// bool cbkLstechImportClause(void * issuer, int * lbd, MapleCOMSPS::vec<MapleCOMSPS::Lit> & mcls) {
//    baselgl* mp = (baselgl*)issuer;
//    shared_ptr<clause_store> cls = NULL;
//    if (mp->import_clause.pop(cls) == false) return false;
//    *lbd = cls->lbd;
//    mcls.clear();
//    for (size_t i = 0; i < cls->size; i++) {
//       MapleCOMSPS::Lit lit = MINI_LIT(cls->data[i]);
//       mcls.push(lit);
//    }
//    cls->free_clause();
//    return true;
// }

baselgl::baselgl(int id, light* light) : basesolver(id, light) {

   // solver->issuer          = this;
   // solver->cbkExportClause = NULL;
   // solver->cbkImportClause = NULL;

   solver = lglinit();
   lglseterm(solver, cbCheckTerminate, this);
	
   // if (OPT(share)) {
   //    solver->cbkExportClause = cbkLstechExportClause;
   //    solver->cbkImportClause = cbkLstechImportClause;
   // }
}

baselgl::~baselgl()
{
	delete solver;
}

void baselgl::parse_from_MEM(char* instance) {
    int vars, clauses;
    vec<vec<int>> clause;
    readinstance(instance, &vars, &clauses, clause);
    maxvar = vars;
    for (int i = 1; i <= clauses; i++) {
      for (int j = 0; j < clause[i].size(); j++)
         lgladd(solver, clause[i][j]);
      lgladd(solver, 0);
    }
}
int baselgl::get_conflicts() {
    return 0;
}