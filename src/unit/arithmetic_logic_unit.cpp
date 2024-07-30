#include "unit/arithmetic_logic_unit.h"
#include "info/op_type.h"
#include "simulator.h"
#include "storage/bus.h"

namespace Czar{
void ArithmeticLogicUnit::Flush(State *current_state) {
    // std::cerr<<"ArithmeticLogicUnit::Flush\n";
    if (current_state->clean_) {
        alu_info.clean();
        // current_state->alu_full_ = false;
        return;
    }
    if(current_state->alu_wire_.first){
        alu_info.push(current_state->alu_wire_.second);
        current_state->alu_wire_.first=false;
    }
}

void ArithmeticLogicUnit::Execute(State *current_state, State *next_state)
{
    if(!alu_info.busy(0)){
        return;
    }
    AluInfo &info=alu_info[0];
    alu_info.take_out(0);
    int ans;
    if(info.ins_.op_type_==OpType::ARITHI){
        if(info.ins_.op_==Op::ADDI){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::SLTI){
            ans=info.lhs_<info.rhs_?1:0;
        }else if(info.ins_.op_==Op::SLTIU){
            ans=static_cast<unsigned int>(info.lhs_)<static_cast<unsigned int>(info.rhs_)?1:0;
        }else if(info.ins_.op_==Op::XORI){
            ans=info.lhs_^info.rhs_;
        }else if(info.ins_.op_==Op::ORI){
            ans=info.lhs_|info.rhs_;
        }else if(info.ins_.op_==Op::ANDI){
            ans=info.lhs_&info.rhs_;
        }else if(info.ins_.op_==Op::SLLI){
            ans=info.lhs_<<info.rhs_;
        }else if(info.ins_.op_==Op::SRLI){
            ans=static_cast<unsigned int>(info.lhs_)>>info.rhs_;
        }else if(info.ins_.op_==Op::SRAI){
            ans=info.lhs_>>info.rhs_;
        }else{
            throw("Unknown op for ALU ARITHI");
        }
    }
    else if(info.ins_.op_type_==OpType::ARITHR){
        if(info.ins_.op_==Op::ADD){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::SUB){
            ans=info.lhs_-info.rhs_;
        }else if(info.ins_.op_==Op::SLL){
            ans=info.lhs_<<info.rhs_;
        }else if(info.ins_.op_==Op::SLT){
            ans=info.lhs_<info.rhs_?1:0;
        }else if(info.ins_.op_==Op::SLTU){
            ans=static_cast<unsigned int>(info.lhs_)<static_cast<unsigned int>(info.rhs_)?1:0;
        }else if(info.ins_.op_==Op::XOR){
            ans=info.lhs_^info.rhs_;
        }else if(info.ins_.op_==Op::SRL){
            ans=static_cast<unsigned int>(info.lhs_)>>info.rhs_;
        }else if(info.ins_.op_==Op::SRA){
            ans=info.lhs_>>info.rhs_;
        }else if(info.ins_.op_==Op::OR){
            ans=info.lhs_|info.rhs_;
        }else if(info.ins_.op_==Op::AND){
            ans=info.lhs_&info.rhs_;
        }else{
            throw("Unknown op for ALU ARITHR");
        }
    }
    else if(info.ins_.op_type_==OpType::BRANCH){
        if(info.ins_.op_==Op::BEQ){
            ans=info.lhs_==info.rhs_?1:0;
        }else if(info.ins_.op_==Op::BNE){
            ans=info.lhs_!=info.rhs_?1:0;
        }else if(info.ins_.op_==Op::BLT){
            ans=info.lhs_<info.rhs_?1:0;
        }else if(info.ins_.op_==Op::BLTU){
            ans=static_cast<unsigned int>(info.lhs_)<static_cast<unsigned int>(info.rhs_)?1:0;
        }else if(info.ins_.op_==Op::BGE){
            ans=info.lhs_>=info.rhs_?1:0;
        }else if(info.ins_.op_==Op::BGEU){
            ans=static_cast<unsigned int>(info.lhs_)>=static_cast<unsigned int>(info.rhs_)?1:0;
        }else{
            throw("Unknown op for ALU BRANCH");
        }
    }
    else if(info.ins_.op_type_==OpType::LOAD){
        if(info.ins_.op_==Op::LB){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::LH){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::LW){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::LBU){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::LHU){
            ans=info.lhs_+info.rhs_;
        }else{
            throw("Unknown op for ALU LOAD");
        }
    }
    else if(info.ins_.op_type_==OpType::STORE){
        if(info.ins_.op_==Op::SB){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::SH){
            ans=info.lhs_+info.rhs_;
        }else if(info.ins_.op_==Op::SW){
            ans=info.lhs_+info.rhs_;
        }else{
            throw("Unknown op for ALU STORE");
        }
    }
    else if(info.ins_.op_==Op::JALR)
    {
        ans=info.lhs_+info.rhs_;
    }
    else{
        throw("Unknown op_type for ALU");
    }
    std::cerr<<"ALU: "<<info.rob_pos_<<' '<<ans<<'\n';
    if(info.ins_.op_type_==OpType::LOAD || info.ins_.op_type_==OpType::STORE)
    {
        cd_bus_->info.push({WriteDes,info.rob_pos_,ans});
        return;
    }
    else{
        cd_bus_->info.push({WriteBack,info.rob_pos_,ans});
    }
}

} // namespace Czar