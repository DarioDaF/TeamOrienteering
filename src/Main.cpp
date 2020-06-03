#include "TOP_Data.hpp"

#include "Utils.hpp"

#include <fstream>
#include <algorithm>

using namespace std;

// DARIO

/*

Per ogni nodo calcola il profitto, ma il profitto tiene conto dei
nodi "vicini" alla traiettoria, quindi se un nodo è nella stessa direzione
scarica il suo guadagno su quelli precedenti


*/

struct move_t {
  idx_t car;
  idx_t point;
};
bool operator==(move_t a, move_t b) {
  return a.car == b.car && a.point == b.point;
}
bool operator!=(move_t a, move_t b) {
  return a.car != b.car || a.point != b.point;
}

bool MoveNext(const TOP_Input& in, TOP_Output& out, vector<bool>& markedCar, move_t& move) {
  if(move != move_t { 0, -1 }) {
    out.RollbackCar(move.car);
    //markedCar[move.car] = false; // Clean car
    // Clean all, any car can go to the freed point :(
    // 24 s with marked and 38 s without on "p1.2.g.txt"
    fill(markedCar.begin(), markedCar.end(), false); // Slows?
  }

  ++move.point; // Do not repeat same
  // Try to assign any car one more time to non visited points
  // Sort in some way?
  for(; move.car < in.Cars(); ++move.car) {
    if(markedCar[move.car]) continue;
    for(; move.point < in.Points(); ++move.point) {
      if(out.Visited(move.point)) continue;
      // Assign
      if(out.MoveCar(move.car, move.point, false).feasible) {
        return true;
        //return move; // Used for stack rollback and move forward
      }
    }
    move.point = -1; // Reset on next car
    markedCar[move.car] = true;
  }
  move = { 0, -1 }; // ?
  return false;
  //return { 0, -1 }; // Fake/restart to signal unreachable
}

move_t MoveDown(const TOP_Input& in, TOP_Output& out, vector<bool>& markedCar) {
  move_t move = { 0, -1 };
  MoveNext(in, out, markedCar, move);
  return move;
}

void Solve(const TOP_Input &in, TOP_Output& out) {
  vector<bool> markedCar(in.Cars()); // CANNOT MARK CAR, EVERY NODE FREED CLEARS MARKED!
  vector<move_t> moveStack;

  int bestProfit = 0;
  TOP_Output bestSolution(in);
  while(true) {
    move_t move = { 0, -1 };
    while(MoveNext(in, out, markedCar, move)) {
      // Got deeper
      moveStack.push_back(move);
      move = { 0, -1 };
    }
    // Filled all
    if(moveStack.empty()) {
      // Finished (might happen if 0 solutions)
      return;
    }
    // Leaf
    int currProfit = out.PointProfit();
    if(currProfit > bestProfit) {
      cerr << "  New best solution: " << currProfit << endl;
      bestProfit = currProfit;
      bestSolution = out; // Copy
    }
    // Go to next or up
    while(!MoveNext(in, out, markedCar, moveStack.back())) {
      moveStack.pop_back();
      if(moveStack.empty()) {
        // Finished
        out = bestSolution;
        return;
      }
    }
  }
}

