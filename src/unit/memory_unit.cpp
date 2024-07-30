// #include "../include/unit/memory_unit.h"
// #include "../include/simulator.h"
#include "unit/memory_unit.h"
#include "simulator.h"
#include <iostream>
namespace Czar {
int HexToInt(char insput_str) {
  return insput_str <= '9' ? insput_str - '0' : (insput_str - 'A') + 10;
}
void MemoryUnit::init() {
  char input_str[10];
  int current_address = 0x0;
  while (std::cin >> input_str) {
    if (input_str[0] == '@') {
      current_address = 0x0;
      for (int i = 1; i <= 8; i++) {
        current_address = (current_address << 4) + HexToInt(input_str[i]);
      }
    } else {
      int current_data = (HexToInt(input_str[0]) << 4) + HexToInt(input_str[1]);
      memory_[current_address++] = current_data;
    }
  }
}

void MemoryUnit::Flush(State *current_state) {
  if (!current_state->stall_) {
    current_state->input_ins_ = memory_[current_state->pc_] +
                                (memory_[current_state->pc_ + 1] << 8) +
                                (memory_[current_state->pc_ + 2] << 16) +
                                (memory_[current_state->pc_ + 3] << 24);
  }
  if (wait_ == 0 && mem_bus_->info.busy(0) == true &&
      (mem_bus_->info[0].type_ == LoadRequest ||
       mem_bus_->info[0].type_ == StoreRequest)) {
    wait_ = 2;
  }
}

void MemoryUnit::Execute(State *current_state, State *next_state) {
  if (wait_ == 0 || --wait_ != 0) {
    return;
  }
  if (mem_bus_->info.busy(0)) {
    if (mem_bus_->info[0].type_ == LoadRequest) {
      int length = mem_bus_->info[0].data_;
      int pos = mem_bus_->info[0].pos_;
      if (length == 1) {
        mem_bus_->info[0] = {LoadFinish, 0, memory_[pos]};
        mem_bus_->info[1] = {LoadFinish, 0, 0};
        mem_bus_->info[2] = {LoadFinish, 0, 0};
        mem_bus_->info[3] = {LoadFinish, 0, 0};
      } else if (length == 2) {
        mem_bus_->info[0] = {LoadFinish, 0, memory_[pos]};
        mem_bus_->info[1] = {LoadFinish, 0, memory_[pos + 1]};
        mem_bus_->info[2] = {LoadFinish, 0, 0};
        mem_bus_->info[3] = {LoadFinish, 0, 0};
      } else if (length == 4) {
        mem_bus_->info[0] = {LoadFinish, 0, memory_[pos]};
        mem_bus_->info[1] = {LoadFinish, 0, memory_[pos + 1]};
        mem_bus_->info[2] = {LoadFinish, 0, memory_[pos + 2]};
        mem_bus_->info[3] = {LoadFinish, 0, memory_[pos + 3]};
      }
    } else if (mem_bus_->info[0].type_ == StoreRequest) {
      for (int i = 0; i < 4; ++i) {
        if (mem_bus_->info.busy(i)) {
          memory_[mem_bus_->info[i].pos_] = mem_bus_->info[i].data_;
        }
      }
      mem_bus_->info[0].type_ = StoreFinish;
      mem_bus_->info.busy(1) = false;
      mem_bus_->info.busy(2) = false;
      mem_bus_->info.busy(3) = false;
    }
  }
}

} // namespace Czar