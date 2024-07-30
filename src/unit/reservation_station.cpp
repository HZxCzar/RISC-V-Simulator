#include "unit/reservation_station.h"
#include "simulator.h"

namespace Czar {
void ReservationStation::Display() {
  std::cerr << "ReservationStation::Display\n";
}
void ReservationStation::CBD() {
  Array<int> newdata(1024);
  for (int pos = 0; pos < 8; ++pos) {
    if (cd_bus_->info.busy(pos) &&
        cd_bus_->info[pos].type_ == BusType::WriteBack) {
      newdata[cd_bus_->info[pos].pos_] = cd_bus_->info[pos].data_;
    }
  }
  for (int pos = 0; pos < 32; ++pos) {
    if (rss_info.busy(pos)) {
      if (rss_info[pos].q1_ != -1 && newdata.busy(rss_info[pos].q1_)) {
        rss_info[pos].v1_ = newdata[rss_info[pos].q1_];
        rss_info[pos].q1_ = -1;
      }
      if (rss_info[pos].q2_ != -1 && newdata.busy(rss_info[pos].q2_)) {
        rss_info[pos].v2_ = newdata[rss_info[pos].q2_];
        rss_info[pos].q2_ = -1;
      }
    }
  }
}

void ReservationStation::Flush(State *current_state) {
  if (current_state->clean_) {
    rss_info.clean();
    current_state->rss_full_ = false;
    return;
  }
  if (current_state->rss_wire_.first) {
    rss_info.push(current_state->rss_wire_.second);
    current_state->rss_wire_.first = false;
  }
  current_state->rss_full_ = rss_info.full();
  CBD();
}

void ReservationStation::Execute(State *current_state, State *next_state) {
  for (int pos = 0; pos < 32; ++pos) {
    if (rss_info.busy(pos) && rss_info[pos].q1_ == -1 &&
        rss_info[pos].q2_ == -1) {
      InsType &ins = rss_info[pos].ins_;
      AluInfo alu_info{ins, rss_info[pos].rob_pos_};
      if (ins.op_type_ == OpType::ARITHI || ins.op_ == Op::JALR) {
        alu_info.lhs_ = rss_info[pos].v1_;
        alu_info.rhs_ = ins.imm_;
      } else if (ins.op_type_ == OpType::ARITHR ||
                 ins.op_type_ == OpType::BRANCH) {
        alu_info.lhs_ = rss_info[pos].v1_;
        alu_info.rhs_ = rss_info[pos].v2_;
      } else if (ins.op_type_ == OpType::LOAD ||
                 ins.op_type_ == OpType::STORE) {
        alu_info.lhs_ = rss_info[pos].v1_;
        alu_info.rhs_ = rss_info[pos].imm_;
      } else {
        std::cerr << ins.op_type_ << std::endl;
        throw("Unknown op_type for Rss_Execute");
      }
      next_state->alu_wire_ = {true, alu_info};
      rss_info.take_out(pos);
      return;
    }
  }
}

} // namespace Czar