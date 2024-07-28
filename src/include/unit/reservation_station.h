#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "base_unit.h"
#include "storage/bus.h"
// #include "../simulator.h"
// #include "../component/Queue.h"
namespace Czar{
class ReservationStation:public BaseUnit {
public:
    explicit ReservationStation(Bus *cd_bus) : cd_bus_(cd_bus),rss_info(32){}
    void Flush(State *current_state);
    void Execute(State *current_state,State *next_state);
private:
    void CBD(); 
    Array<RssInfo> rss_info;
    Bus *cd_bus_;
};
}
#endif // RESERVATION_STATION_H