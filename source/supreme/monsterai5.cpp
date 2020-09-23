//KID MYSTIC MONSTERS
void AI_Eyeguy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_EYEGUYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_EYEGUYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			// fire a shot
			if(me->aiType==MONS_DEADEYE)
			{
				FireBullet(me,me->x,me->y,me->facing*32+16-Random(33),BLT_BOMB,me->friendly);
			}
			else
			{
				FireBullet(me,me->x,me->y,me->facing,BLT_YELWAVE,me->friendly);
			}
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0 && me->reload==0)
		{
			// spit at him
			MakeSound(SND_EYEGUYSHT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy(me,goodguy);

			if(me->aiType==MONS_DEADEYE)
			{
				me->dx=Cosine(me->facing*32)*6;
				me->dy=Sine(me->facing*32)*6;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*3;
				me->dy=Sine(me->facing*32)*3;
			}
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				if(me->aiType==MONS_DEADEYE)
					me->frmAdvance=256;
				else
					me->frmAdvance=128;
					
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		if(me->aiType==MONS_DEADEYE)
		{
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
		}
		else
		{
			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
		}
		
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Peeper(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PEEPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PEEPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			// fire a shot
			FireBullet(me,me->x,me->y,me->facing*32,BLT_BIGYELLOW,me->friendly);
			MakeSound(SND_PEEPSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->reload=30*4;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// not awake yet
	{
		me->seq=ANIM_A2;	// sleep
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;

		if(goodguy && (RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0))
		{
			me->mind=1;	// wake up
			me->action=ACTION_BUSY;	// but don't do anything while waking up
			me->mind1=4;
		}
	}
	else	// awake and pissed
	{
		if(me->mind1)
			me->mind1--;
		
		else if(goodguy)
		{
			FaceGoodguy(me,goodguy);
			me->mind1=4;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		if(Random(64)==0 && me->reload==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
		}
		if(Random(256)==0)
		{
			me->seq=ANIM_A1;	// blink
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
		}
	}
}

void AI_Tower(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_TOWEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_TOWERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)	// leaping attack
		{
			if(me->frm<4)
			{
				me->dx=0;
				me->dy=0;
				me->dz=0;
			}
			else if(me->frm==4)
			{
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				me->dz=10*FIXAMT;
			}
			else if(me->frm<8)
			{
				// stay the course
				Dampen(&me->dx,FIXAMT/8);
				Dampen(&me->dy,FIXAMT/8);
			}
			else if(me->frm==12 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
				ShakeScreen(10);
			}
			else
			{
				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_A1)
		{
			if(me->frmTimer<32)
			{
				MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
				me->facing=(me->facing+1)&7;
				FireBullet(me,me->x,me->y,me->facing*32,BLT_GRENADE,me->friendly);
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT)
				me->mind=1;
		}
		if(me->ouch)
			me->mind=1;
	}
	else	// chase down Kid Mystic
	{
		if(goodguy)
		{
			if(me->mind1)
				{
					me->facing=(me->facing+3+Random(3))&7;
					me->mind1=0;
				}
				else
				{
					FaceGoodguy2(me,goodguy);
				}

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)>200*FIXAMT && me->reload==0)
			{
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=120;
			}
			if(Random(32)==0 || RangeToTarget(me,goodguy)<100*FIXAMT)
			{
				//leap!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
			}
		}
		else
			me->mind=0;
	}
}

void AI_Pinkeye(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp<=0)
		{
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
			me->reload=255;	// set the resurrection timer
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==4 && me->reload>0)
			{
				me->frm=3;	// stay still until you're done reloading!
			}
			if(me->frm==8)
			{
				// resurrect!
				me->hp=20;
				me->frm=0;
				me->seq=ANIM_IDLE;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_IDLE;
			}
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm==5 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y,(byte)((me->facing*32)-16+Random(32)),BLT_ENERGY,me->friendly);
				MakeSound(SND_PEEPSHOOT,me->x,me->y,SND_CUTOFF,1500);
			}
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
		FaceGoodguy(me,goodguy);
	if(me->reload==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->reload=30+Random(90);
	}
}


void AI_Lookeyloo(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOOKEYOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
		{
			MakeSound(SND_LOOKEYDIE,me->x,me->y,SND_CUTOFF,2000);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me,me->x-128*FIXAMT+Random(256*FIXAMT),me->y-64*FIXAMT+Random(128*FIXAMT),
				(byte)Random(256),BLT_BOOM,me->friendly);
			ShakeScreen(20);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm>=5 && me->frm<=17)
				me->mind4 = 1;
			else
				me->mind4 = 0;
			
			if(me->frm==9 && me->frmTimer<32)
			{
				LaunchMegabeam(me,me->x,me->y+FIXAMT*40,me->ID);
			}
			if(me->frm==18)
				me->reload=150;
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=32;
		me->action=ACTION_BUSY;
	}
}

