#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H
#include "base_unit.h"
#include "storage/bus.h"
#include "unit/predictor.h"

namespace Czar {
class ReorderBuffer : public BaseUnit {
public:
  ReorderBuffer(Bus *cd_bus,Predictor *predictor) : BaseUnit(),cd_bus_(cd_bus),predictor_(predictor){}
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);
  void Display();

private:
  void CDB();
  void Commit(State *current_state, State *next_state, RobInfo &info);
  Queue<RobInfo, 32> rob_info;
  Bus *cd_bus_;
  Predictor *predictor_;
};
} // namespace Czar

#endif // REORDER_BUFFER_H