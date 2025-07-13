// File RA_Data.cc
#include "RA_Data.hh"
#include <fstream>
#include <cmath>
#include <iostream>
#include <utility>

RA_Input::RA_Input(string file_name)
{  
  const unsigned MAX_DIM = 100;
  unsigned d,r,a,t,g;
  string incompatible_referee, incompatible_teams, unavailable_date, unavailable_time;
  char ch, buffer[MAX_DIM];

  ifstream is(file_name);
  if(!is)
  {
    cerr << "Cannot open input file " <<  file_name << endl;
    exit(1);
  }
  
  is >> buffer >> ch >> divisions >> ch;
  is >> buffer >> ch >> referees >> ch;
  is >> buffer >> ch >> arenas >> ch;
  is >> buffer >> ch >> teams >> ch;
  is >> buffer >> ch >> games >> ch;

  
  divisionsData.resize(divisions);
  refereesData.resize(referees);
  arenasData.resize(arenas);
  teamsData.resize(teams);
  gamesData.resize(games);
  distanceBetweenArenas.resize(arenas, vector<float>(arenas));
  distanceBetweenArenasAndReferee.resize(arenas, vector<float>(referees));
  travelTimeBetweenArenas.resize(arenas, vector<float>(arenas));
  travelTimeBetweenArenasAndReferee.resize(arenas, vector<float>(referees));

  // read divisions
  is.ignore(MAX_DIM, '\n');           // ignore the first line (header)
  for (d = 0; d < divisions; d++)
  {
    is >> divisionsData[d].code >> ch; // read "D1"
    is.ignore(MAX_DIM, ':');           // ignore the ':' character
    is >> divisionsData[d].min_referees >> ch; // read "MinReferees,"
    is >> divisionsData[d].max_referees >> ch; // read "MaxReferees,"
    is >> divisionsData[d].level >> ch; // read "Level,"
    is >> divisionsData[d].teams >> ch; // read "Teams"
  }

  // read referees
  is.ignore(MAX_DIM, '\n');           // ignore the first line (header)
  for (r = 0; r < referees; r++)
  {
    is >> refereesData[r].code >> ch; // read "R1"
    is >> refereesData[r].level >> ch; // read "Level,"
    is.ignore(MAX_DIM, '(');           // ignore the '(' character
    is >> refereesData[r].coordinates.first >> ch; // read "Coordinates"
    is >> refereesData[r].coordinates.second >> ch; // read "Coordinates"
    is.ignore(MAX_DIM, ')');           // ignore the ')' character
    is >> refereesData[r].experience >> ch; // read "Experience,"
    is.ignore(MAX_DIM, '[');            // ignore the '[' character
    while (is.peek() != ']')            // read incompatible referees
    {
      is >> incompatible_referee >> ch; // read "IncompatibleReferee,"
      refereesData[r].incompatible_referees.push_back(incompatible_referee);
    }
    is.ignore(MAX_DIM, ']');            // ignore the ']' character
    is.ignore(MAX_DIM, '[');            // ignore the '[' character
    while (is.peek() != ']')            // read incompatible teams
    {
      is >> incompatible_teams >> ch; // read "IncompatibleTeam,"
      refereesData[r].incompatible_teams.push_back(incompatible_teams);
    }
    is.ignore(MAX_DIM, ']');            // ignore the ']' character
    is.ignore(MAX_DIM, '[');            // ignore the '[' character
    while (is.peek() != ']')            // read unavailabilities
    {
      is >> unavailable_date >> ch; // read "UnavailableDate,"
      is >> unavailable_time >> ch; // read "UnavailableTime,"
      refereesData[r].unavailabilities.push_back(make_pair(unavailable_date, unavailable_time));
    }
    is.ignore(MAX_DIM, ']');            // ignore the ']' character
  }

  // read Arenas
  is.ignore(MAX_DIM, '\n');             // ignore rest of the line
  for(a=0; a < arenas; a++)
  {
    is >> arenasData[a].code >> ch;
    is.ignore(MAX_DIM, '(');
    is >> arenasData[a].coordinates.first >> ch;
    is >> arenasData[a].coordinates.second >> ch;
    is.ignore(MAX_DIM, ')');
  }

  // read teams
  is.ignore(MAX_DIM, '\n');
  for(t=0; t < teams; t++)
  {
    is >> teamsData[t].code >> ch;
    is >> teamsData[t].division_code >> ch;
  }

  // read Games
  is.ignore(MAX_DIM, '\n');
  for(g=0; g < games; g++)
  {
    is >> gamesData[g].homeTeam_code >> ch;
    is >> gamesData[g].guestTeam_code >> ch;
    is >> gamesData[g].division_code >> ch;
    is >> gamesData[g].date >> ch;
    is >> gamesData[g].time >> ch;
    is >> gamesData[g].arena_code >> ch;
    is >> gamesData[g].experience_required >> ch;
  }
}

