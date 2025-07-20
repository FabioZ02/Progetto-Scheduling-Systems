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
#include <numeric>

using namespace std;

#define SPEED 60

/////////////////////////////////// RA_Input Implementation //////////////////////////////////////

// Parsing date and time strings
tm parseDate(const string& dateStr) {
    tm date = {};
    istringstream ss(dateStr);
    ss >> get_time(&date, "%d/%m/%Y");  // Usa il formato corretto
    if (ss.fail()) {
        std::cerr << "Errore nel parsing della data!\n";
    }
    return date;
}
tm parseTime(const string& timeStr) {
    tm time = {};
    istringstream ss(timeStr);
    ss >> get_time(&time, "%H:%M");  // Ora e minuti
    if (ss.fail()) {
        std::cerr << "Errore nel parsing dell'orario!\n";
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
        std::cerr << "Cannot open input file " << file_name << endl;
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

    // Reading DIVISIONS
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

    // Reading REFEREES
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
                date_time = Trim(date_time); // Clean up whitespace

                if (!date_time.empty()) {
                    size_t space_pos = date_time.find(' ');
                    size_t dash_pos = date_time.find('-');

                    if (space_pos != string::npos && dash_pos != string::npos) {
                        string date_str = date_time.substr(0, space_pos);
                        string time_start_str = date_time.substr(space_pos + 1, dash_pos - space_pos - 1);
                        string time_end_str = date_time.substr(dash_pos + 1);

                        tm date = parseDate(date_str);
                        tm time_start = parseTime(time_start_str);
                        tm time_end = parseTime(time_end_str);

                        refereesData[r].unavailabilities.emplace_back(make_pair(date, make_pair(time_start, time_end)));
                    }
                }
            }
        }
    }

    // Reading ARENAS
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

    // Reading TEAMS
    while (getline(is, line)) {
        if (line.find("TEAMS") != string::npos) break;
    }
    for (unsigned t = 0; t < teams; ++t) {
        if (!getline(is, line) || line.empty()) continue;
        
        stringstream ss(line);
        ss >> teamsData[t].code >> teamsData[t].division_code;
    }

    // Reading GAMES
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

