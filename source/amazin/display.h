#ifndef DISPLAY_H
#define DISPLAY_H

#include <string_view>
#include "mgldraw.h"

extern MGLDraw* mgl;

void DisplayInit();
void DisplayExit();

void PrintMultiline(int x, int y, std::string_view s);
int GetStrLength(std::string_view s);
void PrintBright(int x, int y, std::string_view s, char bright);
void CenterPrintMultiline(int x, int y, std::string_view s);
void CenterPrint(int x, int y, std::string_view s);
void CenterPrintBright(int x, int y, std::string_view s, char bright);
void Print(int x, int y, std::string_view s);
void RightPrint(int x, int y, std::string_view s);
void RightPrintMultiline(int x, int y, std::string_view s);

#endif  // DISPLAY_H