void AI_Golem(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(goodguy==NULL)
	{
		// vanish if there is no goodguy to follow
		me->aiType=MONS_NONE;
		return;
	}

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOLEMOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOLEMDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			// smash
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,600);
			ShakeScreen(5);
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			FireBullet(me,x,y,me->facing,BLT_SHOCKWAVE,me->friendly);
			/*
			if(goodguy)
			{
				if(me->AttackCheck(48,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*6,Sine(me->facing*32)*6,15,map,world);
			}
			*/
			me->reload=60;
		}
		if(me->seq==ANIM_A1)	// forming
		{
			if(!me->CanWalk(me,me->x,me->y,map,world))
				me->aiType=MONS_NONE;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>3)
			{
				x=me->x-32*FIXAMT+Random(64*FIXAMT);
				y=me->y-32*FIXAMT+Random(64*FIXAMT);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		return;
	}
	if(me->mind==0)	// standard combat mode
	{
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frmAdvance=128;
		}
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(goodguy==NULL || goodguy->type==MONS_NOBODY || RangeToTarget(me,goodguy)>512*FIXAMT)
		{
			// too far from goodguy, run home
			me->mind=1;
		}
		else
		{
			if(RangeToTarget(me,goodguy)>300*FIXAMT)	// too far to fight, go home
			{
				me->mind=1;
			}
			else
			{
				if(me->mind1==0)
				{
					FaceGoodguy2(me,goodguy);
					me->mind1=4;
				}
				if(me->reload==0 && RangeToTarget(me,goodguy)<96*FIXAMT)
				{
					me->action=ACTION_BUSY;
					me->seq=ANIM_ATTACK;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->dx=0;
					me->dy=0;
					MakeSound(SND_GOLEMPOUND,me->x,me->y,SND_CUTOFF,1200);
				}
			}
		}
	}
	else if(me->mind==1)	// running home mode
	{
		if(RangeToTarget(me,goodguy)<128*FIXAMT)
		{
			if(goodguy && RangeToTarget(me,goodguy)<300*FIXAMT)
			{
				// got close enough, get violent again
				me->mind=0;
			}
			else
			{
				me->mind=2;	// close enough to the Kid, sit still
			}
		}
		if(me->mind1==0)
		{
			FaceGoodguy2(me,goodguy);
			me->mind1=4;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frmAdvance=128;
		}
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
	}
	else	// waiting patiently mode
	{
		if(me->mind1==0)
			me->mind1=4;
		if(goodguy && RangeToTarget(me,goodguy)<300*FIXAMT)
		{
			// enemy is near, get violent again
			me->mind=0;
		}
		else if(RangeToTarget(me,goodguy)>256*FIXAMT)
		{
			me->mind=1;	// got too far from home!
		}
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		me->dx=0;
		me->dy=0;
	}
}

void AI_Microfish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,2);
		}
		return;	// can't do nothin' right now
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;

	if(me->mind==0)
	{
		me->mind=Random(10)+1;
		me->facing=(byte)Random(8);
	}
	else
		me->mind--;

}

