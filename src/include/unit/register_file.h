#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <cstdint>
namespace Czar {
struct Register{
    int dependency=-1;
    int value=0;
};
struct RegisterFile{
   Register registers[32]; 
};
}
#endif // REGISTER_FILE_H