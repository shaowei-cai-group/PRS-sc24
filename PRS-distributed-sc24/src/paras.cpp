#include "paras.hpp"
#include <cstdio>
#include <iostream>
#include <string>

paras __global_paras;
 
void paras::print_change() {
    printf("c ------------------- Paras list -------------------\n");
    printf("c %-20s\t %-10s\t %-10s\t %-10s\t %s\n",
           "Name", "Type", "Now", "Default", "Comment");

#define PARA(N, T, S, M, D, L, H, C) \
    if (!strcmp(#T, "int")) printf("c %-20s\t %-10s\t %-10d\t %-10s\t %s\n", (#N), (#T), N, (#D), (C)); \
    else printf("c %-20s\t %-10s\t %-10f\t %-10s\t %s\n", (#N), (#T), N, (#D), (C)); 
    PARAS
#undef PARA

#define STR_PARA(N, S, M, D, C) \
    printf("c %-20s\t string\t\t %-10s\t %-10s\t %s\n", (#N), N.c_str(), (#D), (C));
    STR_PARAS
#undef STR_PARA

    printf("c --------------------------------------------------\n");
}
