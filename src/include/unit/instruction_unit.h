#ifndef INSTRUCTION_UNIT_H
#define INSTRUCTION_UNIT_H

#include "base_unit.h"
// #include "../simulator.h"
// #include "../component/Queue.h"
namespace Czar {
    class InstructionUnit:public BaseUnit {
        public:
        InstructionUnit():BaseUnit(){}
        void Flush(State *current_state);
        void Execute(State *current_state,State *next_state);
        void Display(){};
        private:
        void FetchDecode(State *current_state, State *next_state, WordType input_ins);
        void Issue(State *current_state, State *next_state);
        Queue<InsType,1024> instruction_queue;
    };
} // namespace Czar
#endif // INSTRUCTION_UNIT_H