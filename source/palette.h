#ifndef PALETTE_H
#define PALETTE_H

#include <SDL2/SDL.h>
#include <stdint.h>

namespace palette {

SDL_Color getColor(uint8_t index);
uint8_t getExact(SDL_Color color);
uint8_t getNearest(SDL_Color color);

bool reduceImage(SDL_Texture *image);

} // namespace palette

#endif // PALETTE_H
