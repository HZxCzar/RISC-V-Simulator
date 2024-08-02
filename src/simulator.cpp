#include "simulator.h"

namespace Czar {
Simulator::Simulator() {
  cd_bus_ = new Bus(8);
  mem_bus_ = new Bus(4);
  current_state_ = new State;
  next_state_ = new State;
  predictor_ = new Predictor();
  units_[0] = new MemoryUnit(mem_bus_);
  units_[1] = new InstructionUnit(predictor_);
  units_[2] = new ReservationStation(cd_bus_);
  units_[3] = new ArithmeticLogicUnit(cd_bus_);
  units_[4] = new ReorderBuffer(cd_bus_, predictor_);
  units_[5] = new load_store_buffer(mem_bus_);
}
Simulator::~Simulator() {
  delete cd_bus_;
  delete mem_bus_;
  delete current_state_;
  delete next_state_;
  delete predictor_;
  for (int i = 0; i <= 5; ++i) {
    delete units_[i];
  }
}
void Simulator::Init(AddrType pc) {
  current_state_ = nullptr;
  next_state_ = new State;
  next_state_->pc_ = pc;
}
int Simulator::Run() {
  try {
    while (true) {
      Flush();
      if (current_state_->stop_) {
        // std::cout<<"total clock: "<<clock_<<std::endl;
        // predictor_->get_match_rate();
        // std::cout << "total clock number: " << clock_ << std::endl;
        return current_state_->register_file_.registers[10].value & 255U;
      }
      for (int i = 0; i <= 5; ++i) {
        units_[i]->Execute(current_state_, next_state_);
      }
    }
  } catch (const char *msg) {
    std::cerr << msg << std::endl;
    return -1;
  }
}

void Simulator::Flush() {
  ++clock_;
  delete current_state_;
  current_state_ = next_state_;
  current_state_->register_file_.registers[0] = {-1, 0};
  if (current_state_->clean_) {
    if (mem_bus_->info.busy(0)) {
      throw("mem_bus_ not clean while execute clean");
    }
    mem_bus_->info.clean();
    cd_bus_->info.clean();
    current_state_->alu_wire_.first = current_state_->fdback_wire.first =
        current_state_->ins_wire_.first = current_state_->lsb_wire_.first =
            current_state_->rob_lsb_wire.first =
                current_state_->rob_wire_.first =
                    current_state_->rss_wire_.first = false;
    for (int i = 0; i < 32; ++i) {
      current_state_->register_file_.registers[i].dependency = -1;
    }
    next_state_->help = 0;
    current_state_->stall_ = false;
  }
  for (int i = 0; i <= 5; ++i) {
    units_[i]->Flush(current_state_);
  }
  cd_bus_->info.clean();
  next_state_ = new State;
  next_state_->pc_ = current_state_->pc_;
  next_state_->register_file_ = current_state_->register_file_;
  next_state_->stall_ = current_state_->stall_;
  next_state_->help = current_state_->help;
}
} // namespace Czar