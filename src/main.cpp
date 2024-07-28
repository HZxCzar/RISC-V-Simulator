#include"simulator.h"
int main()
{
    Czar::Simulator riskv_cpu;
    riskv_cpu.Init(0x0);
    std::cout<<riskv_cpu.Run();
    return 0;
}