#include <iostream>
#include <cstdlib>
#include "RA_Data.hh"
#include "RA_Greedy.hh"

int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      cerr << "Usage: " << argv[0] << " <input_file>" << endl;
      exit(1);
    }

  RA_Input in(argv[1]);
  RA_Output out(in);

  GreedyRASolver(in, out);

  if (in.Games() <= 50000000) // print the solution only if small enough
    cout << out << endl;
  cout << "Number of violations: " << out.ComputeViolations() << endl;
  cout << "Cost: " << out.ComputeCost() << endl;
  return 0;
}