void AI_BobbyKhan(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BOBBYOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_BOBBYDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(!goodguy)
		return;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A2 && me->frm>=3)
		{
			// spin attack, begin moving
			if(me->frm<10)
			{
				me->dx+=Cosine(me->facing*32);
				me->dy+=Sine(me->facing*32);
				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);
			}
			else
			{
				Dampen(&me->dx,FIXAMT);
				Dampen(&me->dy,FIXAMT);
			}
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;

			if(!me->mind1)
				switch(me->frm)
				{
					case 4:
						if(me->AttackCheck2(x-180,y-100,x,y+80,goodguy))
						{
							goodguy->GetShot(me,goodguy,-FIXAMT*8,FIXAMT*8,32,map,world);
							goodguy->dx=-FIXAMT*8;
							goodguy->dy=FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 5:
						if(me->AttackCheck2(x-200,y-180,x,y,goodguy))
						{
							goodguy->GetShot(me,goodguy,-FIXAMT*12,0,32,map,world);
							goodguy->dx=-FIXAMT*12;
							goodguy->dy=0;
							me->mind1=1;
						}
						break;
					case 6:
						if(me->AttackCheck2(x-150,y-120,x+40,y,goodguy))
						{
							goodguy->GetShot(me,goodguy,-FIXAMT*8,-FIXAMT*8,32,map,world);
							goodguy->dx=-FIXAMT*8;
							goodguy->dy=-FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 7:
						if(me->AttackCheck2(x,y-150,x+150,y-20,goodguy))
						{
							goodguy->GetShot(me,goodguy,0,-FIXAMT*12,32,map,world);
							goodguy->dx=0;
							goodguy->dy=-FIXAMT*12;
							me->mind1=1;
						}
						break;
					case 8:
						if(me->AttackCheck2(x+20,y-100,x+230,y,goodguy))
						{
							goodguy->GetShot(me,goodguy,FIXAMT*8,-FIXAMT*8,32,map,world);
							goodguy->dx=FIXAMT*8;
							goodguy->dy=-FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 9:
						if(me->AttackCheck2(x,y,x+230,y+70,goodguy))
						{
							goodguy->GetShot(me,goodguy,FIXAMT*12,0,32,map,world);
							goodguy->dx=FIXAMT*12;
							goodguy->dy=0;
							me->mind1=1;
						}
						break;
					case 3:
					case 10:
						if(me->AttackCheck2(x-120,y,x+45,y+160,goodguy))
						{
							goodguy->GetShot(me,goodguy,FIXAMT*8,FIXAMT*8,32,map,world);
							goodguy->dx=FIXAMT*8;
							goodguy->dy=FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 11:
						if(me->AttackCheck2(x-220,y,x-10,y+120,goodguy))
						{
							goodguy->GetShot(me,goodguy,FIXAMT*12,0,32,map,world);
							goodguy->dx=0;
							goodguy->dy=FIXAMT*12;
							me->mind1=1;
						}
						break;
				}
		}

		if(me->seq==ANIM_A3 && (me->frm==9 || me->frm==10))
		{
			ShakeScreen(30);
			if(me->frm==9 && me->frmTimer<32)
				MakeSound(SND_SDZLPOUND,me->x,me->y,SND_CUTOFF,1200);

			for(i=0;i<15;i++)
			{
				x=me->x+((400-Random(801))<<FIXSHIFT);
				y=me->y+((400-Random(801))<<FIXSHIFT);
				FireExactBullet(me,x,y,FIXAMT*160+(Random(40)<<FIXSHIFT),
								0,0,0,0,30,2,BLT_ROCK,me->friendly);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=8)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_A4 && me->frm==0 && me->frmTimer<32)
		{
			for(x=0;x<6;x++)
				{
					i=Random(256);
					FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*30,Cosine(i)*4,Sine(i)*4,0,0,255,4*32,BLT_BIGYELLOW,me->friendly);
				}
		}
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->frmTimer<32)
		{
			// slash for pain
			x=(me->x>>FIXSHIFT)-64;
			y=(me->y>>FIXSHIFT);
			if(me->AttackCheck2(x,y,x+72,y+225,goodguy))
			{
				goodguy->GetShot(me,goodguy,0,FIXAMT*12,64,map,world);
				goodguy->dx=0;
				goodguy->dy=FIXAMT*12;
			}
		}

		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		x=RangeToTarget(me,goodguy);
		y=0;	// flags to decide which attack to do

		// spin ANY time he gets close
		if(x<180*FIXAMT)
		{
			y|=8;
			if(goodguy->x>me->x-64*FIXAMT && goodguy->x<me->x+64*FIXAMT &&
				goodguy->y>me->y)
			{
				y|=1;	// 1=slash attack
			}
		}
		else if(x<300*FIXAMT)	// medium range
		{
			// do the slash if he is conveniently in front of you
			if(goodguy->x>me->x-64*FIXAMT && goodguy->x<me->x+64*FIXAMT &&
				goodguy->y>me->y)
			{
				y|=1;	// 1=slash attack
			}
			y|=2;	// 2=rock stomp

			// only do block if player is below me
			if(goodguy->y>me->y)
				y|=4;	// 4=shield block
		}
		else	// long range
		{
			y|=2;	// rock stomp
			y|=8;	// spin attack from afar sometimes to close the gap
		}

		if(y!=0)
		{
			if((y&1) && (Random(10)==0))	// slash attack is highly likely
			{
				MakeSound(SND_BOBBYSLASH,me->x,me->y,SND_CUTOFF,1200);
				me->reload=30;
				me->action=ACTION_BUSY;
				me->seq=ANIM_ATTACK;	// slash
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->dx=0;
				me->dy=0;
				me->mind=0;
				return;
			}
			if((y&2) && (Random(40)==0))	// rock stomp is rare
			{
				MakeSound(SND_BOBBYSTOMP,me->x,me->y,SND_CUTOFF,1200);
				me->reload=60;
				me->action=ACTION_BUSY;
				me->seq=ANIM_A3;	// stomp
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				me->mind=0;
				return;
			}
			if((y&4) && (Random(20)==0))	// shield block is less rare, but is superceded by rock stomp
			{
				me->reload=30;
				me->action=ACTION_BUSY;
				me->seq=ANIM_A1;	// block
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				return;
			}
			if((y&8) && (Random(20)==0))	// spin attack is relatively likely
			{
				MakeSound(SND_BOBBYSPIN,me->x,me->y,SND_CUTOFF,1200);
				me->reload=60;
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;	// spin attack
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				FaceGoodguy(me,goodguy);
				me->dx=0;
				me->dy=0;
				me->mind=0;
				return;
			}
		}
	}
	if(me->ouch>2 && me->action==ACTION_IDLE && me->reload==0 && Random(16)==0 && me->mind==0)
	{
		me->reload=60;
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;	// block
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->dx=0;
		me->dy=0;
		me->mind=1;
		return;
	}
	if(me->action==ACTION_IDLE)
	{
		FaceGoodguy2(me,goodguy);
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		me->mind1=0;
	}
	if(me->seq!=ANIM_MOVE && me->action==ACTION_IDLE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Crazybush(Guy *me,Map *map,world_t *world,Guy *goodguy)
{ 
	int x,y,i,j;
	if(me->reload)
		me->reload--;
	if(me->mind2)
		me->mind2--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}
	
	if(me->aiType==MONS_BURNBUSH)
	{
		if(!me->mind2)
		{
			me->mind2 = 30;
		}
		else if (me->mind2%5==0)
		{
			FireBullet(me,me->x,me->y,me->facing*32,BLT_BADSITFLAME,me->friendly);
			me->reload=3;
		}
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(50*FIXAMT) && Random(8)==0 && map->FindGuy(me->mapx,me->mapy,5,goodguy))
		{
			// get him!
			goodguy->GetShot(me,goodguy,me->dx/2,me->dy/2,1,map,world);
		}
		FaceGoodguy2(me,goodguy);

		me->dx=Cosine(me->facing*32)*5;
		me->dy=Sine(me->facing*32)*5;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Octopus(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_OCTONOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_OCTONDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			WaterRipple(me->x/FIXAMT,me->y/FIXAMT,10*32);
			if((me->frm==4 || me->frm==10 || me->frm==16) && me->reload==0 && goodguy)
			{
				// spit stuff
				byte f;

				FaceGoodguy2(me,goodguy);
				x=me->x+Cosine(me->facing*32)*14;
				y=me->y+Sine(me->facing*32)*14;
				f=me->facing*32-12+Random(25);
				FireBullet(me,x,y,f,BLT_ENERGY,me->friendly);
				me->reload=2;
				MakeSound(SND_OCTONSHOOT,me->x,me->y,SND_CUTOFF,1200);
			}
			if(me->frm==20)
			{
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
		if(me->seq==ANIM_A2)
		{
			if(me->frm==5)
				me->mind=0;
		}
		return;	// can't do nothin' right now
	}
	
	WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*Random(32));

	if(me->mind==0)	// lurking
	{
		if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(48)==0 && me->reload==0)
		{
			ExplodeParticles(PART_WATER,me->x,me->y,0,10);
			MakeSound(SND_OCTONSPLASH,me->x,me->y,SND_CUTOFF,1200);
			WaterRipple(me->x/FIXAMT,me->y/FIXAMT,64*32);
			FaceGoodguy2(me,goodguy);
			// arise
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			me->mind=1;
			me->mind1=0;
			return;
		}

		if(me->mind1)
			me->mind1--;
		if(!me->mind1)	// time to get a new direction
		{
			me->facing=(byte)Random(8);
			me->mind1=Random(60)+1;
		}
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=Random(128)+2;
	}
	else if(me->mind==1)	// arising
	{
		if(me->frm>=5)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=200;
			me->mind=1;
			me->action=ACTION_BUSY;
		}
	}
	if((world->terrain[map->map[me->mapx+me->mapy*map->width].floor].flags&(TF_WATER|TF_LAVA))==0)
		me->GetShot(me,me,0,0,20,map,world);
}

