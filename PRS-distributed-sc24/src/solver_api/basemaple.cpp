#include "utils/System.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"

#include "basemaple.hpp"

// using namespace MapleCOMSPS;

// Macros for minisat literal representation conversion
#define MINI_LIT(lit) lit > 0 ? MapleCOMSPS::mkLit(lit - 1, false) : MapleCOMSPS::mkLit((-lit) - 1, true)

#define INT_LIT(lit) MapleCOMSPS::sign(lit) ? -(var(lit) + 1) : (var(lit) + 1)

void basemaple::add(int l) {
   puts("wrong");
}

int basemaple::configure(const char* name, int val) {
   // if (strcmp(name, "worker_index") == 0) solver->worker_index = val;
   // if (strcmp(name, "worker_seed") == 0) solver->worker_seed = val;
   // if (strcmp(name, "worker_number") == 0) solver->worker_number = val;
   // printf("c maple configure %d\n", id);
   // solver->GE = 1;
   if (id == 1) solver->GE = 1;
   else solver->GE = 0;
   if (id % 2) solver->VSIDS = false;
   else solver->VSIDS = true;
   if (id % 4 >= 2) solver->verso = false;
   else solver->verso = true;
   return 1;
}

int basemaple::solve() {
   MapleCOMSPS::vec<MapleCOMSPS::Lit> miniAssumptions;
   MapleCOMSPS::lbool res = solver->solveLimited(miniAssumptions);
   if (res == (MapleCOMSPS::lbool((uint8_t)0))) return 10;
   if (res == (MapleCOMSPS::lbool((uint8_t)1))) return 20;
   return 0;
}

int basemaple::val(int l) {
   if (solver->model[l - 1] != (MapleCOMSPS::lbool((uint8_t)2))) {
      int lit = solver->model[l - 1] == (MapleCOMSPS::lbool((uint8_t)0)) ? l : -l;
      return lit;
   }
   return l;
}

void basemaple::terminate() {
   solver->interrupt();
}

void cbkLstechExportClause(void * issuer, int lbd, MapleCOMSPS::vec<MapleCOMSPS::Lit> &cls) {
	basemaple* mp = (basemaple*)issuer;
	if (lbd > mp->good_clause_lbd) return;

   shared_ptr<clause_store> ncls = std::make_shared<clause_store>(cls.size());  
	for (int i = 0; i < cls.size(); i++) {
		ncls->data[i] = INT_LIT(cls[i]);
   }

   ncls->lbd  = lbd;
   mp->export_clause.push(ncls);
}

bool cbkLstechImportClause(void * issuer, int * lbd, MapleCOMSPS::vec<MapleCOMSPS::Lit> & mcls) {
   basemaple* mp = (basemaple*)issuer;
   shared_ptr<clause_store> cls = NULL;
   if (mp->import_clause.pop(cls) == false) return false;
   *lbd = cls->lbd;
   mcls.clear();
   for (size_t i = 0; i < cls->size; i++) {
      MapleCOMSPS::Lit lit = MINI_LIT(cls->data[i]);
      mcls.push(lit);
   }
   cls->free_clause();
   return true;
}

basemaple::basemaple(int id, light* light) : basesolver(id, light) {
   // printf("c id is %d\n", id);
   solver = new MapleCOMSPS::SimpSolver();
	solver->issuer          = this;
   solver->cbkExportClause = NULL;
   solver->cbkImportClause = NULL;
   if (OPT(share)) {
      solver->cbkExportClause = cbkLstechExportClause;
      solver->cbkImportClause = cbkLstechImportClause;
   }
}

basemaple::~basemaple()
{
	delete solver;
}

void basemaple::parse_from_MEM(char* instance) {
    int vars, clauses;
    vec<vec<int>> clause;
    readinstance(instance, &vars, &clauses, clause);
    maxvar = vars;
    while (vars > solver->nVars()) solver->newVar();
    MapleCOMSPS::vec<MapleCOMSPS::Lit> lits;
    for (int i = 1; i <= clauses; i++) {
      int l = clause[i].size();
      lits.clear();
      for (int j = 0; j < l; j++)
         lits.push(MINI_LIT(clause[i][j]));
      solver->addClause_(lits);
    }
}
int basemaple::get_conflicts() {
    return 0;
}