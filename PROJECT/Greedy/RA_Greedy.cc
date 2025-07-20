#include "RA_Greedy.hh"
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <climits>
#include <random>

using namespace std;

// Base greedy solver for referee assignment problem with more soft constraints 
void GreedyRASolver(const RA_Input& in, RA_Output& out) {
    vector<unsigned> candidates;
    srand(time(0));  // Initialize random seed for stochastic behavior

    // Iterate over all games
    for (unsigned g = 0; g < in.Games(); g++) {
        const auto& game = in.gamesData[g];
        const auto& division = in.GetDivisionByCode(game.division_code);
        const auto& arena = in.GetArenaByCode(game.arena_code);

        candidates.clear();  // Clear candidate list for current game

        // Generate and shuffle referee indices to introduce randomness in candidate selection
        vector<unsigned> ref_indices(in.Referees());
        iota(ref_indices.begin(), ref_indices.end(), 0);
        shuffle(ref_indices.begin(), ref_indices.end(), default_random_engine(time(0)));

        // Select candidate referees who are available and can attend the game
        for (unsigned i = 0; i < ref_indices.size(); ++i) {
            unsigned r = ref_indices[i];
            const auto& referee = in.refereesData[r];

            // Check hard constraints: availability and feasibility (e.g., travel time)
            if (!out.RefereeAvailableForGame(referee, game)) continue;
            if (!out.CanAttendGame(referee, game)) continue;

            candidates.push_back(r);

            // Randomized early stopping: more likely to stop as we approach max referees
            float stopProbability = float(candidates.size()) / float(division.max_referees);
            if (rand() / float(RAND_MAX) < stopProbability && candidates.size() > division.min_referees) break;
        }

        // Sort candidates using a custom score function based on soft preferences
        sort(candidates.begin(), candidates.end(), [&](unsigned a, unsigned b) {
            const auto& refA = in.refereesData[a];
            const auto& refB = in.refereesData[b];

            // Score function prioritizes level, experience, and proximity
            auto compute_score = [&](const RA_Input::Referee& ref) {
                float score = 0.0f;
                if (ref.level >= division.level) score += ref.level * 70.0f;
                score += ref.experience * 50.0f;
                score -= in.DistanceBetweenArenasAndReferee(arena, ref) * 0.5f;  // Penalize distant referees
                return score;
            };

            return compute_score(refA) > compute_score(refB);  // Descending order
        });

        vector<string> selected;
        unsigned total_experience = 0;

        // Assign referees from sorted candidates, respecting max_referees
        for (unsigned i = 0; i < candidates.size() && selected.size() <= division.max_referees; i++) {
            // Stop early if experience and minimum referee count are satisfied
            if (selected.size() > division.min_referees && total_experience >= game.experience_required) break;

            const auto& referee = in.refereesData[candidates[i]];

            // Check compatibility with already assigned referees and required level
            if (out.IsCompatibleWith(referee, selected) && referee.level >= division.level) {
                out.AssignRefereeToGame(g, referee.code);
                selected.push_back(referee.code);
                total_experience += referee.experience;
            }
        }

        // Fallback: assign additional referees in a second pass if not enough were selected
        if (selected.size() < division.min_referees) {
            for (unsigned r = 0; r < in.Referees(); r++) {
                // Stop if both referee count and required experience are met
                if (selected.size() > division.min_referees && total_experience >= game.experience_required) break;

                const auto& referee = in.refereesData[r];

                // Skip if referee is already selected
                if (find(selected.begin(), selected.end(), referee.code) != selected.end()) continue;

                // Check again basic feasibility and incompatibilities
                if (!out.RefereeAvailableForGame(referee, game)) continue;
                if (!out.CanAttendGame(referee, game)) continue;
                if (in.IsRefereeIncompatibleWithTeam(referee.code, game.homeTeam_code) ||
                    in.IsRefereeIncompatibleWithTeam(referee.code, game.guestTeam_code)) continue;
                if (!out.IsCompatibleWith(referee, selected)) continue;

                // Assign referee in fallback mode
                out.AssignRefereeToGame(g, referee.code);
                selected.push_back(referee.code);
                total_experience += referee.experience;
            }
        }
    }
}

