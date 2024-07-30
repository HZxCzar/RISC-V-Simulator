#include "unit/load_store_buffer.h"
#include "simulator.h"
#include "utils/utils.h"
namespace Czar {
void load_store_buffer::Flush(State *current_state) {
  // std::cerr<<"load_store_buffer::Flush\n";
  if (current_state->clean_) {
    lsb_info.clean();
    current_state->lsb_full_ = false;
    return;
  }
  if (current_state->lsb_wire_.first) {
    lsb_info.push(current_state->lsb_wire_.second);
    current_state->lsb_wire_.first = false;
  }
  current_state->lsb_full_ = lsb_info.full();
  if (current_state->rob_lsb_wire.first) {
    RobInfo &rob_info_ = current_state->rob_lsb_wire.second;
    LsbInfo &lsb_info_ = lsb_info.front();
    if (lsb_info_.rob_pos_ != rob_info_.rob_pos_) {
      throw("rob_pos not match in lsb");
    }
    if (rob_info_.ins_.op_type_ == OpType::LOAD) {
      lsb_info_.rd_ = rob_info_.rd_;
      lsb_info_.addr_ = rob_info_.value_;
    } else if (rob_info_.ins_.op_type_ == OpType::STORE) {
      lsb_info_.rd_ = rob_info_.rd_;
      lsb_info_.addr_ = rob_info_.value_;
      lsb_info_.value_ = rob_info_.sv_;
    } else {
      throw("invalid op_type in lsb flush");
    }
    lsb_info_.state_ = LsbState::LSREADY;
  }
  MEM(); // not contain fdback_wire part <--
}
void load_store_buffer::MEM() {
  if (mem_bus_->info.busy(0)) {
    if (mem_bus_->info[0].type_ == BusType::LoadFinish) {
      LsbInfo &info = lsb_info.front();
      info.value_ = mem_bus_->info[0].data_ + (mem_bus_->info[1].data_ << 8) +
                    (mem_bus_->info[2].data_ << 16) +
                    (mem_bus_->info[3].data_ << 24);
      if (info.ins_.op_ == Op::LB) {
        info.value_ = SignExtend(info.value_, 1);
      } else if (info.ins_.op_ == Op::LH) {
        info.value_ = SignExtend(info.value_, 2);
      }
      info.state_ = LsbState::LSFINISH;
      mem_bus_->info.clean();
    } else if (mem_bus_->info[0].type_ == BusType::StoreFinish) {
      LsbInfo &info = lsb_info.front();
      info.state_ = LsbState::LSFINISH;
      mem_bus_->info.clean();
    }
  }
} // mem_bus_ 在simulator里 clean

void load_store_buffer::Execute(State *current_state, State *next_state) {
  if (lsb_info.empty() || mem_bus_->info.busy(0)) {
    return;
  }
  LsbInfo &info = lsb_info.front();
  if (info.state_ == LsbState::LSREADY) {
    if (info.ins_.op_type_ == OpType::LOAD) {
      if (info.ins_.op_ == Op::LB || info.ins_.op_ == Op::LBU) {
        mem_bus_->info[0] = {LoadRequest, info.addr_, 1};
      } else if (info.ins_.op_ == Op::LH || info.ins_.op_ == Op::LHU) {
        mem_bus_->info[0] = {LoadRequest, info.addr_, 2};
      } else if (info.ins_.op_ == Op::LW) {
        mem_bus_->info[0] = {LoadRequest, info.addr_, 4};
      }
    } else if (info.ins_.op_type_ == OpType::STORE) {
      int data = info.value_;
      if (info.ins_.op_ == Op::SB) {
        mem_bus_->info[0] = {StoreRequest, info.addr_, data & 0b11111111};
        mem_bus_->info.busy(1) = false;
        mem_bus_->info.busy(2) = false;
        mem_bus_->info.busy(3) = false;
      } else if (info.ins_.op_ == Op::SH) {
        mem_bus_->info[0] = {StoreRequest, info.addr_, data & 0b11111111};
        mem_bus_->info[1] = {StoreRequest, info.addr_ + 1,
                             (data >> 8) & 0b11111111};
        mem_bus_->info.busy(2) = false;
        mem_bus_->info.busy(3) = false;
      } else if (info.ins_.op_ == Op::SW) {
        mem_bus_->info[0] = {StoreRequest, info.addr_, data & 0b11111111};
        mem_bus_->info[1] = {StoreRequest, info.addr_ + 1,
                             (data >> 8) & 0b11111111};
        mem_bus_->info[2] = {StoreRequest, info.addr_ + 2,
                             (data >> 16) & 0b11111111};
        mem_bus_->info[3] = {StoreRequest, info.addr_ + 3,
                             (data >> 24) & 0b11111111};
      }
    }
    return;
  } else if (info.state_ == LsbState::LSFINISH) {
    next_state->fdback_wire = std::make_pair(true, info);
    lsb_info.pop();
  }
}
} // namespace Czar