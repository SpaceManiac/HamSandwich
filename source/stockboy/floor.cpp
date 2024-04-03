#include "floor.h"

byte FloorIsSolid(word flr)
{
	// return true if this is a totally impassable floor
	return (flr==1 || flr==21 || flr==41 || flr==61);
}

byte FloorArrowFacing(word flr)
{
	// returns the facing value if this floor is an arrow,
	// otherwise returns NOT_AN_ARROW
	if(flr==2)
		return 0;
	else if(flr==22)
		return 2;
	else if(flr==42)
		return 4;
	else if(flr==62)
		return 6;

	return NOT_AN_ARROW;
}

byte FloorCanWalk(word flr)
{
	return(!FloorIsSolid(flr) && !FloorIsHole(flr) && !FloorIsPit(flr));
}

byte FloorIsHole(word flr)
{
	return (flr==80 || flr==120);
}

byte FloorIsPit(word flr)
{
	return (flr==81 || flr==101 || flr==121 || flr==141);
}

byte FloorIsSpikes(word flr)
{
	return (flr==3 || flr==23 || flr==43 || flr==63);
}

byte FloorOnlyColor(word flr)
{
	// returns the only color allowed on this floor
	// if it is any color, it returns ANY_COLOR
	if(flr>79)
		return ANY_COLOR;
	flr=flr%20;
	if(flr>=4 && flr<12)
		return flr-4;
	else
		return ANY_COLOR;
}

byte PressurePlateState(word flr)
{
	if(flr>12+7+20*3)
		return NOT_PRESSURE_PLATE;
	
	if(flr%20>=12 && flr%20<=12+7)
	{
		if((flr/20)&1)
			return PRESSURE_DOWN;
		else
			return PRESSURE_UP;
	}
	return NOT_PRESSURE_PLATE;
}

byte FloorIsTeleporter(word flr)
{
	if(flr<90 || flr>=90+20*4)
		return 0;
	else if(flr%20>=10 && flr%20<=17)
		return 1;
	return 0;
}

byte FloorIsSummoner(word flr)
{
	if(flr>=160 && flr<=315 && ((flr%20)<16))
	   return 1;
	return 0;
}

byte FloorIsExit(word flr)
{
	if(flr==98 || flr==98+20 || flr==98+40 || flr==98+60)
		return 1;
	return 0;
}

word GetTileNumber(byte flrClass,byte color,byte facing)
{
	if(flrClass<FLR_COLOR)
	{
		return flrClass+facing*20;
	}
	else
	{
		switch(flrClass)
		{
			case FLR_COLOR:
				return 4+color+facing*20;
			case FLR_PRESSURE:
				return 12+color+facing*20;
			case FLR_DOOR:
				return 82+color+facing*20;
			case FLR_HOLE:
				return 80+facing*20;
			case FLR_PIT:
				return 81+facing*20;
			case FLR_TELEPORT:
				return 90+color+facing*20;
			case FLR_SUMMON1:
				return 160+color+facing*20;
			case FLR_SUMMON2:
				return 168+color+facing*20;
			case FLR_SUMMON3:
				return 240+color+facing*20;
			case FLR_SUMMON4:
				return 248+color+facing*20;
			case FLR_EXIT:
				return 98+facing*20;
		}
	}
	return 0;
}

void FloorTurnArrow(word *flr)
{
	*flr+=20;
	if(*flr>62)
		*flr=2;
}
