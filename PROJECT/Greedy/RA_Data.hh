// File RA_Data.hh
#ifndef RA_DATA_HH
#define RA_DATA_HH
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

class RA_Input{
  friend ostream& operator<<(ostream& os, const RA_Input& in);
public:
  RA_Input(string file_name);
  
  // Getters for the problem parameters
  unsigned Divisions() const { return divisions; }
  unsigned Referees() const { return referees; }
  unsigned Arenas() const { return arenas; }
  unsigned Teams() const { return teams; }
  unsigned Games() const { return games; }

  // Division Data structure
  struct Division {
    string code;                // D1 Unique identifier for the division
    unsigned min_referees;        // INT Minimum number of referees required for the division
    unsigned max_referees;        // INT Maximum number of referees allowed for the division
    unsigned level;               // INT Minimum level of experience required by a referee
    unsigned teams;               // INT Number of teams in the division
  };
  vector<Division> divisionsData; // Vector of divisions

  // Referee Data structure
  struct Referee {
    string code;                                      // R1 Unique identifier for the referee
    unsigned level;                                     // INT Level of the referee
    pair <float, float> coordinates;              // Coordinates of the referee (x, y)
    unsigned experience;                                // INT Experience of the referee
    vector<string> incompatible_referees;             // Vector of referees that this referee cannot work with
    vector<string> incompatible_teams;                // Vector of teams that this referee cannot officiate
    vector<pair <string, string>> unavailabilities; // Vector of pairs representing unavailability periods (date, hour)
  };
  vector<Referee> refereesData;   
  
  // Arena Data structure
  struct Arena {
    string code;                          // A1 Unique identifier for the arena
    pair <float, float> coordinates;      // Coordinates of the arena (x, y)
  };
  vector<Arena> arenasData;                 // Vector of arenas

  // Teams Data structure
  struct Team {
    string code;           // T1 Unique identifier for the team
    string division_code;  // D1 Code of the division to which the team belongs
  };
  vector<Team> teamsData;    // Vector of teams

  // Games Data structure
  struct Game {
    string homeTeam_code;         // T1 Code of the first team
    string guestTeam_code;        // T2 Code of the second team
    string division_code;         // D1 Code of the division to which the game belongs
    tm date;                  // Date of the game (day, month, year)
    tm time;                  // Time of the game (hour, minute)
    string arena_code;            // A1 Code of the arena where the game is played
    unsigned experience_required;   // INT Minimum level of experience required by the referees for this game
  };
  vector<Game> gamesData;           // Vector of games


  // Getters for the distance matrices
  float DistanceBetweenArenas(Arena a1, Arena a2) const {return ComputeDistancesBetweenArenas(a1, a2);}
  float DistanceBetweenArenasAndReferee(Arena a, Referee r) const {return ComputeDistancesBetweenArenasAndReferees(a, r);}
  float TravelTimeBetweenArenas(Arena a1, Arena a2) const {return ComputeTravelTimeBetweenArenas(a1, a2);}
  float TravelTimeBetweenArenasAndReferee(Arena a, Referee r) const {return ComputeTravelTimeBetweenArenasAndReferee(a, r);}

  
public:
  unsigned divisions, referees, arenas, teams, games;

  // probabilmente vanno nell'output
  float ComputeDistancesBetweenArenas(Arena a1, Arena a2) const;
  float ComputeDistancesBetweenArenasAndReferees(Arena a, Referee r) const;
  float ComputeTravelTimeBetweenArenas(Arena a1, Arena a2) const;
  float ComputeTravelTimeBetweenArenasAndReferee(Arena a, Referee r) const;
};

class RA_Output{
  friend ostream& operator<<(ostream& os, const RA_Output& out);
  friend istream& operator>>(istream& is, RA_Output& out);
  friend bool operator==(const RA_Output& out1, const RA_Output& out2);

public:
  RA_Output(const RA_Input& i);
  RA_Output& operator=(const RA_Output& out);
  
  bool MinimumReferees() const;
  bool FeasibleDistance() const;
  bool RefereeAvailability() const;
  bool Feasibility() const;

  void AssignRefereeToGame(unsigned game_id, const string& referee_code);
  const vector<string>& AssignedReferees(unsigned game_id) const;

  unsigned ComputeCost() const;

  void Reset();
  void Dump(ostream& os) const;

private:
  const RA_Input& in;
  vector<vector<string>> gameAssignments;

  float DistanceBetweenArenas(RA_Input::Arena a1, RA_Input::Arena a2) const { return in.DistanceBetweenArenas(a1, a2); }
  float DistanceBetweenArenasAndReferee(RA_Input::Arena a, RA_Input::Referee r) const { return in.DistanceBetweenArenasAndReferee(a, r); }
  float TravelTimeBetweenArenas(RA_Input::Arena a1, RA_Input::Arena a2) const { return in.TravelTimeBetweenArenas(a1, a2); }
  float TravelTimeBetweenArenasAndReferee(RA_Input::Arena a, RA_Input::Referee r) const { return in.TravelTimeBetweenArenasAndReferee(a, r); }


  // unsigned ComputeExperienceNeeded() const;
  // unsigned ComputeGameDistribution() const;
  // unsigned ComputeMinDistanceCost() const;
  // unsigned CoputeOptionalRefereee() const;
  // unsigned ComputeAssignmentFrequency() const;
  // unsigned ComputeRefereeCompatibility() const;
  // unsigned ComputeTeamCompatibilityCost() const;
};
#endif