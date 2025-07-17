#include "RA_Data.hh"
#include <iostream>
#include <exception>

using namespace std;

int tryOn() {
    try {
        RA_Input input("./Instances/RA-1-5.txt");
        // cout << "Input data loaded successfully.\n";
        // cout << "\nDetailed data:\n" << input;

        RA_Output output(input);
        // cout << "Output object created successfully.\n";
        // cout << "\nOutput data:\n" << output;

        output.AssignRefereeToGame(0, "R1");
        output.AssignRefereeToGame(0, "R1");
        output.AssignRefereeToGame(0, "R2");
        output.AssignRefereeToGame(1, "R3");
        output.AssignRefereeToGame(2, "R1");
        output.AssignRefereeToGame(2, "R5");

        // cout << "\nDump dell'output corrente:\n" << output;
        cout << "\nMinimumReferees: " << output.NumberOfReferees() << endl;
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
