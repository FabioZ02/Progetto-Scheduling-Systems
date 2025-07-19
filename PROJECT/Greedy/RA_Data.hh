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

    // le strutture possono essere private?
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
      vector<pair <tm, pair<tm, tm>>> unavailabilities; // Vector of pairs representing unavailability periods (date, hour)
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
    
    const Division& GetDivisionByCode(const string& code) const;  // Returns the Division object by its code
    const Referee& GetRefereeByCode(const string& code) const;    // Returns the Referee object by its code
    const Arena& GetArenaByCode(const string& code) const;        // Returns the Arena object by its code
    const Team& GetTeamByCode(const string& code) const;          // Returns the Team object by its code
    
    bool AreRefereesIncompatible(const string& code1, const string& code2) const; // Checks if two referees are incompatible
    bool IsRefereeIncompatibleWithTeam(const string& referee_code, const string& team_code) const; // Checks if a referee is incompatible with a team

  private:
    unsigned divisions, referees, arenas, teams, games;

    float EuclideanDistance(const pair<float, float>& c1, const pair<float, float>& c2) const;
};

class RA_Output{
  friend ostream& operator<<(ostream& os, const RA_Output& out);
  friend istream& operator>>(istream& is, RA_Output& out);
  friend bool operator==(const RA_Output& out1, const RA_Output& out2);

  public:
    RA_Output(const RA_Input& i);
    RA_Output& operator=(const RA_Output& out);

    bool RefereeAvailableForGame(const RA_Input::Referee& referee, const RA_Input::Game& game) const;
    bool CanAttendGame(const RA_Input::Referee& referee, const RA_Input::Game& game) const;
    bool IsCompatibleWith(const RA_Input::Referee& referee, const vector<string>& selected_referees) const;   

    // Hard constraints
    bool NumberOfReferees() const;
    bool FeasibleDistance() const;
    bool RefereeAvailability() const;
    bool Feasibility() const;

    float TotalDistance() const;

    void AssignRefereeToGame(unsigned game_id, const string& referee_code);

    unsigned ComputeViolations() const;
    unsigned ComputeCost() const;

    void Reset();
    void Dump(ostream& os) const;

  private:
    const RA_Input& in;
    vector<vector<string>> gameAssignments; // Vector of vectors to store assigned referees for each game
    vector<vector<unsigned>> teamAssignmentsPerReferee;  // Vector of vectors to store the number of times a referee 
                                                         // has been assigned to a specific team

    const vector<string>& AssignedRefereesToGame(unsigned game_id) const;
    const vector<pair<tm, pair<tm, unsigned>>> GamesAssignedToReferee(const string& referee_code) const; // (date, (time, game_id))

    // soft constraints
    unsigned RefereeLevel() const;
    unsigned ExperienceNeeded() const;
    unsigned GameDistribution() const;
    unsigned OptionalReferee() const;
    unsigned AssignmentFrequency() const;
    unsigned RefereeIncompatibility() const;
    unsigned TeamIncompatibility() const;
};
#endif