#ifndef ARITHMETIC_LOGIC_UNIT_H
#define ARITHMETIC_LOGIC_UNIT_H
#include "base_unit.h"
#include "storage/bus.h"

namespace Czar {
class ArithmeticLogicUnit : public BaseUnit {
public:
  ArithmeticLogicUnit(Bus *cd_bus) : alu_info(1), cd_bus_(cd_bus) {}
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);
  void Display(){};

private:
  Array<AluInfo> alu_info;
  Bus *cd_bus_;
};
} // namespace Czar
#endif // ARITHMETIC_LOGIC_UNIT_H