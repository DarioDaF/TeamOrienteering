#include "TOP_Data.hpp"

#include "Utils.hpp"

#include <fstream>
#include <algorithm>

#define VERSION "0.1"

using namespace std;

void Solve(const TOP_Input& in, TOP_Output& out) {
  // Sover

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
    //auto res = min_elements(pointIdxs.begin(), pointIdxs.end(), [&in](idx_t i1, idx_t i2) {
    /*
    auto res = min_elements(in.Points(), [&in](idx_t i1, idx_t i2) {
      return in.Point(i2).Profit() - in.Point(i1).Profit(); // Inverted because searching for max
    });
    */
    auto res = min_elements(in.Points(), [](auto p1, auto p2) {
      return p2 - p1; // Compare values, inverted because searching for max
    }, [&in](idx_t i) {
      return in.Point(i).Profit(); // Provide value to compare (optional, use idxs if not present)
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

}

#include <map>
#include <string>
typedef void (*solver_f)(const TOP_Input&, TOP_Output&);
const map<string, solver_f> solvers {
  {"simple", Solve},
  {"pro", Solve}
};



#include <tclap/CmdLine.h>

template<class _It, class _Op>
auto transform(_It begin, _It end, _Op op) {
  vector<decltype(op(*begin))> res(end - begin);
  std::transform(begin, end, res.begin(), op);
  return res;
}

int main(int argc, char* argv[]) {
  try {

    TCLAP::CmdLine cl("TOP solver frontend", ' ', VERSION);
    
    vector<string> solverNames(solvers.size());
    std::transform(solvers.begin(), solvers.end(), solverNames.begin(), [](auto mapEl) { return mapEl.first; });
    TCLAP::ValuesConstraint<string> argConstSolver(solverNames);

    TCLAP::ValueArg<string> argSolver("s", "solver", "Solver to use", false, "simple", &argConstSolver, cl);

    cl.parse(argc, argv);

    cout << argSolver.getValue();
  } catch(TCLAP::ArgException& e) {
    cerr << "ERORR: " << e.error() << " for arg " << e.argId() << endl;
  }
/*
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
*/
  return 0;
}
