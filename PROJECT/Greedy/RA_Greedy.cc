#include "RA_Greedy.hh"
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <climits>
#include <random>

using namespace std;

// Solver greedy base
void GreedyRASolver(const RA_Input& in, RA_Output& out) {
    vector<unsigned> candidates;

    srand(time(0));

    for (unsigned g = 0; g < in.Games(); g++) {
        const auto& game = in.gamesData[g];
        const auto& division = in.GetDivisionByCode(game.division_code);
        const auto& arena = in.GetArenaByCode(game.arena_code);

        candidates.clear();

        for (unsigned r = rand()%in.Referees(); r < in.Referees(); r++) {
            const auto& referee = in.refereesData[r];
            if (!out.RefereeAvailableForGame(referee, game)) continue;
            if (!out.CanAttendGame(referee, game)) continue;

            candidates.push_back(r);

            // DA CONTROLLARE A RANDOM
            if ( division.min_referees < candidates.size() ) {
                break;
            }   
        }

        sort(candidates.begin(), candidates.end(), [&](unsigned a, unsigned b) {
            const auto& refA = in.refereesData[a];
            const auto& refB = in.refereesData[b];

            bool levelA = refA.level >= division.level;
            bool levelB = refB.level >= division.level;
            if (levelA != levelB) return levelA > levelB;

            if (refA.experience != refB.experience)
                return refA.experience > refB.experience;

            float distA = in.DistanceBetweenArenasAndReferee(arena, refA);
            float distB = in.DistanceBetweenArenasAndReferee(arena, refB);
            return distA < distB;
        });

        vector<string> selected;
        unsigned total_experience = 0;

        for (unsigned i = 0; i < candidates.size() && selected.size() < division.max_referees; i++) {
            const auto& referee = in.refereesData[candidates[i]];

            if (out.IsCompatibleWith(referee, selected)) {
                if (selected.size() < division.min_referees || total_experience < game.experience_required) {
                    out.AssignRefereeToGame(g, referee.code);
                    selected.push_back(referee.code);
                    total_experience += referee.experience;
                }
            }
        }

        if (selected.size() < division.min_referees) {
            for (unsigned r = 0; r < in.Referees(); r++) {
                if (selected.size() >= division.min_referees) break;
                const auto& referee = in.refereesData[r];
                if (find(selected.begin(), selected.end(), referee.code) != selected.end()) continue;
                if (!out.RefereeAvailableForGame(referee, game)) continue;
                if (!out.CanAttendGame(referee, game)) continue;
                if (in.IsRefereeIncompatibleWithTeam(referee.code, game.homeTeam_code) ||
                    in.IsRefereeIncompatibleWithTeam(referee.code, game.guestTeam_code)) continue;
                if (!out.IsCompatibleWith(referee, selected)) continue;

                out.AssignRefereeToGame(g, referee.code);
                selected.push_back(referee.code);
                total_experience += referee.experience;
            }
        }
    }
}