void AI_Incabot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		me->x+=Cosine(me->facing*32)*15;
		me->y+=Sine(me->facing*32)*15;
		if(RangeToTarget(me,goodguy)<(35*FIXAMT) && me->frmTimer<128 && me->reload==0 &&
			map->FindGuy(me->mapx,me->mapy,5,goodguy))
		{
			// get him!
			goodguy->GetShot(me,goodguy,me->dx/2,me->dy/2,1,map,world);
			me->reload=4;
		}
		me->x-=Cosine(me->facing*32)*15;
		me->y-=Sine(me->facing*32)*15;
		FaceGoodguy(me,goodguy);

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Incagen(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int rng;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_MOVE && me->frm==8 && me->reload==0)
		{
			g=AddGuy(me->x,me->y+FIXAMT*2,0,MONS_INCABOT,me->friendly);
			if(g && (!g->CanWalk(me,me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			me->reload=60;
				
		}
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me,me->x-48*FIXAMT+Random(97*FIXAMT),me->y-40*FIXAMT+Random(81*FIXAMT),0,BLT_BOOM,me->friendly);
		}
		return;
	}
	rng=400*FIXAMT;

	if(!me->reload && me->mind==1 && goodguy && RangeToTarget(me,goodguy)<rng)
	{
		MakeSound(SND_INCAGEN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		map->map[me->mapx+me->mapy*map->width].templight=8;
		if(me->mapx>0)
			map->map[me->mapx-1+me->mapy*map->width].templight=0;
		if(me->mapx<map->width-1)
			map->map[me->mapx+1+me->mapy*map->width].templight=0;
		if(me->mapy>0)
			map->map[me->mapx+(me->mapy-1)*map->width].templight=0;
		if(me->mapy<map->height-1)
			map->map[me->mapx+(me->mapy+1)*map->width].templight=0;
	}
	if(goodguy && RangeToTarget(me,goodguy)<rng)
		me->mind=1;
}


