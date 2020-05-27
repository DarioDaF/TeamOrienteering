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
        cerr << "Marking car " << chosenCar << endl;
        markedCars[chosenCar] = true;
        break;
      }

      done = out.MoveCar(chosenCar, orderedPoints[i], false).feasible;
      cerr << "Moving car " << chosenCar << " to " << orderedPoints[i] << (done ? " OK" : " FAILED") << endl;

      ++i;
    }
  }

/*
  NumberRange<idx_t> pointIdxs(in.Points());

  {
    cerr << "List points..." << endl;
    for(idx_t i : pointIdxs) {
      cerr << in.Point(i) << endl;
    }
  }

  {
    cerr << "Find max..." << endl;
    auto res = *max_element(pointIdxs.begin(), pointIdxs.end(), [&in](idx_t i1, idx_t i2) {
      return in.Point(i1).Profit() < in.Point(i2).Profit();
    });
    cerr << "Max: " << res << " -> " << in.Point(res) << endl;
  }

  {
    cerr << "Find multiple max..." << endl;
    auto res = min_elements(in.Points(), [&in](idx_t i1, idx_t i2) {
      return in.Point(i2).Profit() - in.Point(i1).Profit(); // Inverted because searching for max
    });
    for(const auto& el : res) {
      cerr << el << "\t-> " << in.Point(el) << endl;
    }
  }

  {
    cerr << "Sort idxs..." << endl;
    vector<idx_t> v = pointIdxs.Vector();
    sort(v.begin(), v.end(), [&in](auto i1, auto i2) {
      return in.Point(i1).Profit() > in.Point(i2).Profit();
    });
    for(const auto& el : v) {
      cerr << el << "\t-> " << in.Point(el) << endl;
    }
  }
*/

}


#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  if(argc != 2) {
    cerr << "Usage " << argv[0] << " [input_file]" << endl;
    return 1;
  }

  TOP_Input in;
  {
    ifstream is(argv[1]);
    if(!is) {
      cerr << "Unable to open file " << argv[1] << endl;
      return 1;
    }
    is >> in;
  }
  TOP_Output out(in);

  out.Clear();
  Solve(in, out);

  if(!out.Feasible()) {
    cout << "Invalid solution" << endl;
  } else {
    cout << "Solution found: " << out.PointProfit() << endl;
  }

  return 0;
}
