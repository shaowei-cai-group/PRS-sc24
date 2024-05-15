#include <chrono>
#include <thread>
#include <fstream>
#include <ctime>
#include <mpi.h>
#include <cmath>

#include "light.hpp"
#include "utils/cmdline.h"
#include "paras.hpp"
#include "comm_tag.h"

void worker_main(light* S, int num_procs, int rank) {

    auto clk_st = std::chrono::high_resolution_clock::now();

    // 阻塞接收初始化信号
    int start;
    MPI_Recv(&start, 1, MPI_INT, 0, START_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(!start) {
        // printf("c [worker%d] I have no need to start\n", rank);
        MPI_Barrier(MPI_COMM_WORLD);
        return;
    }

    // 监听 terminate 信号
    MPI_Irecv(NULL, 0, MPI_INT, 0, TERMINATE_TAG, MPI_COMM_WORLD, &S->terminal_request);

    // 等待同步 CNF 文件
    
    int cnf_length;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&cnf_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    S->instance = new char[cnf_length + 1];

    MPI_Bcast(S->instance, cnf_length, MPI_CHAR, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    // mem_usage per thread per G input file (GB)
    double mem_ptpg = 8.0;

    // mem_usage per thread (GB)
    double mem_pt = (double)cnf_length / 1073741824 * mem_ptpg;

    //printf("mem_pt: %lf\n", mem_pt);

    // max thread with 64G mem machine
    double threads = 64.0 / mem_pt;
    // printf("threads: %lf\n", threads);

    if(threads > 16) {
        // printf("c [worker%d] threads per worker were set %d by default : %d\n", rank, OPT(threads));
    } else {
        OPT(threads) = floor(threads);
        // printf("c [worker%d] too big instance %lld\n", rank, cnf_length);
        // printf("c [worker%d] threads per worker were set %d by memcheck %d\n", rank, OPT(threads));
    }

    int res = S->run();

    const char* str_solver_type[] = { "KISSAT", "MAPLE", "LGL" };
    const char* str_worker_type[] = { "SAT", "UNSAT", "DEFAULT"};

    if(res == 10 || res == 20) {
        printf("c [worker%d] result: %d solver: %s type: %s\n", rank, res, str_solver_type[S->solver_type], str_worker_type[S->worker_type]);
    }

    // printf("c [worker%d] kissat exit with result: %d\n", rank, res);

    MPI_Request *solved_request = new MPI_Request();
    MPI_Request *model_request = new MPI_Request();

    MPI_Irecv(NULL, 0, MPI_INT, 0, MODEL_REPORT_TAG, MPI_COMM_WORLD, model_request);

    if(res == 10) {
        int is_sat = 1;
        MPI_Isend(&is_sat, 1, MPI_INT, 0, SOLVED_REPORT_TAG, MPI_COMM_WORLD, solved_request);

        while(true) {

            int flag;

            auto clk_now = std::chrono::high_resolution_clock::now();
            int solve_time = std::chrono::duration_cast<std::chrono::seconds>(clk_now - clk_st).count();
            if (solve_time >= OPT(times)) {
                // printf("c [worker%d] solve time out\n", rank);
                break;
            }

            // when getting terminate signal
            if(MPI_Test(&S->terminal_request, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 1) {
                // printf("c [worker%d] getting terminate signal\n", rank);
                break;
            }

            // when getting model signal
            if(MPI_Test(model_request, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 1) {

                // printf("c [worker%d] getting send model signal\n", rank);

                // send model and break;
                MPI_Send(S->model.data, S->model.size(), MPI_INT, 0, MODEL_REPORT_TAG, MPI_COMM_WORLD);
                break;
            }
        }

    } else if(res == 20) {
        int flag;
        int is_sat = 0;
        MPI_Isend(&is_sat, 1, MPI_INT, 0, SOLVED_REPORT_TAG, MPI_COMM_WORLD, solved_request);
    } else {
        // when unknown do nothing.
    }

    MPI_Barrier(MPI_COMM_WORLD);
}