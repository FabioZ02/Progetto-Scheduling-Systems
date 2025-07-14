#include "RA_Data.hh"
#include <iostream>
#include <exception>

using namespace std;

int main() {
    try {
        RA_Input input("./Instances/RA-1-5.txt");
        cout << "Input data loaded successfully.\n";

        // Opzionale: stampa dettagli completi
        cout << "\nDetailed data:\n" << input;

        // Test distanza tra prima arena e primo referee
        if (!input.arenasData.empty() && !input.refereesData.empty()) {
            const auto& arena = input.arenasData[1];
            const auto& referee = input.refereesData[1];
            float dist = input.DistanceBetweenArenasAndReferee(arena, referee);
            cout << "\nDistanza tra arena '" << arena.code << "' e referee '" << referee.code << "': " << dist << endl;
        } else {
            cout << "No arenas or referees loaded!" << endl;
        }
        RA_Output output(input);
        cout << "Output object created successfully.\n";

        // Opzionale: stampa dettagli dell'output
        cout << "\nOutput data:\n" << output;
        // Test assegnazione di un referee a una partita
        if (!input.gamesData.empty() && !input.refereesData.empty()) {
            unsigned game_id = 0; // Assumiamo che la prima partita esista
            const string& referee_code = input.refereesData[0].code; // Assumiamo che il primo referee esista
            output.AssignRefereeToGame(game_id, referee_code);
            cout << "\nAssigned referee '" << referee_code << "' to game ID " << game_id << ".\n";
            const auto& assigned_refs = output.AssignedRefereesToGame(game_id);
            for (const auto& ref : assigned_refs) {
                cout << ref << " - ";
            }
            cout << endl;
        } else {
            cout << "No games or referees available for assignment!" << endl;
        }

        output.AssignRefereeToGame(0, "R1");
        output.AssignRefereeToGame(0, "R2");

        output.AssignRefereeToGame(1, "R3");

        output.AssignRefereeToGame(2, "R1");
        output.AssignRefereeToGame(2, "R5");
        cout << "\nDump dell'output corrente:\n" << output;
        cout << "\nMinimumReferees: " << output.MinimumReferees() << endl;
        cout << "FeasibleDistance: " << output.FeasibleDistance() << endl;
        cout << "RefereeAvailability: " << output.RefereeAvailability() << endl;
        cout << "Feasibility (tutti i vincoli): " << output.Feasibility() << endl;


    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occurred" << endl;
        return 1;
    }
    return 0;
}
