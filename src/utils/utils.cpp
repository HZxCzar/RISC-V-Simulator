#include "utils/utils.h"

namespace Czar{
    int SignExtend(int imm, int length)
    {
        if(imm>>(length-1)&1)
        {
            return imm|(((1<<(32-length))-1)<<length);
        }
        else
        {
            return imm;
        }
    }
}