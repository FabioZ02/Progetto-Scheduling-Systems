#include <iostream>
#include "RA_Data.hh"

// int main() {
//     RA_Input in("/home/fabio/ESAME_SCHEDULING/PROJECT/Greedy/Instances/RA-1-5.txt");
//     if (in.arenasData.empty() || in.refereesData.empty()) {
//         std::cerr << "No arenas or referees loaded!" << std::endl;
//         return 1;
//     }
//     // Prendi il primo arena e il primo referee
//     const auto& arena = in.arenasData[0];
//     const auto& referee = in.refereesData[0];
//     float dist = in.ComputeDistancesBetweenArenasAndReferees(arena, referee);
//     std::cout << "Distanza tra arena '" << arena.code << "' e referee '" << referee.code << "': " << dist << std::endl;
//     return 0;
// }
