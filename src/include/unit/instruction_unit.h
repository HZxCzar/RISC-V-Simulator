#ifndef INSTRUCTION_UNIT_H
#define INSTRUCTION_UNIT_H

#include "base_unit.h"
#include "unit/predictor.h"

namespace Czar {
class InstructionUnit : public BaseUnit {
public:
  InstructionUnit(Predictor *predictor) : BaseUnit(),predictor_(predictor){}
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);
  void Display(){};

private:
  void FetchDecode(State *current_state, State *next_state, WordType input_ins);
  void Issue(State *current_state, State *next_state);
  Predictor *predictor_;
  Queue<InsType, 1024> instruction_queue;
};
} // namespace Czar
#endif // INSTRUCTION_UNIT_H