void AI_Incagold(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		return;
	}

	if(me->aiType==MONS_INCAGOLD && me->mind==0)
	{
		me->mind=1;
		if(me->mapx>0 && map->map[me->mapx-1+me->mapy*map->width].wall>0)
		{
			me->facing=0;	// head right
			me->dx=FIXAMT*3/2;
			me->dy=0;
		}
		else
		{
			me->facing=4;	// head left
			me->dx=-FIXAMT*3/2;
			me->dy=0;
		}
	}
	else if(me->aiType==MONS_INCAGOLD2 && me->mind==0)
	{
		me->mind=1;
		if(me->mapy>0 && map->map[me->mapx+(me->mapy-1)*map->width].wall>0)
		{
			me->facing=2;	// head down
			me->dx=0;
			me->dy=FIXAMT*3/2;
		}
		else
		{
			me->facing=6;	// head up
			me->dx=0;
			me->dy=-FIXAMT*3/2;
		}
	}

	if(me->mind1)	// bumped a wall
	{
		me->mind1=0;

		if(me->aiType==MONS_INCAGOLD)
		{
			me->facing=4-me->facing;
			me->dx=-me->dx;
		}
		else
		{
			me->facing=8-me->facing;
			me->dy=-me->dy;
		}
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}

	if(goodguy)
	{
		char sway[]={0,1,2,3,4,3,2,1,0,-1,-2,-3,-4,-3,-2,-1};

		if(me->facing==0 && goodguy->x>me->x && abs(goodguy->y-me->y)<32*FIXAMT)
		{
			FireBullet(me,me->x,me->y,me->facing,BLT_LASER2,me->friendly);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
		if(me->facing==4 && goodguy->x<me->x && abs(goodguy->y-me->y)<32*FIXAMT)
		{
			FireBullet(me,me->x,me->y,me->facing,BLT_LASER2,me->friendly);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
		if(me->facing==2 && goodguy->y>me->y && abs(goodguy->x-me->x)<48*FIXAMT)
		{
			FireBullet(me,me->x,me->y,me->facing,BLT_LASER2,me->friendly);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
		if(me->facing==6 && goodguy->y<me->y && abs(goodguy->x-me->x)<48*FIXAMT)
		{
			FireBullet(me,me->x,me->y,me->facing,BLT_LASER2,me->friendly);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
	}
}

void AI_Incaboss(Guy *me,Map *map,world_t *world,Guy *goodguy) //add new bullets!
{
	int x,y,i,sp;

	if(me->reload)
		me->reload--;
	
	if(me->seq==ANIM_DIE)
	{
		me->dx=FIXAMT*0;
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
				  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
				  me->z,FIXAMT);
	}

	if(goodguy)
	{
		if(goodguy->y<me->y+FIXAMT*64 && abs(goodguy->x-me->x)<FIXAMT*90 && me->mind>0)
			goodguy->GetShot(me,goodguy,me->dx,me->dy,10,map,world);
		
	}

	if(me->mind==0)	// not set up yet
	{
		if(goodguy && map->FindGuy(me->mapx,me->mapy,12,goodguy) && (goodguy->y<=50*TILE_HEIGHT*FIXAMT+(FIXAMT*8)))
		{
			me->mind=1;	// mode 1 = doing nothing
			me->dx=FIXAMT*4;
			me->mind3=60;	// used as a timer
		}
		else
			return;	// wait til he is close
	}

	if(me->mind1)	// bumped a wall
	{
		me->mind1=0;
		me->dx=-me->dx;
	}

	me->mind3--;
	me->z=me->mind3*2;
	
	if(me->mind3==0)
	{
		me->mind++;
		if(me->mind==6)
			me->mind=1;
		me->mind3=60;
		
		switch(me->mind)	// setup each mode
		{
			case 1:
				me->frmAdvance=128;
				break;
			case 2:	// fire breath
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				break;
			case 3:
				break;
			case 4:	// laser eyes
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				break;
			case 5:	// tongue
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=96;
				break;
		}
	}
	switch(me->mind)
	{
		case 1:	// doing nothing
			break;
		case 2:	// fire breath
			// shoot flames
			if(me->frm>=4 && me->frm<=15)
			{
				for(i=0;i<4;i++){
					sp = floor(Random(6)+12);
					FireExactBullet(me,me->x-FIXAMT*48+Random(FIXAMT*96+1),me->y+FIXAMT*32,FIXAMT*4,Cosine(me->facing*32)*sp,Sine(me->facing*32)*sp,0,0,30,me->facing*32,BLT_FLAME5,me->friendly);
				}
			}
			if(me->seq==ANIM_IDLE)
				me->mind3=1;
			break;
		case 3:	// waiting after fire
			break;
		case 4:	// laser eyes
			// fire lasers
			if(me->frm>=6 && me->frm<=16)
			{
				if(me->reload==0)
				{
					MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
					me->reload=10;
				}
				if(me->dx>0)
				{
					FireBullet(me,me->x-FIXAMT*48,me->y-FIXAMT,me->facing,BLT_LASER2,me->friendly);
					FireBullet(me,me->x+FIXAMT*42,me->y-FIXAMT,me->facing,BLT_LASER2,me->friendly);
				}
				else
				{
					FireBullet(me,me->x-FIXAMT*48,me->y-FIXAMT,me->facing,BLT_LASER2,me->friendly);
					FireBullet(me,me->x+FIXAMT*42,me->y-FIXAMT,me->facing,BLT_LASER2,me->friendly);
				}
			}
			if(me->seq==ANIM_IDLE)
				me->mind3=1;
			break;
		case 5:	// tongue
			if(me->frm==7 && me->frmTimer<128)
				MakeSound(SND_INCATONGUE,me->x,me->y,SND_CUTOFF,100);
			break;

	}
}

void AI_Incatongue(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->seq==ANIM_DIE)
	{
		me->parent->hp=0;
		me->parent->seq=ANIM_DIE;
		me->parent->action=ACTION_BUSY;
		me->parent->frm=0;
		me->parent->frmTimer=0;
		me->parent->frmAdvance=64;
	}

	if(me->action==ACTION_BUSY)
	{
		return;
	}

	me->x=me->parent->x;
	if(me->parent->seq==ANIM_A1 && me->parent->frm>5)
	{
		me->y=me->parent->y+FIXAMT*48;
		monsType[me->aiType].size=50;
		monsType[me->aiType].flags&=(~MF_INVINCIBLE);
		if(me->reload==0 && me->parent->frm<20)
		{
			me->reload=3;
			x=me->x+Cosine((me->facing*32)&255)*8;
			y=me->y+Sine((me->facing*32)&255)*8;
			x+=Cosine(me->facing*32)*12;
			y+=Sine(me->facing*32)*12;
			FireBullet(me,x,y,(me->facing*32-17+Random(16))&255,BLT_BIGSHELL,me->friendly);
		}
	}
	else
	{
		me->y=me->parent->y-FIXAMT*32;
		monsType[me->aiType].flags|=MF_INVINCIBLE;
		monsType[me->aiType].size=5;
	}
}

void AI_PeepBomb(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=256;
			me->facing=0;
		}
		return;
	}

	if(me->mind==0)	// spinning, looking for goodguy
	{
		if(me->aiType==MONS_PEEPBOMB)
		{
			me->mind1++;
		}
		else
		{
			me->mind1--;

		}
		me->facing=me->mind1/8;	// pick the frame

		if(PeepAtKid(me->x,me->y,map,me->mind1))
		{
			MakeSound(SND_PEEPALARM,me->x,me->y,SND_CUTOFF|SND_ONE,3000);
			me->mind=1;
			me->dx=Cosine(me->mind1)*7;
			me->dy=Sine(me->mind1)*7;
			me->mind1=30;
		}
	}
	if(me->mind==1)	// chasing!
	{
		me->mind1--;
		if(!me->mind1)
		{
			me->mind=0;
			me->dx=0;
			me->dy=0;
			me->mind1=me->facing*8;
		}
		else
		{
			FaceGoodguy(me,goodguy);
			me->facing*=4;
			me->dx=Cosine(me->facing*8 )*7;
			me->dy=Sine(me->facing*8)*7;
			if(RangeToTarget(me,goodguy)<FIXAMT*64)
			{
				me->hp=0;
				me->facing=0;
				me->seq=ANIM_DIE;
				me->frmAdvance=256;
				me->frm=0;
				me->frmTimer=0;
				me->dx=0;
				me->dy=0;
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
				goodguy->GetShot(me,goodguy,-Cosine(me->facing*8)*20,-Sine(me->facing*8)*20,30,map,world);
				me->action=ACTION_BUSY;
			}
		}
	}
}

