#include "unit/reorder_buffer.h"
#include "simulator.h"
namespace Czar {
void ReorderBuffer::CDB() {
  for (int pos = 0; pos <= cd_bus_->info.get_pointer(); ++pos) {
    if (cd_bus_->info[pos].type_ == BusType::WriteBack) {
      rob_info[cd_bus_->info[pos].pos_].value_ = cd_bus_->info[pos].data_;
      rob_info[cd_bus_->info[pos].pos_].state_ = RobState::WRITE;
    }
  }
}
void ReorderBuffer::Flush(State *current_state) {
  if (current_state->clean_) {
    rob_info.clean();
    current_state->rob_full_ = false;
    current_state->rob_tail = rob_info.tail();
    return;
  }
  if (current_state->rob_wire_.first) {
    rob_info.push(current_state->rob_wire_.second);
    current_state->rob_wire_.first = false;
  }
  current_state->rob_full_ = rob_info.full();
  CDB();
  if (current_state->fdback_wire.first) { // fdback from LSU
    if (rob_info[current_state->fdback_wire.second.rob_pos_].state_ !=
        RobState::LSPREP) {
      throw("invalid feedback");
    }
    rob_info[current_state->fdback_wire.second.rob_pos_].value_ =
        current_state->fdback_wire.second.value_;
    rob_info[current_state->fdback_wire.second.rob_pos_].state_ =
        RobState::COMMIT;
    current_state->fdback_wire.first = false;
  }
}

void ReorderBuffer::Execute(State *current_state, State *next_state) {
  RobInfo info = rob_info.front();
  if (info.state_ != RobState::WRITE && info.state_ != RobState::LSPREP) {
    return;
  }
  if (info.ins_.op_type_ == OpType::BRANCH) {
    if (info.rd_ != info.value_) {
      std::cerr << "wrong predict, make it right next time!\n";
      next_state->clean_ = true;
      next_state->pc_ = info.value_ == 1 ? current_state->pc_ + info.ins_.imm_
                                         : current_state->pc_ + 4;
    } else {
      Commit(current_state, next_state, info);
    }
    rob_info.pop();
    return;
  } else if (info.ins_.op_type_ == OpType::LOAD ||
             info.ins_.op_type_ == OpType::STORE) {
    if (info.state_ == RobState::WRITE) {
      if (info.ins_.op_type_ == OpType::STORE) {
        int reg_pos = info.ins_.rs2_;
        // if (current_state->register_file_.registers[reg_pos].dependency !=
        //     info.rob_pos_) {
        //   throw("invalid: ready to store but register has other dependency");
        // }
        info.sv_ = current_state->register_file_.registers[reg_pos].value;
      }
      //数据传输给LSB
      next_state->rob_lsb_wire = std::make_pair(true, info);
      info.state_ = RobState::LSPREP;
    } else if (info.state_ == RobState::COMMIT) {
      Commit(current_state, next_state, info);
      rob_info.pop();
      return;
    }
  } else if (info.state_ == RobState::WRITE) {
    Commit(current_state, next_state, info);
    rob_info.pop();
    return;
  } else if (info.state_ == RobState::ISSUE) {
    return;
  } else {
    throw("invalid state in  rob execute");
  }
}

void ReorderBuffer::Commit(State *current_state, State *next_state,
                           RobInfo info) {
  InsType ins_ = info.ins_;
  if (ins_.op_type_ == OpType::STORE) {
    return;
  } else if (ins_.op_type_ == OpType::LOAD) {
    if (current_state->register_file_.registers[info.rd_].dependency ==
        info.rob_pos_) {
      next_state->register_file_.registers[info.rd_].dependency = -1;
    }
    next_state->register_file_.registers[info.rd_].value = info.value_;
    info.state_ = RobState::COMMIT;
    return;
  } else if (ins_.op_type_ == OpType::ARITHI ||
             ins_.op_type_ == OpType::ARITHR) {
    if (current_state->register_file_.registers[info.rd_].dependency ==
        info.rob_pos_) {
      next_state->register_file_.registers[info.rd_].dependency = -1;
    }
    next_state->register_file_.registers[info.rd_].value = info.value_;
    info.state_ = RobState::COMMIT;
    return;
  } else if (ins_.op_type_ == OpType::OTHER) {
    if (current_state->register_file_.registers[info.rd_].dependency ==
        info.rob_pos_) {
      next_state->register_file_.registers[info.rd_].dependency = -1;
    }
    if (ins_.op_ == Op::JALR) {
      next_state->register_file_.registers[info.rd_].value =
          info.ins_.ins_addr_ + 4;
      next_state->pc_ = info.value_;
      if (current_state->stall_) {
        throw("invalid stall in commit");
      }
      next_state->stall_ = false;
    }
    info.state_ = RobState::COMMIT;
    return;
  } else if (ins_.op_type_ == OpType::BRANCH) {
    // PC在EXE阶段更新
    info.state_ = RobState::COMMIT;
    return;
  } else {
    throw("invalid op_type in commit");
  }
}

} // namespace Czar