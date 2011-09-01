#include "rage.h"
#include "player.h"
#include "bullet.h"
#include "monster.h"
#include "guy.h"
#include "options.h"

byte rageWpn;

void ShowRage(MGLDraw *mgl)
{
	if(player.rageClock>10)
		RenderRage(15-player.rageClock,mgl);
	else if(player.rageClock<6)
	{
		if(player.rageClock>0)
			RenderRage(player.rageClock-1,mgl);
	}
	else
		RenderRage(4,mgl);
}

byte UpdateRage(MGLDraw *mgl)
{
	return 1;
}

void StartRaging(void)
{
	rageWpn=player.weapon;
	switch(player.weapon)
	{
		case WPN_NONE:
			player.rageClock=10;
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
	}
	player.rageClock+=60;
}

void DoRage(Guy *me)
{
	int cx,cy,i;

	if(player.rageClock>0)
		player.rageClock--;

	if(player.rageClock>59)
		switch(rageWpn)
		{
			case WPN_NONE:
				switch(opt.playAs)
				{
					case PLAYAS_BOUAPHA:
						if(player.rageClock==(player.rageClock/4)*4)
							HammerLaunch(me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
						break;
					case PLAYAS_LUNATIC:
						if(player.rageClock==(player.rageClock/4)*4)
						{
							for(i=0;i<10;i++)
								FireBullet(me->x,me->y,(byte)MGL_random(8),BLT_BALLLIGHTNING,1);
						}
						break;
					case PLAYAS_HAPPY:
						if(player.rageClock==(player.rageClock/4)*4)
							HappyLaunch(me->x,me->y,me->facing,5,HMR_REVERSE|HMR_REFLECT);
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
				FireBullet((cx+MGL_random(640))<<FIXSHIFT,(cy+MGL_random(480))<<FIXSHIFT,
								0,BLT_BOOM,1);
				ShakeScreen(10);	// make the screen shake!
				break;
			case WPN_AK8087:
				FireBullet(me->x,me->y,(byte)MGL_random(8),BLT_LASER,1);
				FireBullet(me->x,me->y,(byte)MGL_random(8),BLT_LASER,1);
				FireBullet(me->x,me->y,(byte)MGL_random(8),BLT_LASER,1);
				break;
			case WPN_FLAME:
				GetCamera(&cx,&cy);
				cx-=320;
				cy-=240;
				for(i=0;i<3;i++)
					FireBullet((cx+MGL_random(640))<<FIXSHIFT,(cy+MGL_random(480))<<FIXSHIFT,
									(byte)MGL_random(8),BLT_FLAME,1);
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
				FireBullet((cx+MGL_random(640))<<FIXSHIFT,(cy+MGL_random(480))<<FIXSHIFT,
					(byte)MGL_random(8),BLT_LIGHTNING,1);
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
					FireBullet((cx+MGL_random(640))<<FIXSHIFT,(cy+MGL_random(480))<<FIXSHIFT,
						(byte)MGL_random(8),BLT_SLASH,1);
				break;
			case WPN_MINES:
				if(player.rageClock==(player.rageClock/8)*8)
				{
					cx=32/8-((player.rageClock-60)/8)+1;
					for(i=0;i<8;i++)
						FireBullet(me->x+Cosine(i*32)*(cx*32),me->y+Sine(i*32)*(cx*32),
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
			case WPN_JETPACK:
				for(i=0;i<8;i++)
					FireBullet(me->x,me->y,i,BLT_FLAME,1);
				break;
		}
}
