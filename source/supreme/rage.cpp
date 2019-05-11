#include "rage.h"
#include "player.h"
#include "bullet.h"
#include "monster.h"
#include "guy.h"
#include "shop.h"

byte rageWpn;
byte rageFace;

void ShowRage(MGLDraw *mgl)
{
	if(player.rageClock>21)
		RenderRage(15-(player.rageClock/2),mgl);
	else if(player.rageClock<10)
	{
		if(player.rageClock>0)
			RenderRage((player.rageClock/2),mgl);
	}
	else
		RenderRage(4,mgl);
}

void SetRageFace(void)
{
	rageFace=goodguy->facing;
}

byte UpdateRage(MGLDraw *mgl)
{
	return 1;
}

void StartRaging(void)
{
	rageWpn=player.weapon;
	goodguy->facing=rageFace;
	switch(player.weapon)
	{
		case WPN_NONE:
			player.rageClock=10;
			if(player.playAs==PLAY_LUNACHIK)
				player.rageClock=40;
			break;
		case WPN_MISSILES:
			player.rageClock=16;
			break;
		case WPN_BOMBS:
			player.rageClock=20;
			break;
		case WPN_AK8087:
			player.rageClock=40;
			break;
		case WPN_FLAME:
			player.rageClock=30;
			break;
		case WPN_BIGAXE:
			player.rageClock=50;
			break;
		case WPN_LIGHTNING:
			player.rageClock=40;
			break;
		case WPN_SPEAR:
			player.rageClock=20;
			break;
		case WPN_MACHETE:
			player.rageClock=10;
			break;
		case WPN_MINES:
			player.rageClock=33;
			break;
		case WPN_MINDCONTROL:
			player.rageClock=2;
			break;
		case WPN_REFLECTOR:
			player.rageClock=60;
			break;
		case WPN_TURRET:
			player.rageClock=64;
			break;
		case WPN_JETPACK:
			player.rageClock=12;
			break;
		case WPN_SWAPGUN:
			player.rageClock=32;
			break;
		case WPN_TORCH:
			player.rageClock=30;
			break;
		case WPN_SCANNER:
			player.rageClock=4;
			break;
		case WPN_STOPWATCH:
			player.rageClock=32;
			break;
		case WPN_FREEZE:
			player.rageClock=16;
			break;
	}
}

void DoRage(Guy *me)
{
	int cx,cy,i;

	if(player.rageClock>0)
		player.rageClock--;

	switch(rageWpn)
	{
		case WPN_NONE:
			switch(player.playAs)
			{
				case PLAY_BOUAPHA:
					if(player.rageClock==(player.rageClock/4)*4)
						HammerLaunch(me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_MECHA:
					if(player.rageClock==(player.rageClock/4)*4)
						HammerLaunch(me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_LUNATIC:
					if(player.rageClock==(player.rageClock/4)*4)
					{
						for(i=0;i<10;i++)
							FireBullet(me->x,me->y,(byte)Random(8),BLT_BALLLIGHTNING,1);
					}
					break;
				case PLAY_HAPPY:
					if(player.rageClock==(player.rageClock/4)*4)
						HappyLaunch(me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_SHROOM:
					if(player.rageClock==(player.rageClock/4)*4)
						HappyLaunch(me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_LUNACHIK:
					FireExactBullet(me->x,me->y,FIXAMT*20,Cosine((player.rageClock*16))*12,Sine((player.rageClock*16))*12,0,0,50,0,BLT_LUNA2,1);
					break;
			}
			break;
		case WPN_MISSILES:
			FireBullet(me->x,me->y,(player.rageClock&7),BLT_MISSILE,1);
			break;
		case WPN_BOMBS:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
							0,BLT_BOOM,1);
			ShakeScreen(10);	// make the screen shake!
			break;
		case WPN_AK8087:
			FireBullet(me->x,me->y,(byte)Random(8),BLT_LASER,1);
			FireBullet(me->x,me->y,(byte)Random(8),BLT_LASER,1);
			FireBullet(me->x,me->y,(byte)Random(8),BLT_LASER,1);
			break;
		case WPN_FLAME:
		case WPN_TORCH:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<3;i++)
			{
				if(curMap->flags&MAP_UNDERWATER)
					FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								(byte)Random(256),BLT_BUBBLE,1);
				else
					FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								(byte)Random(8),BLT_FLAME,1);
			}
			break;
		case WPN_BIGAXE:
			if(player.rageClock==(player.rageClock/5)*5)
			{
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,me->facing,BLT_BIGAXE,1);
			}
			break;
		case WPN_LIGHTNING:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
				(byte)Random(8),BLT_LIGHTNING,1);
			break;
		case WPN_SPEAR:
			if(player.rageClock==(player.rageClock/3)*3)
			{
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,(me->facing+7)&7,BLT_SPEAR,1);
				FireBullet(me->x,me->y,me->facing,BLT_SPEAR,1);
				FireBullet(me->x,me->y,(me->facing+1)&7,BLT_SPEAR,1);
			}
			break;
		case WPN_MACHETE:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<10;i++)
				FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
					(byte)Random(8),BLT_SLASH,1);
			break;
		case WPN_MINES:
			if(player.rageClock==(player.rageClock/8)*8)
			{
				cx=32/8-((player.rageClock)/8)+1;
				for(i=0;i<8;i++)
					FireBullet(me->x+Cosine(i*32)*(cx*36),me->y+Sine(i*32)*(cx*36),
							0,BLT_BOOM,1);
			}
			break;
		case WPN_MINDCONTROL:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me->x,me->y,i,BLT_MINDWIPE,1);
			break;
		case WPN_REFLECTOR:
			FireBullet(me->x,me->y,0,BLT_REFLECT,1);
			break;
		case WPN_TURRET:
		case WPN_SWAPGUN:
			for(i=0;i<4;i++)
				FireBullet(me->x,me->y,(i*64+player.rageClock)&255,BLT_GREEN,1);
			break;
		case WPN_FREEZE:
			if(player.rageClock&1)
				for(i=0;i<3;i++)
					FireBullet(me->x,me->y,(i*85+player.rageClock*3)&255,BLT_FREEZE,1);
			break;
		case WPN_JETPACK:
			for(i=0;i<8;i++)
				if(curMap->flags&MAP_UNDERWATER)
					FireBullet(me->x,me->y,i*32,BLT_BUBBLE,1);
				else
					FireBullet(me->x,me->y,i,BLT_FLAME,1);
			break;
		case WPN_STOPWATCH:
			PlayerHeal(1);
			break;
		case WPN_SCANNER:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me->x,me->y,i,BLT_SCANNER,1);
			break;
	}
}
