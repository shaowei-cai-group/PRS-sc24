#ifndef _paras_hpp_INCLUDED
#define _paras_hpp_INCLUDED

#include <string>
#include <cstring>
#include <unordered_map>

//        name,       type,  short-name,must-need, default ,low, high, comments
#define PARAS \
    PARA( DPS          , int    , '\0'  ,  false   , 0     , 0  , 1   , "DPS/NPS") \
    PARA( DPS_period   , int    , '\0'  ,  false   , 10000 , 1  , 1e8 , "DPS sharing period") \
    PARA( margin       , int    , '\0'  ,  false   , 0     , 0  , 1e3 , "DPS margin") \
    PARA( pakis        , int    , '\0'  ,  false   , 1     , 0  , 1   , "Use pakis diversity") \
    PARA( reset        , int    , '\0'  ,  false   , 0     , 0  , 1   , "Dynamically reseting") \
    PARA( reset_time   , int    , '\0'  ,  false   , 10    , 1  , 1e5 , "Reseting base interval (seconds)") \
    PARA( share        , int    , '\0'  ,  false   , 1     , 0  , 1   , "Sharing learnt clauses") \
    PARA( share_intv   , int    , '\0'  ,  false   , 500000, 0  , 1e9 , "Sharing interval (microseconds)") \
    PARA( share_lits   , int    , '\0'  ,  false   , 300   , 0  , 1e6 , "Sharing lits (per every #share_intv seconds)") \
    PARA( share_method , int    , '\0'  ,  false   , 1     , 0  , 1   , "0 for Circle Propagate/ 1 for Tree Broadcast") \
    PARA( shuffle      , int    , '\0'  ,  false   , 1     , 0  , 1   , "Use random shuffle") \
    PARA( simplify     , int    , '\0'  ,  false   , 1     , 0  , 1   , "Use Simplify (only preprocess)") \
    PARA( threads      , int    , '\0'  ,  false   , 32    , 1  , 128 , "Thread number") \
    PARA( times        , double , '\0'  ,  false   , 5000  , 0  , 1e8 , "Cutoff time") \
    PARA( unique       , int    , '\0'  ,  false   , 1     , 0  , 1   , "Whether perform unique checking when receiving clauses from other nodes")

//            name,   short-name, must-need, default, comments
#define STR_PARAS \
    STR_PARA( config     , '\0'  ,  false   , "" , "Config file")  \
    STR_PARA( instance   , 'i'   ,  true    , "" , "CNF format instance")
    
struct paras 
{
#define PARA(N, T, S, M, D, L, H, C) \
    T N = D;
    PARAS 
#undef PARA

#define STR_PARA(N, S, M, D, C) \
    std::string N = D;
    STR_PARAS
#undef STR_PARA

    std::unordered_map<std::string, int>    map_int;
    std::unordered_map<std::string, double> map_double;
    std::unordered_map<std::string, char*>  map_string;
    void print_change ();
};

extern paras __global_paras;

#define OPT(N) (__global_paras.N)

#endif