#include "TOP_Data.hpp"

#include "Utils.hpp"

#include <fstream>
#include <algorithm>

using namespace std;

// DARIO



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

    SolveDario(in, out);

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
