#ifndef LOAD_STORE_BUFFER_H
#define LOAD_STORE_BUFFER_H
// #include "../simulator.h"
#include "base_unit.h"
#include "storage/bus.h"
namespace Czar {
class load_store_buffer : public BaseUnit {
public:
  load_store_buffer(Bus *mem_bus) : mem_bus_(mem_bus) {}
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);
  void Display(){};

private:
  void MEM();
  Queue<LsbInfo, 32> lsb_info;
  Bus *mem_bus_;
};
} // namespace Czar

#endif // LOAD_STORE_BUFFER_H