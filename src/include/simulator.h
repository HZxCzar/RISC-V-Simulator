// #include "../include/info/op_type.h"
// #include "../include/storage/bus.h"
// #include "../include/unit/base_unit.h"
// #include "unit/register_file.h"
// #include <algorithm>
// #include <iostream>
// #include <utility>
#include "unit/arithmetic_logic_unit.h"
#include "unit/instruction_unit.h"
#include "unit/load_store_buffer.h"
#include "unit/memory_unit.h"
#include "unit/register_file.h"
#include "unit/reorder_buffer.h"
#include "unit/reservation_station.h"

namespace Czar {
// namespace Czar
struct State {
  AddrType pc_;
  WordType input_ins_;
  InsType ins_;
  bool clean_ = false, stop_ = false , stall_ = false ;

  // check full
  bool ins_full_ = false;

  bool rob_full_ = false;

  bool rss_full_ = false;

  bool lsb_full_ = false;

  // wire info
  std::pair<bool, InsType> ins_wire_{false, InsType{}};
  std::pair<bool, RobInfo> rob_wire_{false, RobInfo{}};
  std::pair<bool, RssInfo> rss_wire_{false, RssInfo{}};
  std::pair<bool, AluInfo> alu_wire_{false, AluInfo{}};
  std::pair<bool, LsbInfo> lsb_wire_{false, LsbInfo{}};
  std::pair<bool, RobInfo> rob_lsb_wire{false, RobInfo{}};
  std::pair<bool, LsbInfo> fdback_wire{false, LsbInfo{}};

  int rob_tail=0;
  RegisterFile register_file_;
  int help=0;
};

class Simulator {
  friend class InstructionUnit;

public:
  Simulator();
  void Init(AddrType pc);
  int Run();

protected:
  // void Display();
  void Flush();//先收信号再把线给clean

private:
  int clock_{0};
  Bus *cd_bus_, *mem_bus_;
//   Predictor *predictor_;
  BaseUnit *units_[6];
  State *current_state_, *next_state_;
};
} // namespace Czar