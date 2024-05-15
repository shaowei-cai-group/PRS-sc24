#ifndef _light_hpp_INCLUDED
#define _light_hpp_INCLUDED

#include "utils/parse.hpp"
#include "preprocess/preprocess.hpp"
#include "paras.hpp"

#include <atomic>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <mpi.h>
typedef long long ll;

using std::shared_ptr;

class basesolver;
class sharer;

extern std::atomic<int> terminated;
extern std::mutex mtx;
struct thread_inf{
    int id, inf;
    bool operator < ( const thread_inf &other ) const
	{
		return inf > other.inf;
	}
};

struct light
{
public:
    light();
    ~light();
    // paras *opt;
    preprocess *pre;
    vec<basesolver *> workers;
    vec<sharer *> sharers;
    pthread_t *sharer_ptrs;
    vec<int> model;

    sharer* s;
    std::vector<std::vector<int>> sharing_groups;
    enum { KISSAT, MAPLE, LGL } solver_type;
    enum { SAT, UNSAT, DEFAULT } worker_type;
    int worker_rs;

    MPI_Request terminal_request;

    int num_procs;
    int rank;

    char* filename;
    char* instance;

    int finalResult;
    int winner_id;
    mutable boost::mutex winner_mtx;

    int maxtime;
    void update_winner(int id, int period) {
        boost::mutex::scoped_lock lock(winner_mtx); 
        if (id < winner_id) {
            winner_id = id;
        }
    }
    
    void arg_parse(int argc, char **argv);
    void init_workers();
    void diversity_workers();
    void seperate_groups();
    void parse_input();
    int  run();
    int  solve();
    void terminate_workers();
};

#endif