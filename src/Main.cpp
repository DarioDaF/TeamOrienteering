#include "TOP_Data.hpp"

#include <fstream>

using namespace std;

void Solve(const TOP_Input &in, TOP_Output& out) {
    // Sover
}

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
