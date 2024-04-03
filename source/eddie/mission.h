#ifndef MISSION_H
#define MISSION_H

#include "jamultypes.h"

void InitMission(void);
void AssignMission(void);
void UpdateMission(void);

void TellMissionAboutIt(byte event,byte value);
byte NoMission(void);
void MissionWin(byte cause);
void RenderMissionTarget(void);

#endif