// Function that fill the distance matrices, using euclidean distance
void RA_Input::ComputeDistances()
{
  for(unsigned a1 = 0; a1 < arenas; a1++)
  {
    for(unsigned a2 = 0; a2 < arenas; a2++)
    {
      float dx = arenasData[a1].coordinates.first - arenasData[a2].coordinates.first;
      float dy = arenasData[a1].coordinates.second - arenasData[a2].coordinates.second;
      distanceBetweenArenas[a1][a2] = sqrt(dx * dx + dy * dy);
    }
  }
  for(unsigned a = 0; a < arenas; a++)
  {
    for(unsigned r = 0; r < referees; r++)
    {
      float dx = arenasData[a].coordinates.first - refereesData[r].coordinates.first;
      float dy = arenasData[a].coordinates.second - refereesData[r].coordinates.second;
      distanceBetweenArenasAndReferee[a][r] = sqrt(dx * dx + dy * dy);
    }
  }
}

// Function that fill the travel time matrices, using constant speed
void RA_Input::ComputeTravelTimes()
{
  const float speed = 60.0;
  for(unsigned a1 = 0; a1 < arenas; a1++)
  {
    for(unsigned a2 = 00; a2 < arenas; a2++)
    {
      travelTimeBetweenArenas[a1][a2] = distanceBetweenArenas[a1][a2] / speed;
    }
  }
  for(unsigned r = 0; r < referees; r++)
  {
    for(unsigned a = 0; a < arenas; a++)
    {
      travelTimeBetweenArenasAndReferee[r][a] = distanceBetweenArenasAndReferee[r][a] / speed;
    }
  }
}

ostream& operator<<(ostream& os, const RA_Input& in)
{
 
  os << "Divisions = " << in.divisions << ";\n";
  os << "Referees = " << in.referees << ";\n";
  os << "Arenas = " << in.arenas << ";\n";
  os << "Teams = " << in.teams << ";\n";
  os << "Games = " << in.games << ";\n\n";

  // DIVISIONS
  os << "DIVISIONS % code, min referees, max referees, level, teams\n";
  for (const auto& d : in.divisionsData) {
    os << d.code << ": " << d.min_referees << ", " << d.max_referees
       << ", " << d.level << ", " << d.teams << "\n";
  }
  os << "\n";

  // REFEREES
  os << "REFEREES % code, level, coordinates, experience, incompatible referees, incompatible teams, unavailabilities\n";
  for (const auto& r : in.refereesData) {
    os << r.code << ", " << r.level << ", (" << r.coordinates.first << ", " << r.coordinates.second << "), "
       << r.experience << ", [";
    for (size_t i = 0; i < r.incompatible_referees.size(); ++i) {
      os << r.incompatible_referees[i];
      if (i < r.incompatible_referees.size() - 1) os << ", ";
    }
    os << "], [";
    for (size_t i = 0; i < r.incompatible_teams.size(); ++i) {
      os << r.incompatible_teams[i];
      if (i < r.incompatible_teams.size() - 1) os << ", ";
    }
    os << "], [";
    for (size_t i = 0; i < r.unavailabilities.size(); ++i) {
      os << r.unavailabilities[i].first << " " << r.unavailabilities[i].second;
      if (i < r.unavailabilities.size() - 1) os << ", ";
    }
    os << "]\n";
  }
  os << "\n";

  // ARENAS
  os << "ARENAS % code, coordinates\n";
  for (const auto& a : in.arenasData) {
    os << a.code << " (" << a.coordinates.first << ", " << a.coordinates.second << ")\n";
  }
  os << "\n";

  // TEAMS
  os << "TEAMS % name, division\n";
  for (const auto& t : in.teamsData) {
    os << t.code << " " << t.division_code << "\n";
  }
  os << "\n";

  // GAMES
  os << "GAMES % Home team, guest team, division, date, time, arena, experience\n";
  for (const auto& g : in.gamesData) {
    os << g.homeTeam_code << " " << g.guestTeam_code << " " << g.division_code << " "
       << g.date << " " << g.time << " " << g.arena_code << " " << g.experience_required << "\n";
  }

  return os;
}

