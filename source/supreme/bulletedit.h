#ifndef BULLETEDIT_H
#define BULLETEDIT_H

#include <jamultypes.h>
#include <mgldraw.h>

void BulletEdit_Init(byte modeFrom, bool allowAnything);
void BulletEdit_Exit();

void BulletEdit_Update(int mouseX, int mouseY, MGLDraw* mgl);
void BulletEdit_Render(int mouseX, int mouseY, MGLDraw* mgl);

void BulletEdit_Key(char k);

#endif
