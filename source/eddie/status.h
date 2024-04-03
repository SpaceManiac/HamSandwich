#ifndef STATUS_H
#define STATUS_H

#include "jamultypes.h"
#include "coro.h"

void InitStatus(void);
void ExitStatus(void);

void SetMonitorText(byte line,const char *s);
void MonitorSwitch(byte on);

void EnterPauseMode(void);
void ExitPauseMode(void);

void RenderStatusDisplay(void);
void UpdateStatusDisplay(void);
TASK(void) EnterHighScore(void);
void RenderHiScoreDisplay(void);

#endif
