#ifndef PREDICTOR_H
#define PREDICTOR_H
#include <utility>
#include <iostream>
namespace Czar {
class Predictor {
public:
  Predictor();

  bool GetPridictor(int ins_addr);
  void step(int ins_addr,int branch_match);
  void get_match_rate();

private:
  int Hash(int ins_addr);
  int predictor_mem[32][2];//0 ind 1 result
  std::pair<int,double> match_rate{0,0};
};
} // namespace Czar

#endif // PREDICTOR_H