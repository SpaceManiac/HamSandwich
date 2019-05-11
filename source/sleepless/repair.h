#ifndef REPAIR_H
#define REPAIR_H

#include "mgldraw.h"

// all of these repair functions are used
// when a thing of that type is deleted.  It scratches out
// any reference to the deleted object (or sets it to a default),
// and subtracts 1 from any reference to an object greater than the deleted object.
void RepairCustomSounds(int n);
void RepairTiles(void);
void RepairItems(int n);
void RepairLevels(void);

// this table stuff is needed for the complex adjustments that can be made,
// like shifting levels around or pasting/moving/deleting blocks of tiles
void InitSwapTable(int size);
void ExitSwapTable(void);
void SwapInSwapTable(int me,int you);
void DeleteFromSwapTable(int me);
void DeleteBlockFromSwapTable(int me,int you);
void SwapBlockInSwapTable(int me,int me2,int you);
int GetSwap(int me);

#endif
