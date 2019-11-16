#include <stdint.h>
#include "entities.h"
#include "types.h"
#include "constants.h"

Player create_player(double x, double y){
    Player player = {
    create_coords((double) x + 0.5, (double) y + 0.5),
    create_coords(1, 0),
    create_coords(0, -0.66),
    0,
    100,
    };
    return player;
}

Entity create_entity(uint8_t type, uint8_t x,  uint8_t y, uint8_t initialState, uint8_t initialHealth) {
  UID uid = create_uid(type, x, y);
  Coords pos = create_coords((double) x + .5, (double) y + .5);
  Entity new_entity = { uid, pos, initialState, initialHealth, 0, 0 };
  return new_entity;
}

StaticEntity crate_static_entity(UID uid, uint8_t x,  uint8_t y, bool active) {
  StaticEntity se = { uid, x, y, active };
  return se;
}
