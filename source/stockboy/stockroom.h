#ifndef STOCKROOM_H
#define STOCKROOM_H

#include "mgldraw.h"
#include "map.h"

void InitStockroom(Map *m);

void UpdateStockroom(void);
void RenderStockroom(void);

byte StockroomMenu(MGLDraw *mgl);

char *NameLevel(void);

#endif