void AI_Trapazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;
	if(me->mind3==0)
	me->mind3=1;
	
	if(me->mind3==1)
	{
		if(me->hp==0)
			return;	// don't sweat it if you dead

		if(goodguy && RangeToTarget(me,goodguy)<150*FIXAMT && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			me->mind3=2;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->mind1=0;
			return;
		}
	}
	else
	{
		if(me->ouch==4)
		{
			if(me->hp>0)
				MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
		}

		if(me->action==ACTION_BUSY)
		{
			if(me->seq==ANIM_DIE && me->frm==6)
			{
				me->mind3=1;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
			}
			if(me->seq==ANIM_ATTACK)
			{
				if(me->frm>=6 && me->frm<=8)
				{	
					if(me->aiType==MONS_TRAPAZOID2)
					FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(me->facing*32+(me->frm-6)*10)*8,Sine(me->facing*32+(me->frm-6)*10)*8,0,0,32,me->facing*32+(me->frm-6)*10,BLT_BIGSHELL,me->friendly);
					else
					FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(me->facing*32+(me->frm-6)*10)*8,Sine(me->facing*32+(me->frm-6)*10)*8,0,0,32,me->facing*32+(me->frm-6)*10,BLT_ENERGY,me->friendly);
				}
				if(me->frm==9)
				{
					me->frm=6;
					me->facing=(me->facing+1)&7;
					me->mind1++;
					if(me->mind1==8)
					{
						me->seq=ANIM_DIE;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
					}
				}
			}

			return;
		}
	}
}

void AI_Whackazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;
	if(me->mind3==0)
	me->mind3=1;
	
	if(me->mind3==1)
	{
		if(me->hp==0)
			return;	// don't sweat it if you dead

		if(goodguy && RangeToTarget(me,goodguy)<150*FIXAMT && me->reload==0 && Random(64)==1)
		{
			FaceGoodguy(me,goodguy);
			me->mind3=2;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->mind1=0;
			return;
		}
	}
	else
	{
		if(me->ouch==4)
		{
			if(me->hp>0)
				MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
		}

		if(me->action==ACTION_BUSY)
		{
			if(me->seq==ANIM_DIE && me->frm==6)
			{
				me->mind3=1;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
			}
			if(me->seq==ANIM_ATTACK)
			{
				if(me->frm==9)
				{
					me->frm=6;
					me->facing=(me->facing+1)&7;
					me->mind1++;
					if(me->mind1==8)
					{
						me->seq=ANIM_DIE;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
					}
				}
			}
			return;
		}
	}
}

void AI_Slug(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SNAILDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SNAILHIT,me->x,me->y,SND_CUTOFF,1200);
		me->facing=(me->facing+1-Random(3))&7;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && Random(7)==0)
		{
			FireBullet(me,me->x-FIXAMT*40+Random(FIXAMT*81),me->y-FIXAMT*30+Random(FIXAMT*61),0,BLT_SLIME,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1)	// bumped a wall
	{
		me->facing=(byte)Random(8);
		me->mind1=0;
	}

	if(me->reload==0 && goodguy && RangeToTarget(me,goodguy)<800*FIXAMT)
	{
		me->reload=10;
		FireBullet(me,me->x,me->y,0,BLT_SLIME,me->friendly);
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Snail(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;
	byte b;
	
	if((me->seq==ANIM_A1 && me->frm==8)||(me->seq==ANIM_A2 && me->frm==2))
	{
		if(me->seq==ANIM_A1)
		me->mind2=1;
		else
		me->mind2=0;
	}

	if(me->mind3==1)
	{
		if(goodguy && RangeToTarget(me,goodguy)>(260*FIXAMT))
		{
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->mind3=0;
		}
		else
		{
			me->seq=ANIM_A3;
			me->frmAdvance=0;
		}

		return;
	}

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SNAILDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SNAILHIT,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==10)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
			me->mind3=1;
		}

		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=5)
		{
			// fire lots of acid
			for(i=0;i<2;i++)
			{
				x=me->x+Cosine(me->facing*32)*20-FIXAMT*4+Random(FIXAMT*4+1);
				y=me->y+Sine(me->facing*32)*20-FIXAMT*4+Random(FIXAMT*4+1);
				b=me->facing*32-4+Random(9);
				FireExactBullet(me,x,y,FIXAMT*15,Cosine(b)*12,Sine(b)*12,FIXAMT*6,0,60,((b+16)&255)/32,BLT_ACID,me->friendly);
			}
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;
		return;
	}

	if(goodguy && RangeToTarget(me,goodguy)<(240*FIXAMT))
	{
		// hide in your shell!
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}

	// spit if close enough and you want to
	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT && me->reload==0)
	{
		FaceGoodguy(me,goodguy);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=60;
	}
}

void AI_Ball(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(goodguy && RangeToTarget(me,goodguy)<40*FIXAMT && me->dy>=0)
	{
		MakeSound(SND_BOING+Random(3),me->x,me->y,SND_CUTOFF,2);
		if(me->dy==0)
			me->dy=-FIXAMT*4;
		else
			me->dy=-me->dy;
		me->dx=(goodguy->dx/4)+Cosine(goodguy->facing*32)*2;
	}

	if(me->mind1)
	{
		MakeSound(SND_BOING+Random(3),me->x,me->y,SND_CUTOFF,2);
		if(me->mind1&1)	// horiz hit
		{
			me->dx=-me->dx;
			me->dy+=-FIXAMT/16+Random(FIXAMT/8+1);
		}
		if(me->mind1&2)	// vert hit
		{
			me->dy=-me->dy;
			me->dx+=-FIXAMT/16+Random(FIXAMT/8+1);
		}
		me->mind1=0;
	}

	if(world->terrain[map->map[me->mapx+me->mapy*map->width].floor].flags&(TF_WATER|TF_LAVA))
	{
		FireBullet(me,me->x,me->y,0,BLT_MEGABOOM,0);
		FireBullet(me,me->x,me->y,0,BLT_MEGABOOM,1);
		me->aiType=MONS_NONE;
	}

	Clamp(&me->dx,FIXAMT*5);
	Clamp(&me->dy,FIXAMT*5);
}

