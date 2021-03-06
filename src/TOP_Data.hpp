#ifndef TOP_DATA_HPP
#define TOP_DATA_HPP

#include "Utils.hpp"

#include <iostream>
#include <vector>
#include <cmath>

// Functions inside class are inlined

/*

Input format:
  n {Points}
  m {Cars}
  tmax {MaxTime}
  {for Points}
    {x} '\t' {y} '\t' {profit}
  {end for}

Output format: (start and end points are implicit)
  h {Hops}
  {for Hops}
    {car} '\t' {dest}
  {end for}

*/

class TOP_Point {
    friend std::ostream& operator<<(std::ostream& os, const TOP_Point& p);
    friend std::istream& operator>>(std::istream& is, TOP_Point& p);
  public:
    TOP_Point(double x, double y, int p) : x(x), y(y), p(p) {};
    TOP_Point() : TOP_Point(0, 0, 0) {};

    double X() const { return x; }
    double Y() const { return y; }
    int Profit() const { return p; }

    void Set(double x, double y, int p) { this->x = x; this->y = y; this->p = p; }
    double DistanceSq(TOP_Point p2) const { return (this->x - p2.x)*(this->x - p2.x) + (this->y - p2.y)*(this->y - p2.y); }
    double Distance(TOP_Point p2) const { return sqrt(DistanceSq(p2)); }
  private:
    double x, y;
    int p;
};

class TOP_Input {
    friend std::ostream& operator<<(std::ostream& os, const TOP_Input& in);
    friend std::istream& operator>>(std::istream& is, TOP_Input& in);
  public:
    TOP_Input() { Clear(); }
    void Clear();

    int Cars() const { return cars; }
    int Points() const { return points.size(); }
    double MaxTime() const { return max_time; }
    const TOP_Point& Point(idx_t pos) const { return points[pos]; }
    idx_t StartPoint() const { return 0; }
    idx_t EndPoint() const { return points.size() - 1; }

  private:
    int cars;
    double max_time;
    std::vector<TOP_Point> points;
};

class TOP_Output {
    friend std::ostream& operator<<(std::ostream& os, const TOP_Output& out);
    friend std::istream& operator>>(std::istream& is, TOP_Output& out);
  public:
    struct SimulateMoveCarResult {
      bool feasible;
      double extraTravelTime;
    };

    TOP_Output(const TOP_Input& in) : in(in),
      car_hops(in.Cars()), visited(in.Points()),
      travel_time(in.Cars()) { Clear(); }
    void Clear();

    bool Visited(idx_t point) const { return visited[point] > 0; }
    const SimulateMoveCarResult MoveCar(idx_t car, idx_t dest, bool force = true); // Moves the car
    const SimulateMoveCarResult SimulateMoveCar(idx_t car, idx_t dest) const; // Simulate MoveCar and returns cost difference
    idx_t CarPoint(idx_t car) const { return car_hops[car].empty() ? in.StartPoint() : car_hops[car].back(); }
    idx_t RollbackCar(idx_t car); // Rollback last move car and returns freed point
    double TravelTime(idx_t car) const { return travel_time[car]; }
    bool Feasible() const { return time_violations == 0; }
    int PointProfit() const { return point_profit; }
    
    int Hops(idx_t car) const { return car_hops[car].size() + 1; }
    idx_t Hop(idx_t car, idx_t hop) const {
      if(hop == 0) return in.StartPoint();
      if(hop > car_hops[car].size()) return in.EndPoint();
      return car_hops[car][hop - 1];
    }
  private:
    const TOP_Input& in;
    std::vector<std::vector<idx_t>> car_hops; // car_hops[car][hop] = point
    std::vector<int> visited; // number of visits
    // Redundant
    std::vector<double> travel_time;
    int point_profit;
    int time_violations;
    // Internal functions
    void IncrementVisited(idx_t point, int count);
    void IncrementTravelTime(idx_t car, double count);
};

#endif
