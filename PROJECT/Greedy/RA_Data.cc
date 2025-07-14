// File RA_Data.cc
#include "RA_Data.hh"
#include <fstream>
#include <cmath>
#include <iostream>
#include <utility>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>


using namespace std;

/////////////////////////////////// RA_Input Implementation //////////////////////////////////////

// Parsing date and time strings
tm parseDate(const string& dateStr) {
    tm date = {};
    istringstream ss(dateStr);
    ss >> get_time(&date, "%d/%m/%Y");  // Usa il formato corretto
    if (ss.fail()) {
        cerr << "Errore nel parsing della data!\n";
    }
    return date;
}

tm parseTime(const string& timeStr) {
    tm time = {};
    istringstream ss(timeStr);
    ss >> get_time(&time, "%H:%M");  // Ora e minuti
    if (ss.fail()) {
        cerr << "Errore nel parsing dell'orario!\n";
    }
    return time;
}

string Trim(const string& str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  if (string::npos == first) return "";
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

RA_Input::RA_Input(string file_name) {
    ifstream is(file_name);
    if (!is) {
        cerr << "Cannot open input file " << file_name << endl;
        exit(1);
    }

    // Read starting parameters
    string dummy;
    char ch;
    is >> dummy >> ch >> divisions >> ch;
    is >> dummy >> ch >> referees >> ch;
    is >> dummy >> ch >> arenas >> ch;
    is >> dummy >> ch >> teams >> ch;
    is >> dummy >> ch >> games >> ch;

    divisionsData.resize(divisions);
    refereesData.resize(referees);
    arenasData.resize(arenas);
    teamsData.resize(teams);
    gamesData.resize(games);
    
    string line;
    stringstream ss(line);

    // === Leggi DIVISIONS ===
    while (getline(is, line)) {
        if (line.find("DIVISIONS") != string::npos) break;
    }
    for (unsigned d = 0; d < divisions; ++d) {
        if (!getline(is, line) || line.empty()) continue;
        
        stringstream ss(line);
        string code;
        getline(ss, code, ':');
        divisionsData[d].code = Trim(code);
        
        char comma;
        ss >> divisionsData[d].min_referees >> comma
            >> divisionsData[d].max_referees >> comma
            >> divisionsData[d].level >> comma
            >> divisionsData[d].teams;
    };

    // === Leggi REFEREES ===
    while (getline(is, line)) {
        if (!line.empty() && line[0] == 'R' && line.find('%') != string::npos) break;
    }
    for (unsigned r = 0; r < referees; ++r) {
        if (!getline(is, line) || line.empty()) continue;
        stringstream ss(line);
        string token;
        
        // Read code
        getline(ss, token, ','); // Read until first comma
        refereesData[r].code = token;
        
        // Read level
        getline(ss, token, ',');
        refereesData[r].level = stoi(token);
        
        // Read coordinates
        ss.ignore(); // Skip space after comma
        getline(ss, token, ')'); // Read until closing parenthesis
        token.erase(remove(token.begin(), token.end(), '('), token.end()); // Remove '('
        stringstream coord_ss(token);
        coord_ss >> refereesData[r].coordinates.first;
        coord_ss.ignore(); // Skip comma
        coord_ss >> refereesData[r].coordinates.second;
        ss.ignore(); // Skip comma after coordinates
        
        // Read experience
        getline(ss, token, ',');
        refereesData[r].experience = stoi(token);
        
        // Read incompatible referees
        ss.ignore(); // Skip space after comma
        getline(ss, token, ']'); // Read until closing bracket
        token.erase(remove(token.begin(), token.end(), '['), token.end()); // Remove '['
        if (!token.empty()) {
            stringstream ref_ss(token);
            string ref_code;
            while (getline(ref_ss, ref_code, ',')) {
                if (!ref_code.empty()) {
                    refereesData[r].incompatible_referees.push_back(ref_code);
                }
            }
        }
        ss.ignore(); // Skip comma after incompatible referees
        
        // Read incompatible teams
        ss.ignore(); // Skip space after comma
        getline(ss, token, ']'); // Read until closing bracket
        token.erase(remove(token.begin(), token.end(), '['), token.end()); // Remove '['
        if (!token.empty()) {
            stringstream team_ss(token);
            string team_code;
            while (getline(team_ss, team_code, ',')) {
                if (!team_code.empty()) {
                    refereesData[r].incompatible_teams.push_back(team_code);
                }
            }
        }
        ss.ignore(); // Skip comma after incompatible teams
        
        // Read unavailabilities
        ss.ignore(); // Skip space after comma
        getline(ss, token, ']'); // Read until closing bracket
        token.erase(remove(token.begin(), token.end(), '['), token.end()); // Remove '['
        if (!token.empty()) {
            stringstream unavail_ss(token);
            string date_time;
            while (getline(unavail_ss, date_time, ',')) {
                if (!date_time.empty()) {
                    size_t space_pos = date_time.find(' ');
                    if (space_pos != string::npos) {
                        string date = date_time.substr(0, space_pos);
                        string time = date_time.substr(space_pos + 1);
                        refereesData[r].unavailabilities.emplace_back(date, time);
                    }
                }
            }
        }
    }

    // === Leggi ARENAS ===
    while (getline(is, line)) {
        if (line.find("ARENAS") != string::npos) break;
    }
    for (unsigned a = 0; a < arenas; ++a) {
        if (!getline(is, line) || line.empty()) continue;
        
        stringstream ss(line);
        ss >> arenasData[a].code;
        
        ss.ignore(); // Ignora lo spazio
        string coord_str;
        getline(ss, coord_str, ')');
        coord_str.erase(remove(coord_str.begin(), coord_str.end(), '('), coord_str.end());
        stringstream coord_ss(coord_str);
        coord_ss >> arenasData[a].coordinates.first;
        coord_ss.ignore(); // Ignora la virgola
        coord_ss >> arenasData[a].coordinates.second;
    }

    // === Leggi TEAMS ===
    while (getline(is, line)) {
        if (line.find("TEAMS") != string::npos) break;
    }
    for (unsigned t = 0; t < teams; ++t) {
        if (!getline(is, line) || line.empty()) continue;
        
        stringstream ss(line);
        ss >> teamsData[t].code >> teamsData[t].division_code;
    }

    // === Leggi GAMES ===
    while (getline(is, line)) {
        if (line.find("GAMES") != string::npos) break;
    }
    for (unsigned g = 0; g < games; ++g) {
        while(getline(is, line)) {
            if (!line.empty()) break; // Skip empty lines
        }
        
        stringstream ss(line);
        ss >> gamesData[g].homeTeam_code
            >> gamesData[g].guestTeam_code
            >> gamesData[g].division_code;
        
        string date_str, time_str;
        ss >> date_str >> time_str;
        gamesData[g].date = parseDate(date_str);
        gamesData[g].time = parseTime(time_str);
        
        ss >> gamesData[g].arena_code >> gamesData[g].experience_required;
    }

}

// Function that computes the distance between two arenas
float RA_Input::ComputeDistancesBetweenArenas(Arena a1, Arena a2) const{
  // si può usare memoization per evitare di calcolare più volte la stessa distanza (calcolando all'occorrenza i valori richiesti)
  float dx = a1.coordinates.first - a2.coordinates.first;
  float dy = a1.coordinates.second - a2.coordinates.second;
  return sqrt(dx * dx + dy * dy);
}

// Function that computes the distance between an arena and a referee
float RA_Input::ComputeDistancesBetweenArenasAndReferees(Arena a, Referee r) const{
  float dx = a.coordinates.first - r.coordinates.first;
  float dy = a.coordinates.second - r.coordinates.second;
  return sqrt(dx * dx + dy * dy);
}

// Function that computes the travel time between two arenas
float RA_Input::ComputeTravelTimeBetweenArenas(Arena a1, Arena a2) const{
  const float speed = 60.0;
  return (DistanceBetweenArenas(a1, a2) / speed);
}

// Function that computes the travel time between an arena and a referee
float RA_Input::ComputeTravelTimeBetweenArenasAndReferee(Arena a, Referee r) const{
  const float speed = 60.0;
  return (DistanceBetweenArenasAndReferee(a, r) / speed);
}

ostream& operator<<(ostream& os, const RA_Input& in){
 
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
  char date_buf[11];
  char time_buf[6];
  for (const auto& g : in.gamesData) {
    // Format date and time_buf[6];
    strftime(date_buf, sizeof(date_buf), "%d/%m/%Y", &g.date);
    strftime(time_buf, sizeof(time_buf), "%H:%M", &g.time);
    os << g.homeTeam_code << " " << g.guestTeam_code << " " << g.division_code << " " << date_buf << " " << time_buf << " " << g.arena_code << " " << g.experience_required << "\n";}

  return os;
}

/////////////////////////////////// RA_Output Implementation //////////////////////////////////////

// Builder
RA_Output::RA_Output(const RA_Input& my_in): in(my_in){
  gameAssignments.resize(in.Games());
} 

RA_Output& RA_Output::operator=(const RA_Output& out){
  gameAssignments = out.gameAssignments;
  return *this;
}

// The number of mandatory referees must always be assigned to each game.
bool RA_Output::MinimumReferees() const{
  for (unsigned g = 0; g < in.Games(); ++g) {
    const auto& game = in.gamesData[g];
    // DA VEDERE SE IL NUMERO VARIA IN BASE ALLA DIVISIONE
    unsigned assigned_referees = gameAssignments[g].size();

    // Find the minimum number of referees required for the division of the game
    unsigned min_referees = 1; // default value
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

bool RA_Output::FeasibleDistance() const {
  // For each ref, check the game assignment feasibility
  for (const auto& referee : in.refereesData) {
    
    // Find all games assigned to a ref
    vector<pair<tm, pair<tm, unsigned>>> assignedGames; // (date, (time, game_id))
    for (unsigned g = 0; g < in.Games(); ++g) {
      for (const auto& ref_code : gameAssignments[g]) {
        // DA CONTROLLARE COME FARE IL CONFRONTO CON I CODICI (SE HO VETTORE == STRINGA)
        if (ref_code == referee.code) {
          assignedGames.push_back({in.gamesData[g].date, {in.gamesData[g].time, g}});
        }
      }
    }

    //Sort assigned games by date and time
    sort(assignedGames.begin(), assignedGames.end(), [](const auto& a, const auto& b) {
      time_t ta = mktime(const_cast<tm*>(&a.first));
      time_t tb = mktime(const_cast<tm*>(&b.first));
      if (ta != tb) return ta < tb;
      // If date is the same, compare time
      time_t tma = mktime(const_cast<tm*>(&a.second.first));
      time_t tmb = mktime(const_cast<tm*>(&b.second.first));
      return tma < tmb;
    });

    // Check distance between consecutive games
    for (size_t i = 1; i < assignedGames.size(); ++i) {
      unsigned prev_game = assignedGames[i - 1].second.second;
      unsigned curr_game = assignedGames[i].second.second;
      const auto& prev = in.gamesData[prev_game];
      const auto& curr = in.gamesData[curr_game];

      // Compute end time of the previous game
      tm end_prev = prev.time;
      end_prev.tm_hour += 2;
      mktime(&end_prev); // normalizza

      // Compute strart time of next_game
      tm start_curr = curr.time;

      // Compute difference in minutes between the end of the previous game and the start of the current game
      tm prev_date = prev.date;
      tm curr_date = curr.date;
      prev_date.tm_hour = end_prev.tm_hour;
      prev_date.tm_min = end_prev.tm_min;
      time_t t_end_prev = mktime(&prev_date);
      curr_date.tm_hour = start_curr.tm_hour;
      curr_date.tm_min = start_curr.tm_min;
      time_t t_start_curr = mktime(&curr_date);

      double minutes_between = difftime(t_start_curr, t_end_prev) / 60.0;

      // Compute travel time
      const auto& prev_arena = *find_if(in.arenasData.begin(), in.arenasData.end(),
                                        [&](const auto& a) { return a.code == prev.arena_code; });
      const auto& curr_arena = *find_if(in.arenasData.begin(), in.arenasData.end(),
                                        [&](const auto& a) { return a.code == curr.arena_code; });
      float travel_time = in.TravelTimeBetweenArenas(prev_arena, curr_arena) * 60.0; // in minuti

      if (minutes_between < travel_time) {
        return false;
      }
    }
  }
  return true;
}

bool RA_Output::RefereeAvailability() const {
  // For each ref, check if they are available for the assigned games
  for (const auto& referee : in.refereesData) {
    // Find all games assigned to a ref
    vector<pair<tm, pair<tm, unsigned>>> assignedGames; // (date, (time, game_id))
    for (unsigned g = 0; g < in.Games(); ++g) {
      for (const auto& ref_code : gameAssignments[g]) {
        if (referee.code == ref_code) {
          assignedGames.push_back({in.gamesData[g].date, {in.gamesData[g].time, g}});
        }
      }
    }

    for (const auto& ref_code : gameAssignments[0]) {
      cout << "ref_code = [" << ref_code << "]" << endl;
    }

    // Check if the referee is available for each assigned game
    for (const auto& game : assignedGames) {
      const auto& game_date = game.first;
      const auto& game_time = game.second.first;
      unsigned game_id = game.second.second;

      // Check if the referee is unavailable on this date and time
      bool available = true;
      for (const auto& unavailability : referee.unavailabilities) {
        tm unavailable_date = parseDate(unavailability.first);
        tm unavailable_time = parseTime(unavailability.second);

        // Compare dates and times
        if (unavailable_date.tm_year == game_date.tm_year &&
            unavailable_date.tm_mon == game_date.tm_mon &&
            unavailable_date.tm_mday == game_date.tm_mday &&
            unavailable_time.tm_hour == game_time.tm_hour &&
            unavailable_time.tm_min == game_time.tm_min) {
          available = false;
          break;
        }
      }

      if (!available) {
        return false; // Referee is not available for this game
      }
    }
  }
  return true;
}

bool RA_Output::Feasibility() const{
  return MinimumReferees() && FeasibleDistance() && RefereeAvailability();
}

void RA_Output::AssignRefereeToGame(unsigned game_id, const string& referee_code) {
  // Check if game_id is within bounds
  if (game_id >= gameAssignments.size()) {
    cerr << "Error: game_id out of bounds" << endl;
    return;
  }

  // Add referee code to the vector of referees for this game
  gameAssignments[game_id].push_back(referee_code);
}

const vector<string>& RA_Output::AssignedReferees(unsigned game_id) const{
  return gameAssignments[game_id];
}

unsigned RA_Output::ComputeCost() const{
  return 1;
  // ComputeExperienceNeeded() + ComputeGameDistribution() +
  //        ComputeMinDistanceCost() +
  //        ComputeAssignmentFrequency() + ComputeRefereeCompatibility() +
  //        ComputeTeamCompatibilityCost();
}

void RA_Output::Reset(){
  for(auto& g : gameAssignments){
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

ostream& operator<<(ostream& os, const RA_Output& out){
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
      out.AssignRefereeToGame(game_id, referee);
    }
  }
  return is;
}

bool operator==(const RA_Output& out1, const RA_Output& out2){
  if (out1.in.Games() != out2.in.Games()) return false;
  for (unsigned g = 0; g < out1.in.Games(); ++g) {
    if (out1.gameAssignments[g] != out2.gameAssignments[g]) return false;
  }
  return true;
}