void AI_OctoBoss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	for(i=0;i<8;i++)
	{
		x=me->x/FIXAMT-60+Random(121);
		y=me->y/FIXAMT-60+Random(121);
		WaterRipple(x,y,Random(32*20));
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=4 && me->frm<=11)
		{
			x=me->x-FIXAMT*30+Random(60*FIXAMT+1);
			y=me->y+FIXAMT*70;
			FireBullet(me,x,y,2*32-14+Random(29),BLT_ENERGY,me->friendly);
			me->reload=60;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		x=goodguy->x;
		y=goodguy->y-50*FIXAMT;

		FaceGoodguy2(me,goodguy);
		if(me->x<x)
			me->dx+=FIXAMT/8;
		if(me->y<y)
			me->dy+=FIXAMT/8;
		if(me->x>x)
			me->dx-=FIXAMT/8;
		if(me->y>y)
			me->dy-=FIXAMT/8;

		if(me->mind1)
		{
			me->mind1=0;
			me->dy-=FIXAMT/4;	// move up if bumping an obstacle
		}
		Clamp(&me->dx,FIXAMT*3);
		Clamp(&me->dy,FIXAMT*3);

		if(me->reload==0 && (abs(me->x-x)<150*FIXAMT))	// within a reasonable horiz range
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
		}
	}
}

void AI_OctoTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(!me->parent || me->parent->hp==0)
	{
		me->GetShot(me,me,0,0,255,map,world);
	}

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD3OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD3DIE,me->x,me->y,SND_CUTOFF,1200);
	}
	
	WaterRipple(me->x/FIXAMT,me->y/FIXAMT,MGL_random(32*20));

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==9 && me->reload==0)
		{
			if(!FindVictim(me,(me->x+Cosine(me->facing*32)*160)/FIXAMT,(me->y+Sine(me->facing*32)*100)/FIXAMT,60,
				Cosine(me->facing*32)*10,Sine(me->facing*32)*10,8,map,world,me->friendly))
			{
				FindVictim(me,(me->x+Cosine(me->facing*32)*40)/FIXAMT,(me->y+Sine(me->facing*32)*30)/FIXAMT,80,
					Cosine(me->facing*32)*10,Sine(me->facing*32)*10,8,map,world,me->friendly);
			}
			me->reload=4;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(200*FIXAMT) && Random(8)==0)
			{
				// get him!
				FaceGoodguy(me,goodguy);
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(Random(20)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=10;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=10;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(200*FIXAMT) && Random(8)==0)
			{
				// get him!
				FaceGoodguy(me,goodguy);
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(7)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
		}
	}

	if(me->y<me->parent->y-FIXAMT*140)
	{
		me->dy=FIXAMT*4;
		me->mind=1;
		me->mind1=10;
	}
	if(me->y>me->parent->y+FIXAMT*140)
	{
		me->dy=-FIXAMT*4;
		me->mind=1;
		me->mind1=10;
	}
	if(me->aiType==MONS_OCTOTENT)	// left-side tentacle
	{
		if(me->x<me->parent->x-FIXAMT*220)
		{
			me->dx=FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
		if(me->x>me->parent->x-FIXAMT*30)
		{
			me->dx=-FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
	}
	else
	{
		if(me->x>me->parent->x+FIXAMT*220)
		{
			me->dx=-FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
		if(me->x<me->parent->x+FIXAMT*30)
		{
			me->dx=FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
	}

	if(RangeToTarget(me,me->parent)<FIXAMT*100)
	{
		if(me->x<me->parent->x)
			me->dx-=FIXAMT*6;
		if(me->x>me->parent->x)
			me->dx+=FIXAMT*6;
		if(me->y<me->parent->y)
			me->dy-=FIXAMT*6;
		if(me->y>me->parent->y)
			me->dy+=FIXAMT*6;
		me->mind=1;
		me->mind1=10;
	}
}

void AI_Rammy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOATHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(!goodguy)
		return;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_MOVE)
		{
			if(me->mind<20)
			{
				me->mind++;
				me->mind1=0;	// don't let him bump stuff when slow
			}
			else
			{
				x=me->x+Cosine(me->facing*32)*40;
				y=me->y+Sine(me->facing*32)*40;
			}

				if(FindVictim(me,x>>FIXSHIFT,y>>FIXSHIFT,40,Cosine(me->facing*32)*10,Sine(me->facing*32)*10,20,map,world,me->friendly)||me->mind1)
				{
					// make THUD noise
					MakeSound(SND_GOATDIZZY,me->x,me->y,SND_CUTOFF,1200);
					me->mind3=1;	// become vulnerable
					me->seq=ANIM_A1;	// get stunned
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->action=ACTION_BUSY;
					me->dx=0;
					me->dy=0;
					ShakeScreen(10);
					me->mind1=0;
					return;
				}

			me->dx+=Cosine(me->facing*32)/2;
			me->dy+=Sine(me->facing*32)/2;
			Clamp(&me->dx,FIXAMT*8);
			Clamp(&me->dy,FIXAMT*8);
			if(me->reload==0)
			{
				me->reload=10;
				FaceGoodguy2(me,goodguy);
			}
			if(me->frm==12)
			{
				me->frm=0;
				MakeSound(SND_GOATRUN,me->x,me->y,SND_CUTOFF,1200);
			}
			if(Random(3)==0)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z-Random(FIXAMT*60),FIXAMT);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==22)
		{
			me->mind3=0;	// invulnerable again
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
		}
		return;	// can't do nothin' right now
	}

	FaceGoodguy(me,goodguy);

	if(Random(48)==0)
	{
		MakeSound(SND_GOATRUN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=10;
		me->mind=0;
		return;
	}
}

