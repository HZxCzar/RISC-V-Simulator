#ifndef MEMORY_UNIT_H
#define MEMORY_UNIT_H
#include "base_unit.h"
#include <iostream>
#include "storage/bus.h"
#include "info/op_type.h"
namespace Czar {

class MemoryUnit:public BaseUnit {
 public:
  explicit MemoryUnit(Bus *mem_bus) : mem_bus_(mem_bus) {init();}
  void Flush(State *current_state);
  void Execute(State *current_state,State *next_state);
  void Display(){};
  private:
  void init();
  int memory_[1<<20];
  Bus *mem_bus_;
  int wait_{0};
};
}// namespace Czar
#endif // MEMORY_UNIT_H