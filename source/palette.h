#ifndef PALETTE_H
#define PALETTE_H

#include "globals.h"
#include <stdint.h>

namespace palette {

Color getColor(uint8_t index);
uint8_t getExact(Color color);
uint8_t getNearest(Color color);

} // namespace palette

#endif // PALETTE_H
