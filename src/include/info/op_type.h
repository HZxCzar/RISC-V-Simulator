#ifndef OP_TYPE
#define OP_TYPE

#include <cstddef>
#include <cstdint>

namespace Czar {

using AddrType = uint32_t;
using WordType = uint32_t;

enum OpType { LOAD, STORE, ARITHI, ARITHR, BRANCH, OTHER };
enum Op {
  // LOAD
  LB,  // load byte
  LH,  // load halfword
  LW,  // load word
  LBU, // load byte unsigned
  LHU, // load halfword unsigned
  // STORE
  SB, // store byte
  SH, // store halfword
  SW, // store word
  // ARITHI
  ADDI,  // add immediate
  SLTI,  // set if less than immediate
  SLTIU, // set if less than immediate unsigned
  XORI,  // xor immediate
  ORI,   // or immediate
  ANDI,  // and immediate
  SLLI,  // shift left logical immediate
  SRLI,  // shift right logical immediate
  SRAI,  // shift right arithmetic immediate
  // ARITHR
  ADD,  // add
  SUB,  // substract
  SLL,  // shift left logical
  SLT,  // set if less than
  SLTU, // set if less than unsigned
  XOR,  // xor
  SRL,  // shift right logical
  SRA,  // shift right arithmetic
  OR,   // or
  AND,  // and
  // BRANCH
  BEQ,  // branch if equal
  BNE,  // branch if not equal
  BLT,  // branch if less than
  BGE,  // branch if greater than or equal
  BLTU, // branch if less than unsigned
  BGEU, // branch if greater than or equal unsigned
  // OTHER
  JAL,   // jump and link
  JALR,  // jump and link register
  AUIPC, // add upper immediate to pc
  LUI,   // load upper immediate
};

struct InsType {
  OpType op_type_;
  Op op_;
  int rs1_ = -1, rs2_ = -1;
  int rd_ =
      -1; // Also stores the prediction result  if this ins not use rd_, also -1
  int imm_ = -1;
  AddrType ins_addr_;
};
enum RobState { ISSUE, WRITE, COMMIT, LSPREP }; // EXECUTE
struct RobInfo {
  InsType ins_;
  RobState state_ = ISSUE;
  int rob_pos_ = 0;
  int rd_ = -1;
  int value_ = 0; //存储数据以及ls的地址
  int sv_ = 0;    // store_value
};

struct RssInfo {
  InsType ins_;
  int rob_pos_ = 0;
  int imm_ = 0;
  int v1_, v2_;
  int q1_ = -1, q2_ = -1;
};

struct AluInfo {
  InsType ins_;
  int rob_pos_ = 0;
  int lhs_, rhs_;
};
enum LsbState { LSREADY, LSWAIT, LSFINISH };
struct LsbInfo {
  InsType ins_;
  int rob_pos_ = 0;
  int rd_ = -1;
  int addr_ = 0;
  int value_;
  LsbState state_ = LSWAIT;
};

} // namespace Czar

#endif