// Solver greedy migliorato con priorità ai giochi più difficili
void GreedyRASolver_Improved(const RA_Input& in, RA_Output& out) {
    // Crea vettore di indici dei giochi
    vector<unsigned> gameIndices(in.Games());
    for (unsigned i = 0; i < in.Games(); i++) {
        gameIndices[i] = i;
    }
    
    // Ordina i giochi per priorità (esperienza richiesta, numero di arbitri necessari)
    sort(gameIndices.begin(), gameIndices.end(), [&](unsigned a, unsigned b) {
        const RA_Input::Game& gameA = in.gamesData[a];
        const RA_Input::Game& gameB = in.gamesData[b];
        
        // Priorità ai giochi con più esperienza richiesta
        if (gameA.experience_required != gameB.experience_required)
            return gameA.experience_required > gameB.experience_required;
        
        // Trova le divisioni
        const RA_Input::Division* divA = nullptr;
        const RA_Input::Division* divB = nullptr;
        
        for (const auto& div : in.divisionsData) {
            if (div.code == gameA.division_code) divA = &div;
            if (div.code == gameB.division_code) divB = &div;
        }
        
        if (divA && divB) {
            // Priorità ai giochi con più arbitri necessari
            if (divA->min_referees != divB->min_referees)
                return divA->min_referees > divB->min_referees;
            
            // Priorità ai giochi con livello più alto
            if (divA->level != divB->level)
                return divA->level > divB->level;
        }
        
        return false;
    });
    
    // Conta le assegnazioni per referee per bilanciare il carico
    vector<unsigned> refereeAssignments(in.Referees(), 0);
    
    // Processa i giochi in ordine di priorità
    for (unsigned gameIdx : gameIndices) {
        const RA_Input::Game& game = in.gamesData[gameIdx];
        
        // Trova la divisione corrispondente
        const RA_Input::Division* division = nullptr;
        for (const auto& div : in.divisionsData) {
            if (div.code == game.division_code) {
                division = &div;
                break;
            }
        }
        
        if (!division) continue;
        
        // Trova l'arena
        RA_Input::Arena arena;
        for (const auto& a : in.arenasData) {
            if (a.code == game.arena_code) {
                arena = a;
                break;
            }
        }
        
        // Crea vettore di coppie (referee_index, score)
        vector<pair<unsigned, float>> refereeScores;
        
        for (unsigned r = 0; r < in.Referees(); r++) {
            const RA_Input::Referee& referee = in.refereesData[r];
            
            // Controlla disponibilità base
            if (!out.RefereeAvailableForGame(referee, game)) continue;
            if (!out.CanAttendGame(referee, game)) continue;
            
            // Calcola score (più alto = migliore)
            float score = 0.0f;
            
            // Bonus per livello adeguato
            if (referee.level >= division->level) score += 100.0f;
            
            // Bonus per esperienza
            score += referee.experience * 10.0f;
            
            // Malus per distanza
            float distance = in.DistanceBetweenArenasAndReferee(arena, referee);
            score -= distance * 0.1f;
            
            // Malus per bilanciamento del carico
            score -= refereeAssignments[r] * 5.0f;
            
            // Bonus se può soddisfare l'esperienza richiesta
            if (referee.experience >= game.experience_required) score += 50.0f;
            
            refereeScores.push_back(make_pair(r, score));
        }
        
        // Ordina per score decrescente
        sort(refereeScores.begin(), refereeScores.end(), [](const pair<unsigned, float>& a, const pair<unsigned, float>& b) {
            return a.second > b.second;
        });
        
        // Assegna gli arbitri migliori
        vector<string> selected;
        for (const auto& refScore : refereeScores) {
            if (selected.size() >= division->max_referees) break;
            
            unsigned refIdx = refScore.first;
            const RA_Input::Referee& referee = in.refereesData[refIdx];
            
            // Controlla compatibilità
            if (out.IsCompatibleWith(referee, selected)) {
                out.AssignRefereeToGame(gameIdx, referee.code);
                selected.push_back(referee.code);
                refereeAssignments[refIdx]++;
                
                // Stop se abbiamo raggiunto il minimo e l'esperienza richiesta
                if (selected.size() >= division->min_referees) {
                    unsigned totalExperience = 0;
                    for (const string& sel : selected) {
                        for (const auto& ref : in.refereesData) {
                            if (ref.code == sel) {
                                totalExperience += ref.experience;
                                break;
                            }
                        }
                    }
                    if (totalExperience >= game.experience_required) break;
                }
            }
        }
        
        // Fallback: assegna arbitri casuali se non abbiamo il minimo
        if (selected.size() < division->min_referees) {
            for (unsigned r = 0; r < in.Referees(); r++) {
                if (selected.size() >= division->min_referees) break;
                
                const RA_Input::Referee& referee = in.refereesData[r];
                
                // Controlla se non è già stato selezionato
                bool already_selected = false;
                for (const string& sel : selected) {
                    if (sel == referee.code) {
                        already_selected = true;
                        break;
                    }
                }
                
                if (!already_selected) {
                    out.AssignRefereeToGame(gameIdx, referee.code);
                    selected.push_back(referee.code);
                    refereeAssignments[r]++;
                }
            }
        }
    }
}