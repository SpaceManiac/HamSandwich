#ifndef TILE_H
#define TILE_H

// since there is no actual tilemap in the game, this just serves to
// define what the tile sizes are of the imaginary tiles

#define TILE_WIDTH (16)
#define TILE_HEIGHT (16)

#define SCR_WIDTH (480)
#define SCR_HEIGHT (480)

#define MAP_WIDTH (SCR_WIDTH/TILE_WIDTH)
#define MAP_HEIGHT (SCR_HEIGHT/TILE_HEIGHT)

#endif