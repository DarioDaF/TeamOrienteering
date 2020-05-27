#include "TOP_Data.hpp"

#include "Utils.hpp"

#include <fstream>
#include <algorithm>

using namespace std;

double ComputePointProfitPerDistance(idx_t car, idx_t dest, const TOP_Input &in, const TOP_Output& out) {
  if(out.Visited(dest)) return 0;
  int profit = in.Point(dest).Profit();
  if(profit <= 0) return 0;
  double dist = out.SimulateMoveCar(car, dest).extraTravelTime;
  return profit / dist;
}

void Solve(const TOP_Input &in, TOP_Output& out) {
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
  }

  cerr << "Total errors: " << errors << endl;

  return 0;
}
