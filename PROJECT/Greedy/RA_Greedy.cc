#include "RA_Greedy.hh"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// Greedy solver for the ref assignment problem
void GreedyRASolver(const RA_Input& in, RA_Output& out) {
    // Reset the output
    out.Reset();

    // For each game
    for (unsigned g = 0; g < in.Games(); ++g) {
        const auto& game = in.gamesData[g];

        vector<string> selected_refs; // Get the already assigned referees for the game

        // Referee list for the current game
        for (const auto& ref : in.refereesData) {
            
            // Check if the referee is available for the game
            if (!out.RefereeAvailableForGame(ref, game)) continue;
            // Check if the referee can attend the game
            if (!out.CanAttendGame(ref, game)) continue;
            // Check if the referee is compatible with the already selected referees
            if (!out.IsCompatibleWith(ref, selected_refs)) continue;
            
            // If all checks pass, add the referee to the selected list
            selected_refs.push_back(ref.code);

            if( out.Feasibility() ) {
                break; // Stop searching if we have a feasible solution
            }
        }

        // If we have a feasible solution, assign the referee to the game
        for (const auto& r : selected_refs)
            out.AssignRefereeToGame(g, r);
        
        selected_refs.clear(); // Clear the selected referees for the next game
    }

    // Calcola il costo totale
    out.ComputeCost();
}