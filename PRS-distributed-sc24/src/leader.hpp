#pragma once

#include <chrono>
#include <thread>
#include <fstream>
#include <mpi.h>

#include "comm_tag.h"
#include "light.hpp"
#include "utils/cmdline.h"
#include "paras.hpp"

void leader_main(light* S, int num_procs, int rank) {

    auto clk_st = std::chrono::high_resolution_clock::now();

    // __global_paras.print_change();

    // printf("c [leader] preprocess(simplify) input data\n");

    // 进行化简    
    auto pre = new preprocess();
    char *filename = const_cast<char*>(OPT(instance).c_str());
    int start = pre->do_preprocess(filename);

    // 给每个 worker 发布是否启动计算流程的信息
    for(int i=1; i<num_procs; i++) {
        MPI_Send(&start, 1, MPI_INT, i, START_TAG, MPI_COMM_WORLD);
    }

    // preprocess 证明了UNSAT 则不需要启动云计算
    if(!start) {
        MPI_Barrier(MPI_COMM_WORLD);
        // printf("c [leader] UNSAT!!!!!! by preprocess\n");
        printf("s UNSATISFIABLE\n");
        return;
    }

    std::stringstream ss;

    ss << "p cnf " << pre->vars << " " << pre->clause.size() << std::endl;
    for (int i = 1; i <= pre->clauses; i++) {
        int l = pre->clause[i].size();
        for (int j = 0; j < l; j++)
            ss << pre->clause[i][j] << " ";
        ss << "0" << std::endl;
    }

    const auto& str_ref = ss.str();
    char* cstr = const_cast<char *>(str_ref.c_str());

    int cnf_length = str_ref.size() + 1;

    // printf("c [leader] length of cnf (bytes): %lld\n", cnf_length);

    // printf("c [leader] hand out length of cnf instance to all nodes\n");

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&cnf_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    // printf("c [leader] hand out cnf instance to all nodes\n");

    MPI_Bcast(cstr, cnf_length, MPI_CHAR, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    // printf("c [leader] hand out done!\n");

    int is_sat;
    MPI_Request solved;
    MPI_Status status;
    MPI_Irecv(&is_sat, 1, MPI_INT, MPI_ANY_SOURCE, SOLVED_REPORT_TAG, MPI_COMM_WORLD, &solved);
    int* sol;

    int res = 0;

    // waiting for results:
    while(true) {
        
        // 检测时间是否超限
        auto clk_now = std::chrono::high_resolution_clock::now();
        int solve_time = std::chrono::duration_cast<std::chrono::seconds>(clk_now - clk_st).count();
        if (solve_time >= OPT(times)) {
            // printf("c [leader] solve time out\n");
            break;
        }

        int flag;
        // check if problem solved
        if(MPI_Test(&solved, &flag, &status) == MPI_SUCCESS && flag == 1) {

            MPI_Request temp[num_procs];

            // send terminal signal to all nodes
            for(int i=1; i<num_procs; i++) {
                if(i != status.MPI_SOURCE) {
                    MPI_Isend(NULL, 0, MPI_INT, i, TERMINATE_TAG, MPI_COMM_WORLD, &temp[i]);
                }
            }

            // send signal for getting solution(model) when sat

            if(is_sat) {
                res = 10;
                // printf("c [leader] received model size: %d\n", pre->vars);
                // printf("c SAT!!!!!!\n");

                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, MODEL_REPORT_TAG, MPI_COMM_WORLD);
                sol = new int[pre->vars];
                MPI_Recv(sol, pre->vars, MPI_INT, status.MPI_SOURCE, MODEL_REPORT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else {
                res = 20;
                // printf("s UNSATISFIABLE\n");
            }
            break;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(res == 10) {
        printf("s SATISFIABLE\n");
        for (int i = 1; i <= pre->orivars; i++)
            if (pre->mapto[i]) pre->mapval[i] = (sol[abs(pre->mapto[i])-1] > 0 ? 1 : -1) * (pre->mapto[i] > 0 ? 1 : -1);

        pre->get_complete_model();
        printf("v ");
        for (int i = 1; i <= pre->orivars; i++) {
            printf("%d ", i * pre->mapval[i]);
        }
        printf("0\n");
        delete []sol;
    } else if(res == 20) {
        printf("s UNSATISFIABLE\n");
    } else {
        printf("s UNKNOWN\n");
    }

    auto clk_now = std::chrono::high_resolution_clock::now();
    double solve_time = std::chrono::duration_cast<std::chrono::milliseconds>(clk_now - clk_st).count();
    // printf("c time: %.3f\n", solve_time / 1000);

}