/// Improved greedy solver with prioritization of harder games
void GreedyRASolver_Improved(const RA_Input& in, RA_Output& out) {
    // Create a vector of game indices [0, ..., Games()-1]
    vector<unsigned> gameIndices(in.Games());
    for (unsigned i = 0; i < in.Games(); i++) {
        gameIndices[i] = i;
    }

    // Sort games by priority: experience required, number of referees, division level
    sort(gameIndices.begin(), gameIndices.end(), [&](unsigned a, unsigned b) {
        const RA_Input::Game& gameA = in.gamesData[a];
        const RA_Input::Game& gameB = in.gamesData[b];

        // Prefer games with higher required experience
        if (gameA.experience_required != gameB.experience_required)
            return gameA.experience_required > gameB.experience_required;

        // Retrieve divisions of the games
        const RA_Input::Division* divA = &in.GetDivisionByCode(gameA.division_code);
        const RA_Input::Division* divB = &in.GetDivisionByCode(gameB.division_code);

        if (divA && divB) {
            // Prefer games requiring more referees
            if (divA->min_referees != divB->min_referees)
                return divA->min_referees > divB->min_referees;

            // Prefer higher-level divisions
            if (divA->level != divB->level)
                return divA->level > divB->level;
        }

        // No preference
        return false;
    });

    // Track how many games each referee has been assigned to
    vector<unsigned> refereeAssignments(in.Referees(), 0);

    // Process each game in the sorted (prioritized) order
    for (unsigned gameIdx : gameIndices) {
        const RA_Input::Game& game = in.gamesData[gameIdx];

        // Get the corresponding division and arena
        const RA_Input::Division* division = &in.GetDivisionByCode(game.division_code);
        if (!division) continue;

        RA_Input::Arena arena = in.GetArenaByCode(game.arena_code);

        // List of candidate referees with computed scores
        vector<pair<unsigned, float>> refereeScores;

        for (unsigned r = 0; r < in.Referees(); r++) {
            const RA_Input::Referee& referee = in.refereesData[r];

            // Check basic constraints: availability and feasibility
            if (!out.RefereeAvailableForGame(referee, game)) continue;
            if (!out.CanAttendGame(referee, game)) continue;

            float score = 0.0f;

            // Reward adequate level
            if (referee.level >= division->level) score += referee.level * 70.0f;

            // Reward experience
            score += referee.experience * 10.0f;

            // Penalize distance
            score -= in.DistanceBetweenArenasAndReferee(arena, referee) * 0.5f;

            // Penalize overused referees (load balancing)
            score -= refereeAssignments[r] * 5.0f;

            // Reward if experience satisfies game's required experience
            if (referee.experience >= game.experience_required) score += 50.0f;

            refereeScores.emplace_back(r, score);
        }

        // Sort referees by descending score
        sort(refereeScores.begin(), refereeScores.end(), [](const pair<unsigned, float>& a, const pair<unsigned, float>& b) {
            return a.second > b.second;
        });

        vector<string> selected;

        unsigned totalExperience = 0;

        // Assign referees greedily based on scores
        for (const auto& refScore : refereeScores) {
            if (selected.size() >= division->max_referees) break;

            unsigned refIdx = refScore.first;
            const RA_Input::Referee& referee = in.refereesData[refIdx];

            // Check compatibility with previously selected referees
            if (out.IsCompatibleWith(referee, selected)) {
                out.AssignRefereeToGame(gameIdx, referee.code);
                selected.push_back(referee.code);
                refereeAssignments[refIdx]++;
                totalExperience += in.refereesData[refIdx].experience;

                // Stop early if min referees and experience are satisfied
                if (selected.size() >= division->min_referees && totalExperience >= game.experience_required) break;
            }
        }

        // Fallback: try to assign more referees if minimum is not met
        if (selected.size() < division->min_referees) {
            for (unsigned r = 0; r < in.Referees(); r++) {
                if (selected.size() >= division->min_referees) break;

                const RA_Input::Referee& referee = in.refereesData[r];

                // Skip already selected referees
                if (find(selected.begin(), selected.end(), referee.code) != selected.end()) continue;

                // Basic feasibility checks
                if (!out.RefereeAvailableForGame(referee, game)) continue;
                if (!out.CanAttendGame(referee, game)) continue;

                // Assign if compatible
                if (out.IsCompatibleWith(referee, selected)) {
                    out.AssignRefereeToGame(gameIdx, referee.code);
                    selected.push_back(referee.code);
                    refereeAssignments[r]++;
                }
            }
        }
    }
}