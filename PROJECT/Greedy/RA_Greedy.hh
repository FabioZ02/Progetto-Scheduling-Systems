// File RA_Greedy.hh
#ifndef RA_GREEDY_HH
#define RA_GREEDY_HH

#include "RA_Data.hh"

// Solver greedy base
void GreedyRASolver(const RA_Input& in, RA_Output& out);

// Solver greedy migliorato con prioritizzazione dei giochi
void GreedyRASolver_Improved(const RA_Input& in, RA_Output& out);

#endif