/*
double ComputePointProfitPerDistance(idx_t car, idx_t dest, const TOP_Input &in, const TOP_Output& out) {
  if(out.Visited(dest)) return 0;
  int profit = in.Point(dest).Profit();
  if(profit <= 0) return 0;
  double dist = out.SimulateMoveCar(car, dest).extraTravelTime;
  return profit / dist;
}

void SolveDario(const TOP_Input &in, TOP_Output& out) {
  // Sover
  NumberRange<idx_t> carIdxs(in.Cars());
  NumberRange<idx_t> pointIdxs(in.Points());
  
  vector<bool> markedCars(in.Cars());

  while(true) {
    // Assign minimum travel time car
    idx_t chosenCar = *min_element(carIdxs.begin(), carIdxs.end(), [&in, &out, &markedCars](idx_t c1, idx_t c2) {
      // If marked travel time is infinite!
      if(markedCars[c1]) return false;
      if(markedCars[c2]) return true;
      return out.TravelTime(c1) < out.TravelTime(c2);
    });

    if(markedCars[chosenCar]) {
      break; // All cars are marked
    }

    // Search point with most bucks per distance
    auto orderedPoints = pointIdxs.Vector();
    sort(orderedPoints.begin(), orderedPoints.end(), [&in, &out, &chosenCar](auto p1, auto p2) {
      return ComputePointProfitPerDistance(chosenCar, p1, in, out) > ComputePointProfitPerDistance(chosenCar, p2, in, out);
    });

    idx_t i = 0;
    bool done = false;
    while(!done) {
      if(i >= orderedPoints.size() || out.Visited(orderedPoints[i])) {
        cerr << "    Marking car " << chosenCar << endl;
        markedCars[chosenCar] = true;
        break;
      }
      done = out.MoveCar(chosenCar, orderedPoints[i], false).feasible;
      //cerr << "    Moving car " << chosenCar << " to " << orderedPoints[i] << (done ? " OK" : " FAILED") << endl;
      if(done) {
        cerr << "    Moving car " << chosenCar << " to " << orderedPoints[i] << endl;
      }

      ++i;
    }
  }
}

// KEVIN

double RatingChoice(const TOP_Input &in, const TOP_Output& out, idx_t car, idx_t p, double meanProfit) {
  if(out.Visited(p)) {
    return -INFINITY;
  }

  double profit = in.Point(p).Profit();
  //double alpha = profit / meanProfit;

  double gamma = out.TravelTime(car) / in.MaxTime();
  double extraTravelTimeNorm = out.SimulateMoveCar(car, p).extraTravelTime / (in.MaxTime() - out.TravelTime(car));

  //return alpha * profit - gamma * extraTravelTime;
  return profit / meanProfit - gamma * extraTravelTimeNorm;
}

void SolveKevin(const TOP_Input &in, TOP_Output& out) {
  // Sover
  NumberRange<idx_t> carIdxs(in.Cars());
  NumberRange<idx_t> pointIdxs(in.Points());

  vector<bool> markedCars(in.Cars());

  int notVisitedCount = 0;
  int sumProfit = 0;
  for(const auto& p : pointIdxs) {
    if(!out.Visited(p)) {
      sumProfit += in.Point(p).Profit();
      ++notVisitedCount;
    }
  }

  while(true) {
    // Assign minimum travel time car
    idx_t chosenCar = *min_element(carIdxs.begin(), carIdxs.end(), [&in, &out, &markedCars](idx_t c1, idx_t c2) {
      // If marked travel time is infinite!
      if(markedCars[c1]) return false;
      if(markedCars[c2]) return true;
      return out.TravelTime(c1) < out.TravelTime(c2);
    });

    if(markedCars[chosenCar]) {
      break; // All cars are marked
    }

    auto maxPoints = min_elements(in.Points(), so_negcmp<double>, [&in, &out, &chosenCar, &sumProfit, &notVisitedCount](idx_t p) -> double {
      if(!out.SimulateMoveCar(chosenCar, p).feasible) return -INFINITY;
      return RatingChoice(in, out, chosenCar, p, ((double)sumProfit) / notVisitedCount);
    });

    idx_t chosenPoint = maxPoints[0];

    cerr << "  Assign car " << chosenCar << " to " << chosenPoint << endl;

    if(out.Visited(chosenPoint) || !out.MoveCar(chosenCar, chosenPoint, false).feasible) {
      // Ho finito tutti i punti sicuramente di questa macchina
      markedCars[chosenCar] = true;
      continue;
    }

    // update meanProfit
    --notVisitedCount;
    sumProfit -= in.Point(chosenPoint).Profit();
  }
}
*/

#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  int errors = 0;

  TOP_Input in;
  for(const auto& file : fs::directory_iterator("./Instances")) {
    if(file.path().extension() != ".txt") continue;

    cerr << "Processing: " << file.path().filename() << endl;
    {
      ifstream is(file.path());
      if(!is) {
        ++errors;
        cerr << "  ERROR: Unable to open file" << endl;
        continue;
      }
      is >> in;
    }
    TOP_Output out(in);

    Solve(in, out);

    if(!out.Feasible()) {
      cerr << "  Invalid solution" << endl;
      cout << file.path().filename() << ',' << -1 << endl;
    } else {
      cerr << "  Solution found: " << out.PointProfit() << endl;
      cout << file.path().filename() << ',' << out.PointProfit() << endl;
    }

    {
      ofstream os("Outputs" / file.path().filename().replace_extension(".out"));
      if(!os) {
        ++errors;
        cerr << "  ERROR: Unable to open output file" << endl;
        continue;
      }
      os << in << out;
    }
  }

  cerr << "Total errors: " << ((double)errors) / 2 << endl;

  return 0;
}
