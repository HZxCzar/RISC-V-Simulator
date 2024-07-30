#include "utils/utils.h"
#include <bits/types.h>
namespace Czar {
int SignExtend(int imm, int length) {
  // if((imm>>length)&1)
  // {
  //     return imm|(((1<<(32-length))-1)<<(length+1));
  // }
  // else
  // {
  //     return imm;
  // }
  if ((imm >> length & 1) == 0) {
    return static_cast<int>(imm);
  }
  auto base_num = static_cast<__uint32_t>(-1) << length;
  return imm | base_num;
}
} // namespace Czar