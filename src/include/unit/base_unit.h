#ifndef BASE_UNIT_H
#define BASE_UNIT_H
#include "component/Queue.h"
#include "component/array.h"
namespace Czar {

class State;

class BaseUnit {
public:
  virtual void Flush(State *current_state) = 0;
  virtual void Execute(State *current_state, State *next_state) = 0;
  virtual void Display() = 0;
};

} // namespace Czar

#endif