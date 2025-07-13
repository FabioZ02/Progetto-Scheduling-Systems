// File RA_Data.hh
#ifndef RA_DATA_HH
#define RA_DATA_HH
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

class RA_Input
{
  friend ostream& operator<<(ostream& os, const RA_Input& in);
public:
  RA_Input(string file_name);
  
  // Getters for the problem parameters
  unsigned Divisions() const { return divisions; }
  unsigned Referees() const { return referees; }
  unsigned Arenas() const { return arenas; }
  unsigned Teams() const { return teams; }
  unsigned Games() const { return games; }
  
  // Getters for the distance matrices
  float DistanceBetweenArenas(unsigned a1, unsigned a2) const {return distanceBetweenArenas[a1][a2];}
  float DistanceBetweenArenasAndReferee(unsigned a, unsigned r) const {return distanceBetweenArenasAndReferee[a][r];}


public:
  unsigned divisions, referees, arenas, teams, games;
  // Distance matrices
  vector<vector<float>> distanceBetweenArenas;
  vector<vector<float>> distanceBetweenArenasAndReferee;
  vector<vector<float>> travelTimeBetweenArenas;
  vector<vector<float>> travelTimeBetweenArenasAndReferee;
  void ComputeDistances();
  void ComputeTravelTimes();

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
  vector<Referee> refereesData;                         // Vector of referees

  // Arena Data structure
  struct Arena {
    string code;                          // A1 Unique identifier for the arena
    pair <float, float> coordinates;  // Coordinates of the arena (x, y)
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
    string date;                  // Date of the game (day, month, year)
    string time;                  // Time of the game (hour, minute)
    string arena_code;            // A1 Code of the arena where the game is played
    unsigned experience_required;   // INT Minimum level of experience required by the referees for this game
  };
  vector<Game> gamesData;           // Vector of games

};

class RA_Output 
{
  friend ostream& operator<<(ostream& os, const RA_Output& out);
  friend istream& operator>>(istream& is, RA_Output& out);
  friend bool operator==(const RA_Output& out1, const RA_Output& out2);
public:
  RA_Output(const RA_Input& i);
  RA_Output& operator=(const RA_Output& out);
  float DistanceArenas(unsigned a1, unsigned a2) const { return in.DistanceBetweenArenas(a1, a2); }
  float DistanceArenasAndReferee(unsigned r, unsigned a) const { return in.DistanceBetweenArenasAndReferee(r, a); }
  float TravelTimeArenas(unsigned a1, unsigned a2) const { return in.travelTimeBetweenArenas[a1][a2]; }
  float TravelTimeArenasAndReferee(unsigned r, unsigned a) const { return in.travelTimeBetweenArenasAndReferee[a][r]; }
  bool MinimumReferees() const;
  bool FeasibleDistance() const;
  bool RefereeAvailability() const;
  bool Feasibility() const;
  void AssignRefereetoGame(unsigned game_id, const string& referee_code);
  const vector<string>& AssignedReferees(unsigned game_id) const;
  unsigned ComputeCost() const;
  unsigned ComputeExperienceNeeded() const;
  unsigned ComputeGameDistribution() const;
  unsigned ComputeMinDistanceCost() const;
  unsigned CoputeOptionalRefereee() const;
  unsigned ComputeAssignmentFrequency() const;
  unsigned ComputeRefereeCompatibility() const;
  unsigned ComputeTeamCompatibilityCost() const;
  void Reset();
  void Dump(ostream& os) const;
private:
  const RA_Input& in;
  vector<vector<string>> gameAssignments; 
};
#endif
