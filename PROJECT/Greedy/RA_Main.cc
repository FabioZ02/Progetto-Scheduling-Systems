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
        
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occurred" << endl;
        return 1;
    }
    
    return 0;
}
