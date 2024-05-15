
#include <chrono>
#include <thread>
#include <fstream>
#include <mpi.h>

#include "light.hpp"
#include "utils/cmdline.h"
#include "paras.hpp"

#include "leader.hpp"
#include "worker.hpp"

#include "clause.hpp"

int main(int argc, char **argv) {

    int num_procs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    light* S = new light();
    S->num_procs = num_procs;
    S->rank = rank;
    S->arg_parse(argc, argv);

    // leader
    if(rank == 0) leader_main(S, num_procs, rank);
    else worker_main(S, num_procs, rank);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}