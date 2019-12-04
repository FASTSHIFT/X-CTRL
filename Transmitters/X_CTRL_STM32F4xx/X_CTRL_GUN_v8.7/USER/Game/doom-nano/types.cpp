#include <stdint.h>
#include <math.h>
#include "types.h"
#include "constants.h"

#define sq(val)   (val) * (val)

Coords create_coords(double x, double y) {
  Coords coords = {x, y};
  return coords;
}

uint8_t coords_distance(Coords* a, Coords* b) {
  return sqrt(sq(a->x - b->x) + sq(a->y - b->y)) * DISTANCE_MULTIPLIER;
}

UID create_uid(uint8_t type, uint8_t x, uint8_t y) {
  return (y * LEVEL_WIDTH + x) * 16 + (type & 0x0F);
}
  
uint8_t uid_get_type(UID uid) {
  return uid % 16;
}
