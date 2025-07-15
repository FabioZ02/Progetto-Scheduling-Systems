#include <iostream>
#include <cstdlib>
#include "RA_Data.hh"

int main(int argc, char* argv[])
{
    RA_Input in("/home/fabio/ESAME_SCHEDULING/PROJECT/Greedy/Instances/RA-1-5.txt");
    std::cout << in << std::endl; // Stampa tutti i dati parsati
    // Puoi aggiungere qui altri controlli, ad esempio:
    std::cout << "Numero divisioni: " << in.Divisions() << std::endl;
    std::cout << "Numero arbitri: " << in.Referees() << std::endl;
    std::cout << "Numero arene: " << in.Arenas() << std::endl;
    std::cout << "Numero squadre: " << in.Teams() << std::endl;
    std::cout << "Numero partite: " << in.Games() << std::endl;

    // Test distanza tra prima arena e primo referee
    if (!in.arenasData.empty() && !in.refereesData.empty()) {
        const auto& arena = in.arenasData[0];
        const auto& referee = in.refereesData[0];
        float dist = in.ComputeDistancesBetweenArenasAndReferees(arena, referee);
        std::cout << "Distanza tra arena '" << arena.code << "' e referee '" << referee.code << "': " << dist << std::endl;
    } else {
        std::cout << "No arenas or referees loaded!" << std::endl;
    }
    return 0;
}
