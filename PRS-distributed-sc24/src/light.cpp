#include <chrono>
#include "light.hpp"
#include <unistd.h>
#include <thread>
#include <iostream>

#include "utils/cmdline.h"

light::light():
  finalResult           (0),
  winner_id             (1e9),
  maxtime               (5000)
{
    // opt = new paras();
    //opt->init_paras();
}

light::~light() {
    for (int i = 0; i < workers.size(); i++) delete(workers[i]);
    workers.clear(true);
}

void light::arg_parse(int argc, char **argv) {
    cmdline::parser parser;

    #define STR_PARA(N, S, M, D, C) \
    parser.add<std::string>(#N, S, C, M, D);
    STR_PARAS
    #undef STR_PARA

    #define PARA(N, T, S, M, D, L, H, C) \
    if (!strcmp(#T, "int")) parser.add<int>(#N, S, C, M, D, cmdline::range((int)L, (int)H)); \
    else parser.add<double>(#N, S, C, M, D, cmdline::range((double)L, (double)H));
    PARAS
    #undef PARA

    parser.parse_check(argc, argv);

    #define STR_PARA(N, S, M, D, C) \
    OPT(N) = parser.get<std::string>(#N);
    STR_PARAS
    #undef STR_PARA

    #define PARA(N, T, S, M, D, L, H, C) \
    if (!strcmp(#T, "int")) OPT(N) = parser.get<int>(#N); \
    else OPT(N) = parser.get<double>(#N);
    PARAS
    #undef PARA

    std::string file_string = OPT(instance);
    filename = new char[file_string.size() + 1];
    memcpy(filename, file_string.c_str(), file_string.length());
    filename[file_string.length()] = '\0';


}