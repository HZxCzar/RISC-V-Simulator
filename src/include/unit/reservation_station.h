#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "base_unit.h"
#include "storage/bus.h"

namespace Czar {
class ReservationStation : public BaseUnit {
public:
  explicit ReservationStation(Bus *cd_bus) : rss_info(32), cd_bus_(cd_bus) {}
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);
  void Display();

private:
  void CBD();
  Array<RssInfo> rss_info;
  Bus *cd_bus_;
};
} // namespace Czar
#endif // RESERVATION_STATION_H