// Calculate the distance between two coordinates
float RA_Input::EuclideanDistance(const pair<float, float>& c1, const pair<float, float>& c2) const{
    float dx = c1.first - c2.first;
    float dy = c1.second - c2.second;
    return sqrt(dx * dx + dy * dy);
}
// Si possono unire le due funzioni implementando un valore per scegliere il secondo parametro
// Function that computes the distance between two arenas
float RA_Input::ComputeDistancesBetweenArenas(Arena a1, Arena a2) const{
  return RA_Input::EuclideanDistance(a1.coordinates, a2.coordinates);
}
// Function that computes the distance between an arena and a referee
float RA_Input::ComputeDistancesBetweenArenasAndReferees(Arena a, Referee r) const{
    return RA_Input::EuclideanDistance(a.coordinates, r.coordinates);
}
// Function that computes the travel time between two arenas
float RA_Input::ComputeTravelTimeBetweenArenas(Arena a1, Arena a2) const{
  const float speed = 60.0;
  return (ComputeDistancesBetweenArenas(a1, a2) / speed);
}
// Function that computes the travel time between an arena and a referee
float RA_Input::ComputeTravelTimeBetweenArenasAndReferee(Arena a, Referee r) const{
  const float speed = 60.0;
  return (ComputeDistancesBetweenArenasAndReferees(a, r) / speed);
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
    char date_buf[11];
    char timeStart_buf[6];
    char timeEnd_buf[6];
    for (size_t i = 0; i < r.unavailabilities.size(); ++i) {
      strftime(date_buf, sizeof(date_buf), "%d/%m/%Y", &r.unavailabilities[i].first);
      strftime(timeStart_buf, sizeof(timeStart_buf), "%H:%M", &r.unavailabilities[i].second.first);
      strftime(timeEnd_buf, sizeof(timeEnd_buf), "%H:%M", &r.unavailabilities[i].second.second);
      os << date_buf << " " << timeStart_buf << "-" << timeEnd_buf;
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

// Function to find the division by its code
const RA_Input::Division& RA_Input::GetDivisionByCode(const string& code) const {
    auto it = find_if(divisionsData.begin(), divisionsData.end(),
                      [&](const Division& d) { return d.code == code; });

    if (it == divisionsData.end()) {
        std::cerr << "Errore: Divisione con codice '" << code << "' non trovata.\n";
        exit(1);
    }

    return *it;
}
// Function to find the referee by its code
const RA_Input::Referee& RA_Input::GetRefereeByCode(const string& code) const {
    auto it = find_if(refereesData.begin(), refereesData.end(),
                      [&](const Referee& r) { return r.code == code; });

    if (it == refereesData.end()) {
        std::cerr << "Errore: Arbitro con codice '" << code << "' non trovato.\n";
        exit(1);
    }

    return *it;
}
// Function to find the arena by its code
const RA_Input::Arena& RA_Input::GetArenaByCode(const string& code) const {
    auto arena = find_if(arenasData.begin(), arenasData.end(),
                      [&](const Arena& a) { return a.code == code; });

    if (arena == arenasData.end()) {
        std::cerr << "Errore: Arena con codice '" << code << "' non trovata.\n";
        exit(1);
    }

    return *arena;
}
// Function to find the Team by its code
const RA_Input::Team& RA_Input::GetTeamByCode(const string& code) const {
    auto it = find_if(teamsData.begin(), teamsData.end(),
                      [&](const Team& t) { return t.code == code; });

    if (it == teamsData.end()) {
        std::cerr << "Errore: Team con codice '" << code << "' non trovato.\n";
        exit(1);
    }

    return *it;
}

// Function to check if two teams are incompatible
bool RA_Input::AreRefereesIncompatible(const string& code1, const string& code2) const {
    const auto& ref1 = GetRefereeByCode(code1);
    const auto& ref2 = GetRefereeByCode(code2);

    return (find(ref1.incompatible_referees.begin(), ref1.incompatible_referees.end(), code2) != ref1.incompatible_referees.end() ||
            find(ref2.incompatible_referees.begin(), ref2.incompatible_referees.end(), code1) != ref2.incompatible_referees.end());
}
// Function to check if a referee is incompatible with a team
bool RA_Input::IsRefereeIncompatibleWithTeam(const string& referee_code, const string& team_code) const {
    const auto& referee = GetRefereeByCode(referee_code);
    return find(referee.incompatible_teams.begin(), referee.incompatible_teams.end(), team_code) != referee.incompatible_teams.end();
}


/////////////////////////////////// RA_Output Implementation //////////////////////////////////////

// Builder
RA_Output::RA_Output(const RA_Input& my_in): in(my_in){
    gameAssignments.resize(in.Games());
    teamAssignmentsPerReferee.resize(in.Referees(), vector<unsigned>(in.Teams(), 0)); // Initialize with 0 assignments for each referee per team
}
RA_Output& RA_Output::operator=(const RA_Output& out){
  gameAssignments = out.gameAssignments;
  teamAssignmentsPerReferee = out.teamAssignmentsPerReferee;
  return *this;
}


// Function to assign a referee to a game
void RA_Output::AssignRefereeToGame(unsigned game_id, const string& referee_code) {
  // Check if game_id is within bounds
  if (game_id >= gameAssignments.size()) {
    std::cerr << "Error: game_id " << game_id << " out of bounds" << endl;
    return;
  }

  // Check if the referee_code is already assigned to this game
  if (find(gameAssignments[game_id].begin(), gameAssignments[game_id].end(), referee_code) != gameAssignments[game_id].end()) {
    std::cerr << "Error: Referee '" << referee_code << "' is already assigned to game ID " << game_id << ".\n";
    return;
  }

  // Add referee code to the vector of referees for this game
  // cout << "Assigning referee '" << referee_code << "' to game ID " << game_id << ".\n";
  gameAssignments[game_id].push_back(referee_code);

  // Update the teamAssignmentsPerReferee vector
  unsigned referee_index = find_if(in.refereesData.begin(), in.refereesData.end(),
                                    [&](const RA_Input::Referee& r) { return r.code == referee_code; }) - in.refereesData.begin();
  unsigned team_index_H = find_if(in.teamsData.begin(), in.teamsData.end(),
                                    [&](const RA_Input::Team& t) { return t.code == in.gamesData[game_id].homeTeam_code; }) - in.teamsData.begin();
  unsigned team_index_G = find_if(in.teamsData.begin(), in.teamsData.end(),
                                    [&](const RA_Input::Team& t) { return t.code == in.gamesData[game_id].guestTeam_code; }) - in.teamsData.begin();
  
  teamAssignmentsPerReferee[referee_index][team_index_H]++;
  teamAssignmentsPerReferee[referee_index][team_index_G]++;
}
// Function that returns all the games assigned to a referee
/* Se le partite sono tante e il codice viene eseguito spesso, puoi pensare a una mappa inversa unordered_map<string, 
vector<unsigned>>, dove memorizzi per ogni arbitro la lista di partite assegnate. Ma per ora, find va benissimo.*/
const vector<pair<tm, pair<tm, unsigned>>> RA_Output::GamesAssignedToReferee(const string& referee_code) const {
  vector<pair<tm, pair<tm, unsigned>>> assignedGames; // (date, (time, game_id))
  
  for (unsigned g = 0; g < in.Games(); ++g) {
      if (find(gameAssignments[g].begin(), gameAssignments[g].end(), referee_code) != gameAssignments[g].end()) {
          assignedGames.push_back(make_pair(in.gamesData[g].date, make_pair(in.gamesData[g].time, g)));
      }
  }
  
  return assignedGames;
}
// Function to retrieve the referees assigned to a specific game DA CANCELLARE O USARE
const vector<string>& RA_Output::AssignedRefereesToGame(unsigned game_id) const{
  cout << "Retrieving assigned referees for game ID " << game_id << ".\n";
  return gameAssignments[game_id];
}

// Function to check if a referee is available for a specific game
bool RA_Output::RefereeAvailableForGame(const RA_Input::Referee& referee, const RA_Input::Game& game) const {
    for (const auto& unav : referee.unavailabilities) {
        if (unav.first.tm_year == game.date.tm_year &&
            unav.first.tm_mon == game.date.tm_mon &&
            unav.first.tm_mday == game.date.tm_mday) {

            int game_start = game.time.tm_hour * 60 + game.time.tm_min;
            int game_end = game_start + 120;

            int unav_start = unav.second.first.tm_hour * 60 + unav.second.first.tm_min;
            int unav_end   = unav.second.second.tm_hour * 60 + unav.second.second.tm_min;

            if (!(game_end <= unav_start || game_start >= unav_end)) {
                return false; // overlap
            }
        }
    }
    return true;
}
// Function to check if a referee can attend a game considering travel time
bool RA_Output::CanAttendGame(const RA_Input::Referee& referee, const RA_Input::Game& new_game) const {
    // Retrieve all games assigned to the referee
    auto assigned = GamesAssignedToReferee(referee.code);

    for (const auto& ag : assigned) {
        const auto& assigned_game = in.gamesData[ag.second.second];

        // Check if the assigned game is on the same day
        if (assigned_game.date.tm_year == new_game.date.tm_year &&
            assigned_game.date.tm_mon  == new_game.date.tm_mon &&
            assigned_game.date.tm_mday == new_game.date.tm_mday) {

            // Calculate end time of the assigned game
            tm end_time = assigned_game.time;
            end_time.tm_hour += 2;
            mktime(&end_time); // normalize overflow of time

            // Calculate available time between the end of the assigned game and the start of the new game
            tm d1 = assigned_game.date; d1.tm_hour = end_time.tm_hour; d1.tm_min = end_time.tm_min;
            tm d2 = new_game.date;      d2.tm_hour = new_game.time.tm_hour; d2.tm_min = new_game.time.tm_min;

            time_t t1 = mktime(&d1);
            time_t t2 = mktime(&d2);
            double available_minutes = difftime(t2, t1) / 60.0;

            const auto& a1 = in.GetArenaByCode(assigned_game.arena_code);
            const auto& a2 = in.GetArenaByCode(new_game.arena_code);

            double travel_minutes = in.TravelTimeBetweenArenas(a1, a2) * 60.0;

            // Check if the available time is sufficient for travel
            if (available_minutes < travel_minutes)
                return false;
        }
    }
    return true;
}
// Function to check if a referee is compatible with the selected referees
bool RA_Output::IsCompatibleWith(const RA_Input::Referee& ref, const vector<string>& selected_referees) const {
    for (const auto& other_code : selected_referees) {
        if(in.AreRefereesIncompatible(ref.code, other_code)){
            std::cerr << "Incompatibility found: Referee " << ref.code << " is incompatible with referee " << other_code << ".\n";
            return false;
        }
    }
    return true;
}





// The number of mandatory referees must always be assigned to each game.
bool RA_Output::NumberOfReferees() const{
  for (unsigned g = 0; g < in.Games(); ++g) {
    const auto& game = in.gamesData[g];

    unsigned assigned_referees = gameAssignments[g].size();

    // Find the minimum and maximum number of referees required for the division of the game
    unsigned min_referees = 1; // default value
    unsigned max_referees = 1; // default value
    for (const auto& div : in.divisionsData) {
      if (div.code == game.division_code) {
        min_referees = div.min_referees;
        max_referees = div.max_referees;
        break;
      }
    }

    if (assigned_referees < min_referees) {
      std::cerr << "Referees Number: Game " << g << " requires at least " << min_referees << " referees, but only " << assigned_referees << " are assigned.\n";
      return false;
    }
    if (assigned_referees > max_referees) {
      std::cerr << "Referees Number: Game " << g << " requires at most " << max_referees << " referees, but " << assigned_referees << " are assigned.\n";
      return false;
    }
  }
  return true;
}
// The distance between the arenas of two consecutive games assigned to the same referee must be feasible.
bool RA_Output::FeasibleDistance() const {
  // For each ref, check the game assignment feasibility
  for (const auto& referee : in.refereesData) {
    
    // Find all games assigned to a ref (date, (time, game_id)))
    vector<pair<tm, pair<tm, unsigned>>> assignedGames = GamesAssignedToReferee(referee.code); // (date, (time, game_id))

    //Sort assigned games by date and time
    sort(assignedGames.begin(), assignedGames.end(), [](const auto& a, const auto& b) {
      if (a.first.tm_year != b.first.tm_year) return a.first.tm_year < b.first.tm_year;
      if (a.first.tm_mon != b.first.tm_mon) return a.first.tm_mon < b.first.tm_mon;
      if (a.first.tm_mday != b.first.tm_mday) return a.first.tm_mday < b.first.tm_mday;
      if (a.second.first.tm_hour != b.second.first.tm_hour) return a.second.first.tm_hour < b.second.first.tm_hour;
      return a.second.first.tm_min < b.second.first.tm_min;
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
      mktime(&end_prev); // normalize

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
      const auto& prev_arena = *find_if(in.arenasData.begin(), in.arenasData.end(), [&](const auto& a) { return a.code == prev.arena_code; });
      const auto& curr_arena = *find_if(in.arenasData.begin(), in.arenasData.end(), [&](const auto& a) { return a.code == curr.arena_code; });
      float travel_time = in.TravelTimeBetweenArenas(prev_arena, curr_arena) * 60.0; // in minuti

      if (minutes_between < travel_time) {
        // std::cerr << "Feasible Distance: Referee " << referee.code << " cannot travel from game " << prev_game
        //      << " to game " << curr_game << " in time. Required travel time: "
        //      << travel_time << " minutes, available time: " << minutes_between << " minutes.\n";
        return false;
      }
    }
  }
  return true;
}
// The referee must be available for all the games assigned to them.
bool RA_Output::RefereeAvailability() const {
  // For each ref, check if they are available for the assigned games
  for (const auto& referee : in.refereesData) {
    // Find all games assigned to a ref
    vector<pair<tm, pair<tm, unsigned>>> assignedGames = GamesAssignedToReferee(referee.code); // (date, (time, game_id))

    // Check if the referee is available for each assigned game
    for (const auto& game : assignedGames) {
      auto& game_date = game.first;
      auto& game_time = game.second.first;
      unsigned game_id = game.second.second;

      // Check if the referee is unavailable on this date and time
      bool available = true; // Assume available unless we find a match
      for (const auto& unavailability : referee.unavailabilities) {
        // Compare dates and times
        tm unav_date = unavailability.first;
        tm unav_start = unavailability.second.first;
        tm unav_end = unavailability.second.second;
        
        // Only if the unavailability date matches the game date
        if (unav_date.tm_year == game_date.tm_year && unav_date.tm_mon == game_date.tm_mon && unav_date.tm_mday == game_date.tm_mday) {
          
          // Convert times to minutes since the start of the day for easier comparison
          int game_start_min = game_time.tm_hour * 60 + game_time.tm_min;
          int game_end_min = game_start_min + 120; // match lasts 2 hours (120 minutes)

          int unav_start_min = unav_start.tm_hour * 60 + unav_start.tm_min;
          int unav_end_min = unav_end.tm_hour * 60 + unav_end.tm_min;

          // Overlap condition: if the game time overlaps with the unavailability time
          if (! (game_end_min <= unav_start_min || game_start_min >= unav_end_min)) {
              return false; // The referee is not available
          }
        }
      }

      if (!available) {
        // std::cerr << "Ref Availability: Referee " << referee.code << " is not available for game ID " << game_id
        //      << " on " << put_time(&game_date, "%d/%m/%Y") << " at "
        //      << put_time(&game_time, "%H:%M") << ".\n";
        return false;
      }
    }
  }
  return true;
}
// Check if the solution is feasible
bool RA_Output::Feasibility() const{
  return NumberOfReferees() && FeasibleDistance() && RefereeAvailability();
}


// Check if the referee level is at leat the minimum required for the game in that division
unsigned RA_Output::RefereeLevel() const {
    unsigned violations = 0;
    for (unsigned g = 0; g < in.Games(); ++g) {
        const auto& game = in.gamesData[g];
        unsigned required_level = 0;

        // Find the division of the game
        for (const auto& div : in.divisionsData) {
            if (div.code == game.division_code) {
                required_level = div.level;
                break;
            }
        }

        // Check if all assigned referees meet the required level
        for (const auto& referee_code : gameAssignments[g]) {
            for (const auto& referee : in.refereesData) {
                if (referee.code == referee_code) {
                    if (referee.level < required_level) {
                        // std::cerr << "Ref Level violation: Referee " << referee.code << " assigned to game " << g 
                        //      << " does not meet the required level of " << required_level 
                        //      << " - Referee level: " << referee.level << ".\n";
                        violations++;
                    }
                }
            }
        }
    }
    return violations;
}
// Sum of the experience of all referees assigned to the games
unsigned RA_Output::ExperienceNeeded() const {
    unsigned violations = 0;
    for (unsigned g = 0; g < in.Games(); ++g) {
        const auto& game = in.gamesData[g];
        unsigned total_experience = 0;

        // Check if the sum of the experience of all referees assigned to the game is at 
        // least the required experience
        for (const auto& referee_code : gameAssignments[g]) {
            for (const auto& referee : in.refereesData) {
                if (referee.code == referee_code) {
                    total_experience += referee.experience;
                }
            }
        }

        // If the total experience is less than the required experience, count a violation
        if( total_experience < game.experience_required) {
            // std::cerr << "Experience violation: Game " << g << " requires at least " << game.experience_required 
            //      << " experience, but only " << total_experience << " is assigned.\n";
            violations++;
        }
    }
    return violations;
}
// Check the distribution of games among referees
unsigned RA_Output::GameDistribution() const {
    unsigned violations = 0;

    // Count the number of games assigned to each referee
    vector<unsigned> games_per_referee(in.Referees(), 0);
    for (unsigned r = 0; r < in.Referees(); ++r) {
        for (unsigned t = 0; t < in.Teams(); ++t) {
            games_per_referee[r] += teamAssignmentsPerReferee[r][t];
        }
    }

    // Calculate the mean and standard deviation of games assigned to referees
    float mean_value = accumulate(games_per_referee.begin(), games_per_referee.end(), 0) / in.Referees();
    float std_dev = 0.0;
    for (unsigned r = 0; r < in.Referees(); ++r) {
        std_dev += pow(games_per_referee[r] - mean_value, 2);
    }
    std_dev = sqrt(std_dev / in.Referees());

    // Count the number of referees whose assigned games deviate from the mean by more than the standard deviation
    for (unsigned r = 0; r < in.Referees(); ++r) {
        if ( abs(games_per_referee[r] - mean_value) > ceil(std_dev) + 1) { // ceiling to allow for a small margin of error
            // std::cerr << "Game Distribution violation: Referee " << in.refereesData[r].code << " has " << games_per_referee[r] 
            //       << " games assigned, which deviates from the mean (" << mean_value 
            //       << ") by more than the standard deviation (" << ceil(std_dev) << ")\n";
            violations++;
        }
    }

    return violations;
}
// Check the total distance traveled by all referees
float RA_Output::TotalDistance() const {
    float total_distance = 0.0;

    // For each referee, calculate the total distance traveled
    for (unsigned r = 0; r < in.Referees(); ++r) {
        const auto& referee = in.refereesData[r];
        float distance_per_referee = 0.0;

        // Find all games assigned to this referee
        vector<pair<tm, pair<tm, unsigned>>> assignedGames = GamesAssignedToReferee(referee.code); // (date, (time, game_id))

        // Sort assigned games by date and time
        sort(assignedGames.begin(), assignedGames.end(), [](const auto& a, const auto& b) {
            if (a.first.tm_year != b.first.tm_year) return a.first.tm_year < b.first.tm_year;
            if (a.first.tm_mon != b.first.tm_mon) return a.first.tm_mon < b.first.tm_mon;
            if (a.first.tm_mday != b.first.tm_mday) return a.first.tm_mday < b.first.tm_mday;
            if (a.second.first.tm_hour != b.second.first.tm_hour) return a.second.first.tm_hour < b.second.first.tm_hour;
            return a.second.first.tm_min < b.second.first.tm_min;
        });

        // Group games by day
        /* SI POTREBBE COSTRUIRE UNA MAPPA ANZICHE' DUE VETTORI PARALLELI*/
        vector<vector<unsigned>> daily_game_ids; // Vector to hold game IDs for each day
        vector<tm> daily_dates; // Vector to hold the dates of the games

        for (const auto& ag : assignedGames) {
            // Check if the date is already in the daily_dates vector
            if (daily_dates.empty() || !(ag.first.tm_year == daily_dates.back().tm_year &&
                                         ag.first.tm_mon  == daily_dates.back().tm_mon &&
                                         ag.first.tm_mday == daily_dates.back().tm_mday)) {
                daily_dates.push_back(ag.first);    // Add the new date to the daily_dates vector
                daily_game_ids.emplace_back();      // Create a new vector for the new day
            }
            // Add the game ID to the corresponding day's vector
            daily_game_ids.back().push_back(ag.second.second);
        }

        // Calculate the distance traveled for each game day
        for (const auto& games_in_day : daily_game_ids) {
            if (games_in_day.empty()) continue;

            // Get the arena for the first game of the day
            const auto& first_game = in.gamesData[games_in_day[0]];
            const auto& first_arena = *find_if(in.arenasData.begin(), in.arenasData.end(),
                                               [&](const RA_Input::Arena& a) { return a.code == first_game.arena_code; });

            // Calculate the distance from referee's home to first arena
            float day_distance = in.DistanceBetweenArenasAndReferee(first_arena, referee);

            // If there are more games on the same day, calculate the distance between arenas
            for (size_t i = 1; i < games_in_day.size(); ++i) {
                const auto& prev_game = in.gamesData[games_in_day[i - 1]];
                const auto& curr_game = in.gamesData[games_in_day[i]];

                const auto& prev_arena = *find_if(in.arenasData.begin(), in.arenasData.end(),
                                                  [&](const RA_Input::Arena& a) { return a.code == prev_game.arena_code; });
                const auto& curr_arena = *find_if(in.arenasData.begin(), in.arenasData.end(),
                                                  [&](const RA_Input::Arena& a) { return a.code == curr_game.arena_code; });

                // Calculate the distance between the previous arena and the current arena
                float distance_between_arenas = in.DistanceBetweenArenas(prev_arena, curr_arena);
                day_distance += distance_between_arenas; // add the distance between arenas
            }

            // Calculate the distance from the last arena back to home
            const auto& last_game = in.gamesData[games_in_day.back()];
            const auto& last_arena = *find_if(in.arenasData.begin(), in.arenasData.end(),
                                              [&](const RA_Input::Arena& a) { return a.code == last_game.arena_code; });

            day_distance += in.DistanceBetweenArenasAndReferee(last_arena, referee);

            distance_per_referee += day_distance;
        }

        total_distance += distance_per_referee;
    }

    return total_distance;
}
// Check if the number of referees is at least the minimum + 1
unsigned RA_Output::OptionalReferee() const {
    unsigned violations = 0;
    for (unsigned g = 0; g < in.Games(); ++g) {
        const auto& game = in.gamesData[g];
        unsigned assigned_referees = gameAssignments[g].size();

        // Find the minimum number of referees allowed for the division of the game
        unsigned min_referees = 1; // default value
        for (const auto& div : in.divisionsData) {
            if (div.code == game.division_code) {
                min_referees = div.min_referees;
                break;
            }
        }
        // Check if the number of assigned referees is less or equal to the minimum required
        // so there are no optional referees
        if(assigned_referees <= min_referees) {
            // std::cerr << "Optional Referee violation: Game " << g << " has " << assigned_referees 
            //      << " referees assigned, which is less than or equal to the minimum required of " 
            //      << min_referees << ".\n";
            violations++;
        }
    }
    return violations;
}
// Check if a referee is assigned to the same team more often than a certain threshold
unsigned RA_Output::AssignmentFrequency() const {
    unsigned violations = 0;

    // For each team, calculate the mean and standard deviation of assignments per referee
    // Calculate the mean value based on the games of first team
    float mean_team = 0, std_dev_team;

    for (unsigned r = 0; r < in.Referees(); ++r) {
        mean_team += teamAssignmentsPerReferee[r][0];
    }
    mean_team = mean_team * in.Teams() / in.Referees(); // Mean frequency of assignments each referee

    for (unsigned t = 0; t < in.Teams(); ++t) {
        std_dev_team = 0;
        
        // Calculate the standard deviation of assignments for each team
        for (unsigned r = 0; r < in.Referees(); ++r) {
            std_dev_team += pow(teamAssignmentsPerReferee[r][t] - mean_team, 2);
        }
        std_dev_team = sqrt(std_dev_team / in.Referees()); // Standard deviation of assignments each referee
        
        for (unsigned r = 0; r < in.Referees(); ++r) {
            if ( abs(teamAssignmentsPerReferee[r][t] - mean_team) > ceil(std_dev_team) + 1) { // ceiling to allow for a small margin of error
                violations++;
                // std::cerr << "Assign Frequency violation: Referee " << in.refereesData[r].code << " is assigned to team " 
                //      << in.teamsData[t].code << " more often than the allowed threshold (mean + std_dev). "
                //      << "Assigned: " << teamAssignmentsPerReferee[r][t] 
                //      << ", Mean: " << mean_team << ", Std Dev: " << ceil(std_dev_team) << ".\n";
            }
        }
    }

    return violations;
}
// Two incompatible referees cannot be assigned to the same game
unsigned RA_Output::RefereeIncompatibility() const {
    unsigned violations = 0;

    for (unsigned g = 0; g < in.Games(); ++g) {
        vector<string> assigned_referees = gameAssignments[g]; // Get the referees assigned to this game

        // Check each pair of assigned referees for incompatibility
        for (size_t i = 0; i < assigned_referees.size(); ++i) {
            for (size_t j = i + 1; j < assigned_referees.size(); ++j) {
                //DA CAMBIARE E METTERE CON ISCOMPATIBLEWITH
                
                
                const string& ref1_code = assigned_referees[i];
                const string& ref2_code = assigned_referees[j];

                // Find the referees in the input data
                const auto& ref1 = *find_if(in.refereesData.begin(), in.refereesData.end(),
                                             [&](const RA_Input::Referee& r) { return r.code == ref1_code; });
                const auto& ref2 = *find_if(in.refereesData.begin(), in.refereesData.end(),
                                             [&](const RA_Input::Referee& r) { return r.code == ref2_code; });

                // Check if they are incompatible
                if (find(ref1.incompatible_referees.begin(), ref1.incompatible_referees.end(), ref2_code) != ref1.incompatible_referees.end() ||
                    find(ref2.incompatible_referees.begin(), ref2.incompatible_referees.end(), ref1_code) != ref2.incompatible_referees.end()) {
                    violations++;
                    // std::cerr << "Incompatibility Referee violation: Referee " << ref1_code << " and Referee " << ref2_code
                    //      << " are assigned to game ID " << g << ".\n";
                }
            }
        }
    }

    return violations;
}
// A referee cannot be assigned to a game played by an incompatible team
unsigned RA_Output::TeamIncompatibility() const {
    unsigned violations = 0;

    for (unsigned g = 0; g < in.Games(); ++g) {
        const auto& game = in.gamesData[g];
        vector<string> assigned_referees = gameAssignments[g]; // Get the referees assigned to this game

        // Check for incompatibility between referees and teams
        for (size_t i = 0; i < assigned_referees.size(); ++i) {
            const string& ref_code = assigned_referees[i];
            
            // Find the referee in the input data
            const auto& ref = in.GetRefereeByCode(ref_code);
            
            // Check if the referee is assigned to a game with incompatible teams
            if (in.IsRefereeIncompatibleWithTeam(ref.code, game.homeTeam_code)) {
                violations++;
                // std::cerr << "Incompatibility Team violation: Referee " << ref_code << " is assigned to game ID " << g 
                //      << " with incompatible team: " << game.homeTeam_code << ".\n";
            }
            if (in.IsRefereeIncompatibleWithTeam(ref.code, game.guestTeam_code)) {
                violations++;
                // std::cerr << "Incompatibility Team violation: Referee " << ref_code << " is assigned to game ID " << g 
                //      << " with incompatible team: " << game.guestTeam_code << ".\n";
            }
        }
    }

    return violations;
}

// Function to compute the total number of violations
unsigned RA_Output::ComputeViolations() const {
  unsigned tot = 0;
  tot += RefereeLevel(); cout << RefereeLevel() << "\n";
  tot += ExperienceNeeded(); cout << ExperienceNeeded() << "\n";
  tot += GameDistribution(); cout << GameDistribution() << "\n";
  // tot += TotalDistance(); cout << TotalDistance() << "\n";
  tot += OptionalReferee(); cout << OptionalReferee() << "\n";
  tot += AssignmentFrequency(); cout << AssignmentFrequency() << "\n";
  tot += RefereeIncompatibility(); cout << RefereeIncompatibility() << "\n";
  tot += TeamIncompatibility(); cout << TeamIncompatibility() << "\n";
  return tot;
}
// Function to compute the cost of the solution
unsigned RA_Output::ComputeCost() const{
  return 70 * RefereeLevel() + 50 * ExperienceNeeded() +
         20 * GameDistribution() + 
         // 100 * TotalDistance() +
         10 * OptionalReferee() + 50 * AssignmentFrequency() +
         35 * RefereeIncompatibility() + 45 * TeamIncompatibility();
}

// Function to reset the output
void RA_Output::Reset(){
  gameAssignments.clear();
  teamAssignmentsPerReferee.clear();
}
// Function to dump the output to an ostream
void RA_Output::Dump(ostream& os) const {
  for (unsigned g = 0; g < in.Games(); ++g) {
    const auto& game = in.gamesData[g];
    os << game.homeTeam_code << " " << game.guestTeam_code << " " << gameAssignments[g].size();
    for (const auto& r : gameAssignments[g])
      os << " " << r;
    os << "\n";
  }

  // os << "Total violations: " << ComputeViolations() << "\n \n";
  // os << "Total cost: " << ComputeCost() << "\n \n";
  
  // for (unsigned r = 0; r < in.Referees(); ++r) {
  //   os << "Referee " << in.refereesData[r].code << " assigned to teams:\n";
  //   for (unsigned t = 0; t < in.Teams(); ++t) {
  //     if (teamAssignmentsPerReferee[r][t] > 0) {
  //       os << "  Team " << in.teamsData[t].code << ": " << teamAssignmentsPerReferee[r][t] << " times\n";
  //     }
  //   }
  // }
  // os << "\n \n";
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
    int game_id = -1;
    for (unsigned g = 0; g < out.in.Games(); ++g) {
      if (out.in.gamesData[g].homeTeam_code == home &&
          out.in.gamesData[g].guestTeam_code == guest) {
        game_id = g;
        break;
      }
    }
    if (game_id == -1) {
      std::cerr << "Errore: partita " << home << "-" << guest << " non trovata\n";
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
  if (out1.teamAssignmentsPerReferee.size() != out2.teamAssignmentsPerReferee.size()) return false;
  for (unsigned r = 0; r < out1.in.Referees(); ++r) {
    if (out1.teamAssignmentsPerReferee[r] != out2.teamAssignmentsPerReferee[r]) return false;
  }
  return true;
}