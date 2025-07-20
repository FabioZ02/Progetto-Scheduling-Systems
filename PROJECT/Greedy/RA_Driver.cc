#include <iostream>
#include <cstdlib>

#include "RA_Data.hh"
#include "RA_Greedy.hh"

#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <climits>
#include <random>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        exit(1);
    }

    RA_Input in(argv[1]);
    float best_distance = numeric_limits<float>::max();
    RA_Output best_output(in);
    unsigned best_cost = numeric_limits<unsigned>::max();
    
    for (int trial = 0; trial < 20; trial++) {
        RA_Output trial_output(in);
        GreedyRASolver_Improved(in, trial_output); // or Improved

        // It saves the best output based only on the total cost
        float dist = trial_output.TotalDistance();
        if (best_output.ComputeCost() < best_cost) {
            best_distance = dist;
            best_output = trial_output;
        }
  }

    if (in.Games() <= 50000) // print the solution only if small enough
        cout << best_output << endl;
    // cout << "Violations: " << best_output.ComputeViolations() << endl;
    cout << "Cost: " << best_output.ComputeCost() << endl;
    cout << "Total distance: " << best_distance << " km" << endl;

    return 0;
}

