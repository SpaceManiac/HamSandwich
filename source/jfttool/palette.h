#include <stdint.h>
#include <SDL2/SDL.h>

namespace palette {

SDL_Color getColor(uint8_t index);
uint8_t getExact(SDL_Color color);
uint8_t getNearest(SDL_Color color);

bool reduceImage(SDL_Surface *image);

} // namespace palette