void AI_Hoppy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOATHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->mind3==1)
		me->action=ACTION_BUSY;

	if(me->action==ACTION_BUSY)
	{
		if(me->z>0 )
			me->dz+=FIXAMT/4;
		if(me->seq==ANIM_A2 && me->frm==3)
		{
			me->mind3=1;
			me->dz=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*5;
			me->dy=Sine(me->facing*32)*5;
			me->z=FIXAMT;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}

		if(me->z==0 && me->mind3==1 && me->seq==ANIM_IDLE)
		{
			// thud sound again
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			ShakeScreen(10);
			me->mind3=0;
			me->seq=ANIM_A3;	// squish onto the ground
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->mind1=0;
			FireBullet(me,me->x,me->y,0,BLT_SHOCKWAVE,me->friendly);
		}

		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(16)==0)
	{
		// get him!
		FaceGoodguy(me,goodguy);
		MakeSound(SND_GOATSHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	if(!me->mind1)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind1=Random(40)+1;
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
	}
	else
		me->mind1--;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Gruffy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOATHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*24;
			y=me->y+Sine(me->facing*32)*24;
			FireBullet(me,x,y,(byte)(me->facing*32-8+Random(17)),BLT_BIGYELLOW,me->friendly);
			me->reload=5;
			me->mind1=1;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0)
			{
				// spit at him
				MakeSound(SND_GOATSHOOT,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}

			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0)
			{
				// spit at him
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickShroom(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			i=(me->facing*32-8+Random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireExactBullet(me,x,y,FIXAMT*20,Cosine(i)*8,Sine(i)*8,0,0,16,i,BLT_SPORE,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(128)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)Random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)Random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(8)==0)
		{
			// get him!
			MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		FaceGoodguy2(me,goodguy);

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickSpider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0)
		{
			i=(me->facing*32-8+Random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireBullet(me,x,y,i,BLT_ACID,me->friendly);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(128)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)Random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)Random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(8)==0)
		{
			// get him!
			MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		FaceGoodguy2(me,goodguy);

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickCorpse(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			i=(me->facing*32-8+Random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireExactBullet(me,x,y,FIXAMT*50,Cosine(i)*8,Sine(i)*8,-FIXAMT,0,50,i,BLT_ENERGY,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(16)==0)
			{
				// get him!
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(10)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)Random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)Random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(16)==0)
		{
			// get him!
			FaceGoodguy2(me,goodguy);
			MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(40)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			x=me->x+Cosine(me->facing*32)*4;
			y=me->y+Sine(me->facing*32)*4;
			f=me->facing*32-8+Random(17);
			FireExactBullet(me,x,y,me->z,Cosine(f)*8,Sine(f)*8,0,0,24-Random(4),f,BLT_FLAME2,me->friendly);
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,200);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly zip towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(4)==0)
			{
				// get him!
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}

			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(16)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=110;
		}
	}
}

void DoMove(Guy *me,byte move,int dx,int dy)
{
	me->seq=move;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=128;
	me->action=ACTION_BUSY;
	me->dx=dx;
	me->dy=dy;
}

void AI_Dancer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}
	switch(me->mind)
	{
		case 0:
		case 12:
			DoMove(me,ANIM_MOVE,-FIXAMT,0);
			break;
		case 1:
		case 2:
		case 4:
		case 8:
		case 10:
			DoMove(me,ANIM_ATTACK,0,0);
			break;
		case 3:
		case 11:
			DoMove(me,ANIM_MOVE,FIXAMT,0);
			break;
		case 5:
		case 9:
			DoMove(me,ANIM_MOVE,0,-FIXAMT);
			break;
		case 6:
		case 7:
			DoMove(me,ANIM_MOVE,0,FIXAMT);
			break;
	}
	me->mind++;
	if(me->mind==13)
		me->mind=0;
}

void AI_Log(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte tries;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
		return;
	
	if(me->hp%2==0)
		me->facing=11;
	else
		me->facing=0;
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x/FIXAMT;
			y=me->y/FIXAMT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;
	}
	
	if(me->mind==0)	// waiting to be ridden
	{
		// chill
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;
		me->mind2=0;
	}
	else	// being ridden
	{
		if (me->mind2==0)
		me->mind2 = 30;
		else if (me->mind2>1)
		me->mind2--;
	
		if(!editing && !player.cheated)
			profile.progress.raftDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);
		
		goodguy->x=me->x;
		me->dx=goodguy->dx*0.75;
		goodguy->y=(me->y)+12*FIXAMT;
		
		x=me->mapx;
		y=me->mapy;
		y++;
		if(x>=0 && x<map->width && y>=0 && y<map->height &&
						map->map[x+y*map->width].wall==0 &&
						!(GetItem(map->map[x+y*map->width].item)->flags&IF_SOLID) &&
						!(GetTerrain(world,map->map[x+y*map->width].floor)->flags&(TF_SOLID|TF_WATER|TF_LAVA)))
			{
				me->x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				me->y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				me->mind1=0;
				me->mind=0;
				goodguy->dx=0;
				goodguy->dy=0;
				me->dx=0;
				me->dy=0;
				goodguy->y=me->y+(TILE_HEIGHT)*FIXAMT;
				player.vehicle=0;
				return;

			}
		me->dy=(goodguy->dy+(((-0.1*me->mind2)+5.1)*FIXAMT));
		if (me->dy<0)
		me->dy=0;
		if(me->mind2==1)
		me->dx=goodguy->dx/2;
		if(me->mind1&2)
		if(me->mind1)
			me->dx=0;
		me->mind1=0;
	}
	WaterRipple(me->x/FIXAMT,me->y/FIXAMT,Random(32*20));
	me->frmAdvance=64;
}