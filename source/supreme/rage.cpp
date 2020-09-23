#include "winpch.h"
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
		case WPN_MEGAPHONE:
			player.rageClock=50;
			break;
		case WPN_LIGHTNING:
			player.rageClock=40;
			break;
		case WPN_SPEAR:
			player.rageClock=20;
			break;
		case WPN_MACHETE:
		case WPN_MEDICKIT:
		case WPN_DEATHRAY:
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
		case WPN_BLACKHOLE:
			player.rageClock=4;
			break;
		case WPN_STOPWATCH:
			player.rageClock=32;
			break;
		case WPN_FREEZE:
			player.rageClock=16;
			break;
		default:
			player.rageClock=20;
			break;
	}
}

void DoRage(Guy *me)
{
	int x,y,cx,cy,cx2,cy2,i;

	if(player.rageClock>0)
		player.rageClock--;

	switch(rageWpn)
	{
		case WPN_NONE:
			switch(player.playAs)
			{
				case PLAY_BOUAPHA:
					if(player.rageClock==(player.rageClock/4)*4)
						HammerLaunch(me,me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_MECHA:
					if(player.rageClock==(player.rageClock/4)*4)
						HammerLaunch(me,me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_LOONY:
				case PLAY_LUNATIC:
					if(player.rageClock==(player.rageClock/4)*4)
					{
						for(i=0;i<10;i++)
							FireBullet(me,me->x,me->y,(byte)Random(8),BLT_BALLLIGHTNING,1);
					}
					break;
				case PLAY_HAPPY:
					if(player.rageClock==(player.rageClock/4)*4)
						HappyLaunch(me,me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_SHROOM:
					if(player.rageClock==(player.rageClock/4)*4)
						HappyLaunch(me,me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_LUNACHIK:
					FireExactBullet(me,me->x,me->y,FIXAMT*20,Cosine((player.rageClock*16))*12,Sine((player.rageClock*16))*12,0,0,50,0,BLT_LUNA2,1);
					break;
				case PLAY_WIZ:
					if(player.rageClock==(player.rageClock/4)*4)
					{
						for(i=0;i<8;i++)
						FireExactBullet(me,me->x,me->y,FIXAMT*20,Cosine((player.rageClock*16))*12,Sine((player.rageClock*16+(256/8*i)))*12,0,0,50,0,BLT_BIGSHELL,1);
					}
					break;
				case PLAY_MYSTIC:
					if(player.rageClock==(player.rageClock/4)*4)
						HammerLaunch(me,me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
				case PLAY_WOLF:
					if(player.rageClock==(player.rageClock/4)*4)
						WolfSpew(me,me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
					break;
			}
			break;
		case WPN_MISSILES:
			FireBullet(me,me->x,me->y,(player.rageClock&7),BLT_MISSILE,1);
			break;
		case WPN_BOMBS:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
							0,BLT_BOOM,1);
			ShakeScreen(10);	// make the screen shake!
			break;
		case WPN_AK8087:
			FireBullet(me,me->x,me->y,(byte)Random(8),BLT_LASER,1);
			FireBullet(me,me->x,me->y,(byte)Random(8),BLT_LASER,1);
			FireBullet(me,me->x,me->y,(byte)Random(8),BLT_LASER,1);
			break;
		case WPN_WATERGUN:
			FireBullet(me,me->x,me->y,(byte)Random(256),BLT_SHARKGOOD,1);
			FireBullet(me,me->x,me->y,(byte)Random(256),BLT_SHARKGOOD,1);
			FireBullet(me,me->x,me->y,(byte)Random(256),BLT_SHARKGOOD,1);
			break;
		case WPN_FLAME:
		case WPN_TORCH:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<3;i++)
			{
				if(curMap->flags&MAP_UNDERWATER)
					FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								(byte)Random(256),BLT_BUBBLE,1);
				else
					FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								(byte)Random(8),BLT_FLAME,1);
			}
			break;
		case WPN_BIGAXE:
			if(player.rageClock==(player.rageClock/5)*5)
			{
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,me->facing,BLT_BIGAXE,1);
			}
			break;
		case WPN_LIGHTNING:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
				(byte)Random(8),BLT_LIGHTNING,1);
			break;
		case WPN_SPEAR:
			if(player.rageClock==(player.rageClock/3)*3)
			{
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,(me->facing+7)&7,BLT_SPEAR,1);
				FireBullet(me,me->x,me->y,me->facing,BLT_SPEAR,1);
				FireBullet(me,me->x,me->y,(me->facing+1)&7,BLT_SPEAR,1);
			}
			break;
		case WPN_MACHETE:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<10;i++)
				FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
					(byte)Random(8),BLT_SLASH,1);
			break;
		case WPN_MINES:
			if(player.rageClock==(player.rageClock/8)*8)
			{
				cx=32/8-((player.rageClock)/8)+1;
				for(i=0;i<8;i++)
					FireBullet(me,me->x+Cosine(i*32)*(cx*36),me->y+Sine(i*32)*(cx*36),
							0,BLT_BOOM,1);
			}
			break;
		case WPN_MINDCONTROL:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me,me->x,me->y,i,BLT_MINDWIPE,1);
			break;
		case WPN_REFLECTOR:
			FireBullet(me,me->x,me->y,0,BLT_REFLECT,1);
			break;
		case WPN_TURRET:
		case WPN_SWAPGUN:
			for(i=0;i<4;i++)
				FireBullet(me,me->x,me->y,(i*64+player.rageClock)&255,BLT_GREEN,1);
			break;
		case WPN_FREEZE:
			if(player.rageClock&1)
				for(i=0;i<3;i++)
					FireBullet(me,me->x,me->y,(i*85+player.rageClock*3)&255,BLT_FREEZE,1);
			break;
		case WPN_JETPACK:
			for(i=0;i<8;i++)
				if(curMap->flags&MAP_UNDERWATER)
					FireBullet(me,me->x,me->y,i*32,BLT_BUBBLE,1);
				else
					FireBullet(me,me->x,me->y,i,BLT_FLAME,1);
			break;
		case WPN_STOPWATCH:
			PlayerHeal(1);
			break;
		case WPN_SCANNER:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me,me->x,me->y,i,BLT_SCANNER,1);
			break;
		case WPN_BOOMERANG:
			if(player.rageClock==(player.rageClock/5)*5)
			{
				for(i=0;i<8;i++)
				FireBullet(me,me->x,me->y,i,BLT_BOOMERANG,1);
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
			}
			break;
		case WPN_CACTUS:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me,me->x,me->y,i*32+Random(16)-8,BLT_SPINE,1);
			MakeSound(SND_CACTONSHOOT,me->x,me->y,SND_CUTOFF,1);
			break;
		case WPN_ROCKETS:
			FireBullet(me,me->x,me->y,(player.rageClock&7),BLT_ROCKET,1);
			break;
		case WPN_MEGAPHONE:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			cx2 = cx+Random(640);
			cy2 = cx+Random(480);
			FireBullet(me,(cx2)<<FIXSHIFT,(cy2)<<FIXSHIFT,
				(byte)Random(8),BLT_SHOCKWAVE2,1);
			MakeSound(SND_ELDEROUCH,cx2,cy2,SND_CUTOFF,1200);
			break;
		case WPN_PUMPKIN:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			FireExactBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,FIXAMT*1,Cosine(0)*0,Sine(0)*0,5*FIXAMT,0,50,64,BLT_PUMPKIN,1);
			break;
		case WPN_DEATHRAY:
			FireBullet(me,me->x,me->y, (player.rageClock&7),BLT_LASER2,1);
			FireBullet(me,me->x,me->y, (player.rageClock&7)+4,BLT_LASER2,1);
			MakeSound(SND_DEATHRAY,me->x,me->y,SND_CUTOFF,1200);
			break;
		case WPN_WHOOPIE:
			if(player.rageClock&1)
				for(i=0;i<16;i++)
					FireExactBullet(me,me->x,me->y,FIXAMT*20,Cosine(i*16)*12,Sine(i*16)*12,0,0,16,i*16,BLT_SPORE,1);
			MakeSound(SND_MUSHSPORES,me->x,me->y,SND_CUTOFF,1);
			break;
		case WPN_BLACKHOLE:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me,me->x,me->y,i*32,BLT_HOLESHOT,1);
			break;
		case WPN_MEDICKIT:
			PlayerHeal(4);
			break;
		case WPN_BURNINATOR:
			if (player.rageClock % 5 == 0)
			{
				GetCamera(&cx,&cy);
				cx-=320;
				cy-=240;
				FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								0,BLT_MEGABOOM,1);
				ShakeScreen(20);	// make the screen shake!
				for(i=0;i<10;i++)
				{
					x=me->x+((256-Random(513))<<FIXSHIFT);
					y=me->y+((256-Random(513))<<FIXSHIFT);
					FireExactBullet(me,x,y,FIXAMT*80+(Random(40)<<FIXSHIFT),
									0,0,0,0,30,2,BLT_ROCK,me->friendly);
				}
			}
			break;
		case WPN_DOUBLEGUN:
			if (player.rageClock % 5 == 0)
			{
				for (i = 0; i < 5; i++)
					FireBullet(me,me->x,me->y,(byte)Random(8),BLT_LASER,1);
			}
			break;
		case WPN_BFG:
			if(player.rageClock==(player.rageClock/5)*5)
			{
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,me->facing,BLT_CHEESEHAMMER,1);
			}
			break;
		case WPN_SNOWBLOWER:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<3;i++)
			{
				if(!curMap->flags&MAP_LAVA)
					FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								(byte)Random(256),BLT_SNOWBALL,1);
				else
					FireBullet(me,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
								(byte)Random(256),BLT_SHARKGOOD,1);
			}
			break;
		case WPN_WAND:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me,me->x,me->y,i,BLT_WAND,1);
			break;
		case WPN_CONFUSE:
			if(player.rageClock&1)
				for(i=0;i<8;i++)
					FireBullet(me,me->x,me->y,i,BLT_PNKWAVE,1);
			break;
		case WPN_GRENADIER:
			FireBullet(me,me->x,me->y,(i*64+player.rageClock)&255,BLT_GRENADE,1);
			break;
	}
}
