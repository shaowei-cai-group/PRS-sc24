#include "light.hpp"
#include "solver_api/basesolver.hpp"
#include "sharer.hpp"
#include "unordered_map"
#include "clause.hpp"
#include <unistd.h>
#include "comm_tag.h"
#include <boost/thread/thread.hpp>

int nums = 0;
double share_time = 0;
int num_procs, rank;

const int BUF_SIZE = 100 * 1024 * 1024;
std::vector<std::pair<MPI_Request*, shared_ptr<int[]>>> send_data_struct;
MPI_Request receive_request;
int buf[BUF_SIZE];

int num_received_clauses_by_network = 0;
int num_skip_clauses_by_network = 0;

// 记录子句是否已经导入过

std::unordered_map<int, bool> clause_imported;

void sharer::share_clauses_to_other_node(int from, const std::vector<shared_ptr<clause_store>> &cls) {

    // 环形传递，数据来源如果是目的地，说明数据已轮转一圈，停止发送。
    if(OPT(share_method) == 0 && from == next_node) return;

    // 定义发送数据
    int send_length = 1;

    // 初始化发送数据
    for(int i=0; i<cls.size(); i++) {
        send_length += (cls[i]->size + 2);
    }

    shared_ptr<int[]> send_buf(new int[send_length]());

    for(int i=0; i<send_length; i++) {
        send_buf[i] = i;
    }

    int index = 0;

    send_buf[index++] = from;

    for(int i=0; i<cls.size(); i++) {
        auto& c = cls[i];
        send_buf[index++] = c->size;
        send_buf[index++] = c->lbd;
        for(int j=0; j<c->size; j++) {
            send_buf[index++] = c->data[j];
        }
    }

    assert(index == send_length);

    // 调用 MPI 发送共享子句

    if(OPT(share_method)) {

        // printf("MPI_SEND: %d %d   from: %d rank: %d\n", son[from][rank][0], son[from][rank][1], from, rank);
        
        // 树形传递
        if(son[from][rank][0] != -1) {
            MPI_Request *send_request = new MPI_Request();
            MPI_Isend(send_buf.get(), send_length, MPI_INT, son[from][rank][0], SHARE_CLAUSES_TAG, MPI_COMM_WORLD, send_request);
            send_data_struct.push_back(std::make_pair(send_request, send_buf));
        }
        if(son[from][rank][1] != -1) {
            MPI_Request *send_request = new MPI_Request();
            MPI_Isend(send_buf.get(), send_length, MPI_INT, son[from][rank][1], SHARE_CLAUSES_TAG, MPI_COMM_WORLD, send_request);
            send_data_struct.push_back(std::make_pair(send_request, send_buf));
        }

    } else {
        
        // printf("c node%d(%d) to %d exported lits from network\n", rank, S->worker_type, send_length);
        MPI_Request *send_request = new MPI_Request();
        // 环形传递
        MPI_Isend(send_buf.get(), send_length, MPI_INT, next_node, SHARE_CLAUSES_TAG, MPI_COMM_WORLD, send_request);
        send_data_struct.push_back(std::make_pair(send_request, send_buf));
    }    
    
    // printf("c [worker] send clauses: %d\n", send_length);

    // 清理 send_data_struct，把发送完毕的发送数据结构清理掉
    for(int i=0; i<send_data_struct.size(); i++) {
        // 已完成发送，释放内存空间
        int flag;
        if(MPI_Test(send_data_struct[i].first, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 1) {
            delete send_data_struct[i].first;
            // 与数组最后一个交换，然后 pop_back;
            std::swap(send_data_struct[i], send_data_struct[send_data_struct.size()-1]);
            send_data_struct.pop_back();
        }
    }
}

int sharer::receive_clauses_from_other_node(std::vector<shared_ptr<clause_store>> &clauses, int &transmitter) {

    clauses.clear();

    int flag;
    MPI_Status status;

    transmitter = -1;
    int from = -1;

    MPI_Test(&receive_request, &flag, &status);

    // 没有数据需要接收
    if(flag == 0) {
        return -1;
    }

    int index = 0;
    int count;
    MPI_Get_count(&status, MPI_INT, &count);

    transmitter = status.MPI_SOURCE;

    from = buf[index++];

    while(index < count) {
        num_received_clauses_by_network++;

        shared_ptr<clause_store> cl = std::make_shared<clause_store>(buf[index++]);

        cl->lbd = buf[index++];
        for(int i=0; i<cl->size; i++) {
            cl->data[i] = buf[index++];
        }

        if(clause_imported[cl->hash_code()]) {
            num_skip_clauses_by_network++;
            continue;
        }

        clauses.push_back(cl);
    }

    assert(index == count);

    MPI_Irecv(buf, BUF_SIZE, MPI_INT, MPI_ANY_SOURCE, SHARE_CLAUSES_TAG, MPI_COMM_WORLD, &receive_request);

    return from;
}

void sharer::clause_sharing_init(std::vector<std::vector<int>> &sharing_groups) {

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // printf("c sharing groups: ");
    // for(int i=0; i<sharing_groups.size(); i++) {
    //     printf(" [ ");
    //     for(int j=0; j<sharing_groups[i].size(); j++) {
    //         printf("%d ", sharing_groups[i][j]);
    //     }
    //     printf("]");
    // }
    // printf("\n");

    if(OPT(share_method)) {
        init_tree_transmission(sharing_groups);
    } else {
        init_circular_transmission(sharing_groups);
    }

    MPI_Irecv(buf, BUF_SIZE, MPI_INT, MPI_ANY_SOURCE, SHARE_CLAUSES_TAG, MPI_COMM_WORLD, &receive_request);

}

void sharer::clause_sharing_end() {
    // printf("c node%d sharing nums: %d\nc sharing time: %.2lf\n", rank, nums, share_time);
    // printf("c node%d sharing received_num_by_network: %d\n", rank, num_received_clauses_by_network);
    // printf("c node%d sharing skip_num_by_network: %d\n", rank, num_skip_clauses_by_network);
    // printf("c node%d sharing unique reduce percentage: %.2f%%\n", rank, (double) num_skip_clauses_by_network / num_received_clauses_by_network * 100);
}

void sharer::do_clause_sharing() {

    static auto clk_st = std::chrono::high_resolution_clock::now();
    
    ++nums;
    auto clk_now = std::chrono::high_resolution_clock::now();
    int solve_time = std::chrono::duration_cast<std::chrono::milliseconds>(clk_now - clk_st).count();

    // printf("c node%d(%d)round %d, time: %d.%d\n", rank, S->worker_type, nums, solve_time / 1000, solve_time % 1000);

    // 导入外部网络传输的子句
    std::vector<shared_ptr<clause_store>> clauses;
    int transmitter;
    int from;

    int received_lits = 0;
    while((from = receive_clauses_from_other_node(clauses, transmitter)) != -1 &&clauses.size() > 0) {

        for (int k = 0; k < clauses.size(); k++) {
            clause_imported[clauses[k]->hash_code()] = true;
            received_lits += clauses[k]->size;
        }

        for (int j = 0; j < consumers.size(); j++) {
            consumers[j]->import_clauses_from(clauses);
        }

        // 传递外部网络传输的子句给下个节点
        share_clauses_to_other_node(from, clauses);
    }

    // printf("c node%d(%d) get %d exported lits from network\n", rank, S->worker_type, received_lits);

    for (int i = 0; i < producers.size(); i++) {
        cls.clear();
        producers[i]->export_clauses_to(cls);

        // 删除掉重复的学习子句
        int t_size = cls.size();
        for(int i=0; i<t_size; i++) {
            if(clause_imported[cls[i]->hash_code()]) {
                std::swap(cls[i], cls[t_size-1]);
                t_size--;
                i--;
            }
        }
        
        cls.resize(t_size);

        int percent = sort_clauses(i);
        //分享当前节点产生的子句
        if(cls.size() > 0) share_clauses_to_other_node(rank, cls);

        //printf("c [worker] thread-%d: get %d exported clauses\n", i, t_size);
            
        // 增加 lits 限制

        if(S->worker_type != light::SAT) {
            if (percent < 75) {
                producers[i]->broaden_export_limit();
            }
            else if (percent > 98) {
                producers[i]->restrict_export_limit();
            }
        }

        for (int k = 0; k < cls.size(); k++) {
            clause_imported[cls[k]->hash_code()] = true;
        }
          
        for (int j = 0; j < consumers.size(); j++) {
            if (producers[i]->id == consumers[j]->id) continue;
            consumers[j]->import_clauses_from(cls);
        }
        
    }

    auto clk_ed = std::chrono::high_resolution_clock::now();
    share_time += 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(clk_ed - clk_now).count();
}

int sharer::sort_clauses(int x) {
    for (int i = 0; i < cls.size(); i++) {
        int sz = cls[i]->size;
        while (sz > bucket[x].size()) bucket[x].push();
        if (sz * (bucket[x][sz - 1].size() + 1) <= OPT(share_lits)) 
            bucket[x][sz - 1].push_back(cls[i]);
        // else
        //     cls[i]->free_clause();
    }
    cls.clear();
    int space = OPT(share_lits);
    for (int i = 0; i < bucket[x].size(); i++) {
        int clause_num = space / (i + 1);
        // printf("%d %d\n", clause_num, bucket[x][i].size());
        if (!clause_num) break;
        if (clause_num >= bucket[x][i].size()) {
            space -= bucket[x][i].size() * (i + 1);
            for (int j = 0; j < bucket[x][i].size(); j++)
                cls.push_back(bucket[x][i][j]);
            bucket[x][i].clear();
        }
        else {
            space -= clause_num * (i + 1);
            for (int j = 0; j < clause_num; j++) {
                cls.push_back(bucket[x][i].back());
                bucket[x][i].pop_back();
            }
        }
    }
    // printf("c share %d lits\n", OPT(share_lits) - space);
    return (OPT(share_lits) - space) * 100 / OPT(share_lits);
}

void sharer::init_tree_transmission(std::vector<std::vector<int>> &sharing_groups) {

    srand(17);

    son = new int**[num_procs];
    for(int i=0; i<num_procs; i++) {
        son[i] = new int*[num_procs];
        for(int j=0; j<num_procs; j++) {
            son[i][j] = new int[2];
        }
    }
    
    // printf("c =========build tree=========\n");
    for(int i=0; i<sharing_groups.size(); i++) {
        // create binary trees for every group;
        std::vector<int> &group = sharing_groups[i];

        for(int j=0; j<group.size(); j++) {
            // create a binary tree with root group[j];

            // radom shuffle
            std::vector<int> rs = group;
            // keep group[j] as the first
            std::swap(rs[j], rs[0]);
            for(int k=1; k<rs.size(); k++) {
                std::swap(rs[k], rs[rand()%(rs.size()-1)+1]);
            }

            // build a tree from array
            for(int k=0; k<rs.size(); k++) {
                son[rs[0]][rs[k]][0] = ( 2 * k + 1 < rs.size() ) ? rs[ 2 * k + 1 ] : -1;
                son[rs[0]][rs[k]][1] = ( 2 * k + 2 < rs.size() ) ? rs[ 2 * k + 2 ] : -1;
                
                // printf("c son[%d][%d][%d]=%d\tson[%d][%d][%d]=%d\n", rs[0], rs[k], 0, son[rs[0]][rs[k]][0], rs[0], rs[k], 1, son[rs[0]][rs[k]][1]);
            }
        }
    }
}

void sharer::init_circular_transmission(std::vector<std::vector<int>> &sharing_groups) {
    for(int i=0; i<sharing_groups.size(); i++) {
        for(int j=0; j<sharing_groups[i].size(); j++) {
            if(sharing_groups[i][j] == S->rank) {
                next_node = sharing_groups[i][(j+1)%sharing_groups[i].size()];
                return;
            }
        }
    }
}