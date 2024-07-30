#ifndef BUS_H
#define BUS_H

#include "component/array.h"
#include <utility>
#include <vector>

namespace Czar {

enum BusType {
  WriteBack,
  WriteDes,
  CommitReg,
  LoadFinish,
  StoreFinish,
  LoadRequest,
  StoreRequest
};

struct BusInfo {
  BusType type_;
  int pos_; // rob_pos for cd_bus, ram_pos for mem_bus
  int data_;
};

struct Bus {
  explicit Bus(int width) : info(width) {}
  Array<BusInfo> info;
  // std::pair<bool,BusInfo> info[32];
};

} // namespace Czar

#endif