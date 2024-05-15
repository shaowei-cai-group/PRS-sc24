#include "basesolver.hpp"

extern "C" {
#include "lglib.h"
}

class baselgl : public basesolver {
public:
   void terminate();
   void add(int l);
   int  solve();
   int  val(int l);
   int  configure(const char* name, int id);
   
   int  get_conflicts();
   void parse_from_MEM(char* instance);
   void exp_clause(void *cl, int lbd) {
      // puts("wrong");
   }
   bool imp_clause(shared_ptr<clause_store>cls, void *cl) {
      // puts("wrong");
   }

   baselgl(int id, light *light);
   ~baselgl();

   LGL* solver;
   // friend bool cbkLstechImportClause(void *, int *, MapleCOMSPS::vec<MapleCOMSPS::Lit> &);
   // friend void cbkLstechExportClause(void *, int, MapleCOMSPS::vec<MapleCOMSPS::Lit> &);
};
