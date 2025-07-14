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
            const auto& arena = input.arenasData[4];
            const auto& referee = input.refereesData[1];
            float dist = input.ComputeTravelTimeBetweenArenasAndReferee(arena, referee); // Converti in minuti
            cout << "\nDistanza tra arena '" << arena.code << "' e referee '" << referee.code << "': " << dist << endl;
        } else {
            cout << "No arenas or referees loaded!" << endl;
        }
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occurred" << endl;
        return 1;
    }
    return 0;
}
