#ifndef STATUS_H
#define STATUS_H

#include "jamultypes.h"

void InitStatus(void);
void ExitStatus(void);

void SetMonitorText(byte line,char *s);
void MonitorSwitch(byte on);

void EnterPauseMode(void);
void ExitPauseMode(void);

void RenderStatusDisplay(void);
void UpdateStatusDisplay(void);
void EnterHighScore(void);
void RenderHiScoreDisplay(void);

#endif