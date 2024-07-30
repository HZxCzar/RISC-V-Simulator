#include "unit/instruction_unit.h"
// #include "../include/info/op_type.h"
#include "simulator.h"
#include "utils/utils.h"

namespace Czar {
void DecodeLoad(WordType input_ins, InsType &ins) {
  int ins_type = input_ins >> 12 & 0b111;
  ins.imm_ = SignExtend(input_ins >> 20, 11);
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (ins_type == 0b000) {
    ins.op_ = Op::LB;
  } else if (ins_type == 0b001) {
    ins.op_ = Op::LH;
  } else if (ins_type == 0b010) {
    ins.op_ = Op::LW;
  } else if (ins_type == 0b100) {
    ins.op_ = Op::LBU;
  } else if (ins_type == 0b101) {
    ins.op_ = Op::LHU;
  } else {
    throw("Invalid instruction type in DecodeLoad");
  }
}

void DecodeStore(WordType input_ins, InsType &ins) {
  int ins_type = input_ins >> 12 & 0b111;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rs2_ = input_ins >> 20 & 0b11111;
  ins.imm_ = SignExtend(
      (input_ins >> 25 & 0b1111111) << 5 | (input_ins >> 7 & 0b11111), 11);
  if (ins_type == 0b000) {
    ins.op_ = Op::SB;
  } else if (ins_type == 0b001) {
    ins.op_ = Op::SH;
  } else if (ins_type == 0b010) {
    ins.op_ = Op::SW;
  } else {
    throw("Invalid instruction type in DecodeStore");
  }
}

void DecodeArithI(WordType input_ins, InsType &ins) {
  int ins_type = input_ins >> 12 & 0b111;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (ins_type == 0b000) {
    ins.op_ = Op::ADDI;
  } else if (ins_type == 0b010) {
    ins.op_ = Op::SLTI;
  } else if (ins_type == 0b011) {
    ins.op_ = Op::SLTIU;
  } else if (ins_type == 0b100) {
    ins.op_ = Op::XORI;
  } else if (ins_type == 0b110) {
    ins.op_ = Op::ORI;
  } else if (ins_type == 0b111) {
    ins.op_ = Op::ANDI;
  } else {
    ins.imm_ = input_ins >> 20 & 0b11111;
    if (ins_type == 0b001) {
      ins.op_ = Op::SLLI;
    } else if (ins_type == 0b101) {
      int funct7 = input_ins >> 25 & 0b1111111;
      if (funct7 == 0b0000000) {
        ins.op_ = Op::SRLI;
      } else if (funct7 == 0b0100000) {
        ins.op_ = Op::SRAI;
      } else {
        throw("Invalid instruction type in DecodeArithI 101");
      }
    } else {
      throw("Invalid instruction type in DecodeArithI");
    }
    return;
  }
  ins.imm_ = SignExtend(input_ins >> 20, 11);
}

void DecodeArithR(WordType input_ins, InsType &ins) {
  int ins_type = input_ins >> 12 & 0b111;
  int funct7 = input_ins >> 25 & 0b1111111;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rs2_ = input_ins >> 20 & 0b11111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (ins_type == 0b000) {
    if (funct7 == 0b0000000) {
      ins.op_ = Op::ADD;
    } else if (funct7 == 0b0100000) {
      ins.op_ = Op::SUB;
    } else {
      throw("Invalid instruction type in DecodeArithR 000");
    }
  } else if (ins_type == 0b001) {
    ins.op_ = Op::SLL;
  } else if (ins_type == 0b010) {
    ins.op_ = Op::SLT;
  } else if (ins_type == 0b011) {
    ins.op_ = Op::SLTU;
  } else if (ins_type == 0b100) {
    ins.op_ = Op::XOR;
  } else if (ins_type == 0b101) {
    if (funct7 == 0b0000000) {
      ins.op_ = Op::SRL;
    } else if (funct7 == 0b0100000) {
      ins.op_ = Op::SRA;
    } else {
      throw("Invalid instruction type in DecodeArithR 101");
    }
  } else if (ins_type == 0b110) {
    ins.op_ = Op::OR;
  } else if (ins_type == 0b111) {
    ins.op_ = Op::AND;
  } else {
    throw("Invalid instruction type in DecodeArithR");
  }
}

void DecodeBranch(WordType input_ins, InsType &ins) {
  int ins_type = input_ins >> 12 & 0b111;
  ins.imm_ = SignExtend(
      (((input_ins >> 31 & 0b1) << 12) | ((input_ins >> 7 & 0b1) << 11) |
       ((input_ins >> 25 & 0b111111) << 5) | ((input_ins >> 8 & 0b1111) << 1)),
      12);
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rs2_ = input_ins >> 20 & 0b11111;
  if (ins_type == 0b000) {
    ins.op_ = Op::BEQ;
  } else if (ins_type == 0b001) {
    ins.op_ = Op::BNE;
  } else if (ins_type == 0b100) {
    ins.op_ = Op::BLT;
  } else if (ins_type == 0b101) {
    ins.op_ = Op::BGE;
  } else if (ins_type == 0b110) {
    ins.op_ = Op::BLTU;
  } else if (ins_type == 0b111) {
    ins.op_ = Op::BGEU;
  } else {
    throw("Invalid instruction type in DecodeBranch");
  }
}

void DecodeOther(WordType input_ins, InsType &ins) {
  int ins_type = input_ins & 0b1111111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (ins_type == 0b1101111) {
    ins.imm_ = SignExtend(((input_ins >> 31 & 0b1) << 20) |
                              ((input_ins >> 12 & 0b11111111) << 12) |
                              ((input_ins >> 20 & 0b1) << 11) |
                              ((input_ins >> 21 & 0b1111111111) << 1),
                          20);
    ins.op_ = Op::JAL;
  } else if (ins_type == 0b1100111) {
    ins.rs1_ = input_ins >> 15 & 0b11111;
    ins.imm_ = SignExtend(input_ins >> 20, 11);
    ins.op_ = Op::JALR;
  } else if (ins_type == 0b0010111) {
    ins.imm_ = SignExtend((input_ins >> 12) << 12, 31);
    ins.op_ = Op::AUIPC;
  } else if (ins_type == 0b0110111) {
    ins.imm_ = SignExtend((input_ins >> 12) << 12, 31);
    ins.op_ = Op::LUI;
  } else {
    std::cerr << ins.ins_addr_ << ' ' << ins_type << '\n';
    throw("Invalid instruction type in DecodeOther");
  }
}

void Decode(WordType input_ins, InsType &ins) {
  int ins_type = input_ins & 0b1111111;
  if (ins_type == 0b0000011) {
    ins.op_type_ = OpType::LOAD;
    DecodeLoad(input_ins, ins);
  } else if (ins_type == 0b0100011) {
    ins.op_type_ = OpType::STORE;
    DecodeStore(input_ins, ins);
  } else if (ins_type == 0b0010011) {
    ins.op_type_ = OpType::ARITHI;
    DecodeArithI(input_ins, ins);
  } else if (ins_type == 0b0110011) {
    ins.op_type_ = OpType::ARITHR;
    DecodeArithR(input_ins, ins);
  } else if (ins_type == 0b1100011) {
    ins.op_type_ = OpType::BRANCH;
    DecodeBranch(input_ins, ins);
  } else {
    ins.op_type_ = OpType::OTHER;
    DecodeOther(input_ins, ins);
  }
}

void InstructionUnit::Flush(State *current_state) {
  if (current_state->clean_) {
    instruction_queue.clean();
    current_state->ins_full_ = false;
  }
  if (current_state->ins_wire_.first) {
    instruction_queue.push(current_state->ins_wire_.second);
  }
  current_state->ins_full_ = instruction_queue.full();
}

void InstructionUnit::FetchDecode(State *current_state, State *next_state,
                                  WordType input_ins) {
  // state transition & control the pc_
  if (current_state->ins_full_ || current_state->stall_ || next_state->clean_) {
    return;
  }
  InsType ins;
  ins.ins_addr_ = current_state->pc_;

  if (input_ins == 0x0ff00513) {
    next_state->stall_ = true;
  }
  Decode(input_ins, ins);
  if (ins.op_type_ == OpType::BRANCH) {
    // predict
    bool flag = true;
    ins.rd_ = 1;
    if (flag) {
      next_state->pc_ = current_state->pc_ + ins.imm_;
    } else {
      next_state->pc_ = current_state->pc_ + 4;
    }
  } else if (ins.op_ == Op::JAL) {
    next_state->pc_ = current_state->pc_ + ins.imm_;
    ins =
        InsType{ARITHI,       ADDI,    0,
                -1,           ins.rd_, static_cast<int>(current_state->pc_ + 4),
                ins.ins_addr_};
  } else if (ins.op_ == Op::JALR) {
    next_state->stall_ = true;
    next_state->help = current_state->help + 1;
  } else if (ins.op_type_ == OpType::OTHER) {
    if (ins.op_ == Op::AUIPC) {
      ins.imm_ = current_state->pc_ + ins.imm_;
    } else if (ins.op_ == Op::LUI) {
      ins = InsType{ARITHI, ADDI, 0, -1, ins.rd_, ins.imm_, ins.ins_addr_};
    }
    next_state->pc_ = current_state->pc_ + 4;
  } else {
    next_state->pc_ = current_state->pc_ + 4;
  }
  next_state->ins_wire_ = std::make_pair(true, ins);
}
void InstructionUnit::Issue(State *current_state, State *next_state) {
  if (instruction_queue.empty() || current_state->rob_full_ ||
      current_state->rss_full_ || current_state->lsb_full_) {
    return;
  }
  InsType &ins = instruction_queue.front();
  RobInfo rob_info{ins, ISSUE, current_state->rob_tail, ins.rd_};
  RssInfo rss_info{ins, current_state->rob_tail};
  if (ins.op_type_ == OpType::LOAD || ins.op_type_ == OpType::STORE) {
    LsbInfo lsb_info{ins, current_state->rob_tail, -1};
    if (ins.op_type_ == OpType::STORE) {
      rob_info.rd_ = ins.rs2_; // STORE存储rs2
    }
    if (current_state->register_file_.registers[ins.rs1_].dependency == -1) {
      rss_info.v1_ = current_state->register_file_.registers[ins.rs1_].value;
    } else {
      rss_info.q1_ =
          current_state->register_file_.registers[ins.rs1_].dependency;
    }
    rss_info.imm_ = ins.imm_;
    next_state->lsb_wire_ = {true, lsb_info};
  } else if (ins.op_type_ == OpType::ARITHI || ins.op_ == Op::JALR) {
    if (current_state->register_file_.registers[ins.rs1_].dependency == -1) {
      rss_info.v1_ = current_state->register_file_.registers[ins.rs1_].value;
    } else {
      rss_info.q1_ =
          current_state->register_file_.registers[ins.rs1_].dependency;
    }
    if (ins.op_ == Op::JALR) {
      rss_info.imm_ = ins.imm_;
    }
    rss_info.imm_ = ins.imm_;
  } else if (ins.op_type_ == OpType::ARITHR || ins.op_type_ == OpType::BRANCH) {
    if (current_state->register_file_.registers[ins.rs1_].dependency == -1) {
      rss_info.v1_ = current_state->register_file_.registers[ins.rs1_].value;
    } else {
      rss_info.q1_ =
          current_state->register_file_.registers[ins.rs1_].dependency;
    }
    if (current_state->register_file_.registers[ins.rs2_].dependency == -1) {
      rss_info.v2_ = current_state->register_file_.registers[ins.rs2_].value;
    } else {
      rss_info.q2_ =
          current_state->register_file_.registers[ins.rs2_].dependency;
    }
  }
  if (ins.op_type_ != OpType::BRANCH && ins.rd_ != -1) { //更新依赖
    next_state->register_file_.registers[ins.rd_].dependency =
        current_state->rob_tail;
  }
  instruction_queue.pop();
  next_state->rob_wire_ = {true, rob_info};
  next_state->rss_wire_ = {true, rss_info};
}

void InstructionUnit::Execute(State *current_state, State *next_state) {
  FetchDecode(current_state, next_state, current_state->input_ins_);
  Issue(current_state, next_state);
}

} // namespace Czar