RA_Output::RA_Output(const RA_Input& my_in): in(my_in)
{
  gameAssignments.resize(in.Games());
} 

RA_Output& RA_Output::operator=(const RA_Output& out)
{
  gameAssignments = out.gameAssignments;
  return *this;
}

bool RA_Output::Feasibility() const
{
  return MinimumReferees() && FeasibleDistance() && RefereeAvailability();
}

// The number of mandatory referees must always be assigned to each game.
bool RA_Output::MinimumReferees() const
{
  for (unsigned g = 0; g < in.Games(); ++g) {
    const auto& game = in.gamesData[g];
    unsigned assigned_referees = gameAssignments[g].size();

    // Cerca la divisione con il codice giusto
    unsigned min_referees = 1; // valore di default
    for (const auto& div : in.divisionsData) {
      if (div.code == game.division_code) {
        min_referees = div.min_referees;
        break;
      }
    }

    if (assigned_referees < min_referees) {
      return false;
    }
  }
  return true;
}

bool RA_Output::FeasibleDistance() const 
{};


void RA_Output::AssignRefereetoGame(unsigned game_id, const string& referee_code)
{
  gameAssignments[game_id].push_back(referee_code);
}

const vector<string>& RA_Output::AssignedReferees(unsigned game_id) const
{
  return gameAssignments[game_id];
}

unsigned RA_Output::ComputeCost() const
{
  return ComputeExperienceNeeded() + ComputeGameDistribution() +
         ComputeMinDistanceCost() +
         ComputeAssignmentFrequency() + ComputeRefereeCompatibility() +
         ComputeTeamCompatibilityCost();
}

void RA_Output::Reset()
{
  for(auto& g : gameAssignments)
  {
    g.clear();
  }
}

void RA_Output::Dump(ostream& os) const {
  for (unsigned g = 0; g < in.Games(); ++g) {
    const auto& game = in.gamesData[g];
    os << game.homeTeam_code << " " << game.guestTeam_code << " " << gameAssignments[g].size();
    for (const auto& r : gameAssignments[g])
      os << " " << r;
    os << "\n";
  }
}

ostream& operator<<(ostream& os, const RA_Output& out)
{
  out.Dump(os);
  return os;
}

istream& operator>>(istream& is, RA_Output& out) {
  out.Reset();
  string home, guest, referee;
  unsigned num_refs;
  while (is >> home >> guest >> num_refs) {
    unsigned game_id = -1;
    for (unsigned g = 0; g < out.in.Games(); ++g) {
      if (out.in.gamesData[g].homeTeam_code == home &&
          out.in.gamesData[g].guestTeam_code == guest) {
        game_id = g;
        break;
      }
    }
    if (game_id == -1) {
      cerr << "Errore: partita " << home << "-" << guest << " non trovata\n";
      exit(1);
    }

    for (unsigned i = 0; i < num_refs; ++i) {
      is >> referee;
      out.AssignRefereetoGame(game_id, referee);
    }
  }
  return is;
}



bool operator==(const RA_Output& out1, const RA_Output& out2)
{
  if (out1.in.Games() != out2.in.Games()) return false;
  for (unsigned g = 0; g < out1.in.Games(); ++g) {
    if (out1.gameAssignments[g] != out2.gameAssignments[g]) return false;
  }
  return true;
}

