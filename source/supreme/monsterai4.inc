void AI_Lazybones(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	// mind1=exact facing for shooting
	// mind=timer to wake up
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			x=me->x+Cosine(me->mind1)*16;
			y=me->y+Sine(me->mind1)*16;
			if(me->frm==2)
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,950);
			FireBullet(me,x,y,me->mind1,BLT_ENERGY,me->friendly);
			me->mind1+=8;
			me->facing=(me->mind1/32);
			if(me->frm==17)
			{
				me->reload=10;
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmAdvance=128;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==6)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmAdvance=128;
		}
		if(me->seq==ANIM_A2 && me->frm==6)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
		}
		return;	// can't do nothin' right now
	}

	if((me->ouch || RangeToTarget(me,goodguy)<256*FIXAMT) && me->reload==0)
	{
		MakeSound(SND_SKELALIVE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		me->mind1=Random(256);
		return;
	}
}

void AI_StareyBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
	}

	FaceGoodguy2(me,goodguy);

	if(me->mind>0)
	{
		me->mind++;
		if(me->mind==15)
			me->mind=0;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->mind==0)
			{
				MakeSound(SND_BATSTARE,me->x,me->y,SND_CUTOFF,500);
				me->mind=1;
			}
		}

		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT)
			{
				me->frmTimer=0;
				if(goodguy->x<me->x)
				{
					goodguy->dx-=(340*FIXAMT/128);
					goodguy->dy-=(340*FIXAMT/1024);
				}
				else
				{
					goodguy->dx+=(340*FIXAMT/128);
					goodguy->dy+=(340*FIXAMT/1024);
				}
				if(goodguy->y<me->y)
				{
					goodguy->dy-=(340*FIXAMT/128);
					goodguy->dx+=(340*FIXAMT/1024);
				}
				else
				{
					goodguy->dy+=(340*FIXAMT/128);
					goodguy->dx-=(340*FIXAMT/1024);
				}
				Clamp(&goodguy->dx,FIXAMT*7);
				Clamp(&goodguy->dy,FIXAMT*7);
			}
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<270*FIXAMT)
	{
		me->seq=ANIM_ATTACK;
		me->action=ACTION_BUSY;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Madman(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WACKOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WACKODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=10;
		}

		if(me->seq==ANIM_DIE && me->frm==5)
			me->frmAdvance=128;
		return;	// can't do nothin' right now
	}

	FaceGoodguy(me,goodguy);

	if(RangeToTarget(me,goodguy)<(96*FIXAMT) && Random(16)==0 && me->reload==0)
	{
		// stab him
		MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=255;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=1;
		return;
	}

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

void AI_PygmyAcct(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,ang;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=2 && me->frm<=5 && me->reload==0)
		{
			ang=(me->facing*32-40+(20*(me->frm-2)))&255;
			x=me->x+Cosine(ang)*16;
			y=me->y+Sine(ang)*16;
			FireBullet(me,x,y,ang,BLT_PAPER,me->friendly);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && Random(200)==0)
		{
			if(Random(4)==0)
			{
				me->seq=ANIM_A1;	// spin the spear
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->mind1=0;
				me->action=ACTION_BUSY;
			}
			else	// just change facing
			{
				me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
				me->mind1=0;
			}
		}
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<350*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				me->mind1=120;	// for 4 seconds minimum

				FaceGoodguy(me,goodguy);

				// do angry animation
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				MakeSound(SND_PYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		me->dx=0;
		me->dy=0;
	}
	else
	{
		FaceGoodguy(me,goodguy);

		if(Random(50)==0 && me->reload==0)
		{
			// fire at him
			MakeSound(SND_PYGMYCASE,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>380*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
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

void AI_Crazypants(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WACKOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else if(me->mind==3)
			MakeSound(SND_WACKODIE,me->x,me->y,SND_CUTOFF,1200);
		else
		{
			if(!me->friendly)
				player.enemiesSlain--;
			me->hp=monsType[me->aiType].hp;
			me->mind++;
			me->seq=ANIM_A2;
			me->frm=0;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			MakeSound(SND_CRAZYMAD,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
		}
	}

	if(me->mind==0)	// have yet to spot Bouapha
	{
		if(me->ouch || RangeToTarget(me,goodguy)<300*FIXAMT)
		{
			me->seq=ANIM_A2;
			me->frm=0;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->mind=1;
			me->action=ACTION_BUSY;
			MakeSound(SND_CRAZYMAD,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=2 && me->frm<=12)
		{
			me->dx=Cosine(me->facing*32)*5*me->mind;
			me->dy=Sine(me->facing*32)*5*me->mind;

			if(me->reload==0)
			{
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				if(me->AttackCheck(40,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*6,Sine(me->facing*32)*6,8,map,world);
					me->reload=6;
				}
			}

		}
		else if(me->seq==ANIM_A1 && me->frm>2)
		{
			if(me->frm&1)
			{
				me->dx+=Cosine(me->facing*32)*2;
				me->dy+=Sine(me->facing*32)*2;
				Clamp(&me->dx,3*me->mind);
				Clamp(&me->dy,3*me->mind);
				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
			me->facing=(me->facing+1)&7;
			// spin!
			if(me->reload==0)
			{
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				if(me->AttackCheck(40,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*6,Sine(me->facing*32)*6,12,map,world);
					me->reload=6;
				}
			}
		}
		else if(me->seq==ANIM_DIE)
		{
			// spin to death
			me->facing=(me->facing+1)&7;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind2==0)		// when mind2=0, singlemindedly drift towards Bouapha
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(12)==0)
		{
			// get him!
			MakeSound(SND_CRAZYDASH,me->x,me->y,SND_CUTOFF,1200);
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
		else if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(7)==0)
		{
			// get him!
			MakeSound(SND_CRAZYSPIN,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
		FaceGoodguy(me,goodguy);

		me->dx=Cosine(me->facing*32)*me->mind*3;
		me->dy=Sine(me->facing*32)*me->mind*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128+me->mind*32;
		}
		if(me->mind1>0 && RangeToTarget(me,goodguy)>(64*FIXAMT))
		{
			me->mind2=1;	// bumped a wall, so Randomy maneuver
			me->facing=(byte)Random(8);
			me->mind3=10;
		}
	}
	else if(me->mind2==1)	// random wandering
	{
		if(!(me->mind3--))	// time to get a new direction
		{
			me->mind2=0;	// get back on track
		}

		me->dx=Cosine(me->facing*32)*me->mind*3/2;
		me->dy=Sine(me->facing*32)*me->mind*3/2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128+me->mind*32;
		}
	}

	me->mind1=0;
}

void AI_Yerfdog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->z>FIXAMT*8 && me->facing!=1 && abs(me->dz)<FIXAMT*2)
	{
		me->mind1=2-me->facing;
		me->facing=1;
	}
	else if(me->z<=FIXAMT*8)
	{
		me->facing=me->mind1;
	}

	me->dz+=FIXAMT*3/2;
	if(me->z==0)
	{
		me->dz=FIXAMT*3+Random(FIXAMT*7);
	}
}

void AI_EliteSeal(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SEALOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SEALDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<32 && me->frm==3)
		{
			f=me->facing*32;
			x=me->x+Cosine(f)*16;
			y=me->y+Sine(f)*16;
			if(me->aiType==MONS_ATOMICSEAL)
			FireBullet(me,x,y,me->facing,BLT_ROCKET,me->friendly);
			else
			FireBullet(me,x,y,me->facing,BLT_MISSILE,me->friendly);
			MakeSound(SND_MISSILELAUNCH,x,y,SND_CUTOFF,1050);
			me->reload=10;
		}
		if(me->seq==ANIM_DIE && me->frm>=4)
		{
			for(f=0;f<8;f++)
				SpecialSnow(me->x+((-32+Random(65))<<FIXSHIFT),
							me->y+((-32+Random(65))<<FIXSHIFT));
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(Random(64)==0 && me->reload==0 && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				// get him!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
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
}

void AI_Cone(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		/*
		if(me->hp>0)
			MakeSound(SND_SEALOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SEALDIE,me->x,me->y,SND_CUTOFF,1200);
			*/
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(35,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*2,Sine(me->facing*32)*2,3,map,world);
				me->reload=6;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==6 && me->reload==0)
		{
			f=me->facing*32;
			FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(f)*10,Sine(f)*10,0,0,20,f,BLT_BIGSHELL,me->friendly);
			MakeSound(SND_MUSHMISSILE,x,y,SND_CUTOFF,1050);
			me->reload=4;
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;

		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, don't move
	{
		if(me->ouch)
			me->mind=1;
		return;
	}
	else
	{
		if(RangeToTarget(me,goodguy)<48*FIXAMT && me->reload==0)
		{
			// get him!
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
		else if(RangeToTarget(me,goodguy)<512*FIXAMT && me->reload==0 &&
			Random(30)==0)
		{
			// get him!
			me->seq=ANIM_A1;
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
}

void AI_PygmyDiver(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,ang;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GLUP,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_DIVERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->seq!=ANIM_DIE)
	{
		me->dz=0;
		if(me->z<FIXAMT*10)
			me->z+=FIXAMT/4;
		else me->z=FIXAMT*10;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0)
		{
			ang=((me->facing+1)&7)*32;
			x=me->x+Cosine(ang)*16;
			y=me->y+Sine(ang)*16;

			FireExactBullet(me,x,y,FIXAMT*10,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,FIXAMT,0,90,me->facing,BLT_HARPOON,me->friendly);
			FireExactBullet(me,x,y,FIXAMT*10,Cosine((me->facing*32+16)&255)*12,Sine((me->facing*32+16)&255)*12,FIXAMT,0,90,me->facing,BLT_HARPOON,me->friendly);
			FireExactBullet(me,x,y,FIXAMT*10,Cosine((me->facing*32+256-16)&255)*12,Sine((me->facing*32+256-16)&255)*12,FIXAMT,0,90,me->facing,BLT_HARPOON,me->friendly);

			me->reload=2;
		}
		if(me->seq==ANIM_A1 && me->frm==2)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=64;
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && Random(200)==0)
		{
			me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
			me->mind1=0;
		}
		if(RangeToTarget(me,goodguy)<350*FIXAMT || me->ouch>0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			me->mind1=120;	// for 4 seconds minimum

			FaceGoodguy(me,goodguy);

			// do to-swim animation
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			MakeSound(SND_DIVERANGRY,me->x,me->y,SND_CUTOFF,1200);
		}

		me->dx=0;
		me->dy=0;
	}
	else
	{
		FaceGoodguy(me,goodguy);

		if(Random(30)==0 && me->reload==0)
		{
			// fire at him
			MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>380*FIXAMT)
			{
				me->mind=0;	// get bored again
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE || (me->seq==ANIM_MOVE && me->frm==12))
		{
			if((me->seq==ANIM_A1 && me->frm==2) || me->seq!=ANIM_A1)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
	}
}

void AI_Puffyfish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,ang;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			if(me->mind<4)
			{
				me->mind++;
				me->mind2=60;
				if(me->mind==4)
				{
					me->hp=1;
					me->GetShot(me,me,0,0,1,map,world);
				}
			}
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==3 && me->reload==0)
		{
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
			me->reload=10;
		}
		if(me->seq==ANIM_DIE && me->frm>=3)
		{
			for(x=0;x<16;x++)
			{
				ang=Random(256);
				FireBullet(me,me->x,me->y,ang,BLT_SPINE,me->friendly);
			}
		}
		return;	// can't do nothin' right now
	}

	me->mind2--;
	if(me->mind2==0)
	{
		if(me->mind>0)
			me->mind--;

		me->mind2=60;
	}

	switch(me->mind)
	{
		case 0:
			if(me->seq!=ANIM_MOVE || me->frm==8)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			break;
		case 1:
		case 2:
		case 3:
			if(me->seq!=ANIM_A1+me->mind-1 || me->frm==8)
			{
				me->seq=ANIM_A1+me->mind-1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			break;
	}


	if(RangeToTarget(me,goodguy)<FIXAMT*128 && me->reload==0)
	{
		if(me->mind<4)
		{
			me->mind++;
			me->mind2=60;
			if(me->mind==4)
			{
				me->hp=1;
				me->GetShot(me,me,0,0,1,map,world);
				return;
			}
		}
		me->reload=10;
	}

	if(me->aiType==MONS_PUFFYFISH2)
		me->mind1-=2;
	else
		me->mind1+=2;
	me->facing=(me->mind1/32)&7;

	me->dx=Cosine(me->mind1)*2;
	me->dy=Sine(me->mind1)*2;
}

void AI_Yeti2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_YETIOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_YETIDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;	// can only die forward
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->frmTimer<32)
		{
			x=me->x+Cosine(me->facing*32)*72;
			y=me->y+Sine(me->facing*32)*72;
			x+=Cosine(((me->facing+2)*32)&255)*32;
			y+=Sine(((me->facing+2)*32)&255)*32;
			g=AddBaby(x,y,FIXAMT*10,MONS_SNOWBALL+Random(2),me);	// Randomy pick which type of snowball, normal or reverse
			if(g && !g->CanWalk(g,g->x,g->y,map,world))
				RemoveGuy(g);
			else if(g)
			{
				g->facing=me->facing;
			}
			me->reload=10;
		}
		if(me->seq==ANIM_A2 && me->frm>3)
		{
			ShakeScreen(3);
			// snowballs fall all over
			x=me->x+((320-Random(641))<<FIXSHIFT);
			y=me->y+((240-Random(481))<<FIXSHIFT);
			if(Random(3)==0)
				FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_BIGSNOW,me->friendly);
			else
				FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_SNOWBALL,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if((goodguy && RangeToTarget(me,goodguy)<(300*FIXAMT)) || (me->ouch))
		{
			me->mind=1;	// start into action
			me->mind1=0;

			me->seq=ANIM_A3;	// wave hello first
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->facing=2;
			me->action=ACTION_BUSY;

			MakeSound(SND_OLAFHI,me->x,me->y,SND_CUTOFF,1200);
		}
	}
	else
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			// rotate
			me->facing=(me->facing-1+Random(3))&7;
			me->mind1=Random(30)+1;
		}

		if((!me->reload) && goodguy && Random(20)==0)
		{
			// select an attack method
			x=Random(16);

			if(x<12) // 75% chance of primary weapon
			{
				// Olaf rolls snowballs
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				FaceGoodguy2(me,goodguy);
				me->action=ACTION_BUSY;
			}
			else	// 50% chance of yodel
			{
				MakeSound(SND_YETIYODEL,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->facing=2;
				me->action=ACTION_BUSY;
			}
			me->dx=0;
			me->dy=0;
			return;
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

void AI_Spikey(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_VAMPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_VAMPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=2 && (me->frm<=6) && me->aiType!=MONS_VAMPLORD)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->aiType==MONS_DRACULITE)
			{FireExactBullet(me,me->x,me->y,me->z+24,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,0,0,16,me->facing*32+Random(8)-4,BLT_SPORE,me->friendly);}
			else
			FireBullet(me,x,y,(me->facing*32-24+Random(49))&255,BLT_SPINE,me->friendly);
			me->reload=5;
		}
		if(me->aiType==MONS_VAMPLORD && me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && RangeToTarget(me,goodguy)<160*FIXAMT)
		{
			me->reload=2;
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			g=AddBaby(x,y,FIXAMT*20,MONS_BAT,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				MakeSound(SND_BATDIVE,g->x,g->y,SND_CUTOFF,1200);
				g->facing= me->facing;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->action=ACTION_BUSY;
				g->dx=Cosine(g->facing*32)*12-FIXAMT*2+Random(FIXAMT*4);
				g->dy=Sine(g->facing*32)*12;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly drift towards Bouapha
	{
		if(player.garlic && goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		{
			MakeSound(SND_VAMPFEAR,me->x,me->y,SND_CUTOFF,1200);
			me->mind=2;	// enter fear mode
			return;
		}

		if(goodguy)
		{
			if((RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(8)==0 && me->aiType!=MONS_VAMPLORD)||(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0 && me->aiType==MONS_VAMPLORD))
			{
				// get him!
				MakeSound(SND_VAMPATTK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=320;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*7;
			me->dy=Sine(me->facing*32)*7;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
			}
			if(me->mind1>0 && RangeToTarget(me,goodguy)>(64*FIXAMT))
			{
				me->mind=1;	// bumped a wall, so Randomy maneuver
				me->facing=(byte)Random(8);
				me->mind2=10;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->facing=(byte)Random(8);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind2--))	// time to get a new direction
		{
			me->mind=0;	// get back on track
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
	else
	{
		if(player.garlic && goodguy)
		{
			// run from the garlic
			FaceGoodguy(me,goodguy);
			me->facing=(me->facing+4)&7;	// face opposite direction
			me->dx=Cosine(me->facing*32)*7;
			me->dy=Sine(me->facing*32)*7;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
			}
		}
		else
		{
			me->mind=0;
		}
		return;
	}

	me->mind1=0;
}

void AI_Bombot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
			if (me->aiType==MONS_ATOMBOT)
			{
				x=me->x+Cosine(me->facing*32)*20;
				y=me->y+Sine(me->facing*32)*20;
				FireBullet(me,x,y,me->facing*32-16+Random(33),BLT_BOMB_HOMING,me->friendly);
				me->reload=2;
			}
			else
			{
				x=me->x+Cosine(((me->facing+2)&7)*32)*24;
				y=me->y+Sine(((me->facing+2)&7)*32)*20;
				FireBullet(me,x,y,me->facing*32-16+Random(33),BLT_BOMB,me->friendly);
				x=me->x+Cosine(((me->facing-2)&7)*32)*24;
				y=me->y+Sine(((me->facing-2)&7)*32)*20;
				FireBullet(me,x,y,me->facing*32-16+Random(33),BLT_BOMB,me->friendly);
				me->reload=2;
			}
		}
		if(me->seq==ANIM_DIE && me->frm>3 && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly roll towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(72)==0)
			{
				// shoot at him
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
			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(48)==0)
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
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(256)==0)
			{
				// shoot at him
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
			if((goodguy) && Random(3)==0)
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

void AI_FollowBunny(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
		return;

	if(me->ouch==4)
	{
		MakeSound(SND_BUNNYDIE,me->x,me->y,SND_CUTOFF,800);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			for(x=0;x<8;x++)
				SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT),
							me->y+((-16+Random(33))<<FIXSHIFT));
		}
		return;
	}

	if(me->mind==0)	// not activated yet
	{
		if(RangeToTarget(me,goodguy)<72*FIXAMT)
		{
			me->mind=1;
			MakeNormalSound(SND_FOLLOWHI);
		}
	}
	else if(me->mind==1)	// following Bouapha
	{
		Dampen(&me->dx,FIXAMT/8);
		Dampen(&me->dy,FIXAMT/8);

		if(me->x<goodguy->x)
			me->dx+=FIXAMT;
		else
			me->dx-=FIXAMT;
		if(me->y<goodguy->y)
			me->dy+=FIXAMT;
		else
			me->dy-=FIXAMT;

		me->dx+=-FIXAMT/2+Random(FIXAMT);
		me->dy+=-FIXAMT/2+Random(FIXAMT);

		if(RangeToTarget(me,goodguy)<48*FIXAMT)
		{
			me->mind=2;
			me->facing=Random(8);
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->mind1=Random(20)+10;
		}
	}
	else // not following, got too close
	{
		me->mind1--;
		if(!me->mind1)
			me->mind=1;
	}

	Clamp(&me->dx,FIXAMT*4);
	Clamp(&me->dy,FIXAMT*4);

	if(me->dx>FIXAMT)
	{
		if(me->dy>FIXAMT)
			me->facing=1;
		else if(me->dy<-FIXAMT)
			me->facing=7;
		else
			me->facing=0;
	}
	else if(me->dx<-FIXAMT)
	{
		if(me->dy>FIXAMT)
			me->facing=3;
		else if(me->dy<-FIXAMT)
			me->facing=5;
		else
			me->facing=4;
	}
	else
	{
		if(me->dy>FIXAMT)
			me->facing=2;
		else if(me->dy<-FIXAMT)
			me->facing=6;
	}

	if(me->mind!=0)
	{
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Autozoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not activated yet
	{
		if(RangeToTarget(me,goodguy)<512*FIXAMT || me->ouch)
		{
			me->mind=1;
			me->mind2=3;
			MakeNormalSound(SND_CARSTART);
		}
	}
	else if(me->mind==1)	// active, zooming after Bouapha
	{
		if(me->mind2)
			me->mind2--;
		else
		{
			me->mind2=3;
			FaceGoodguy2(me,goodguy);
		}

		Dampen(&me->dx,FIXAMT/8);
		Dampen(&me->dy,FIXAMT/8);

		me->dx+=Cosine(me->facing*32)/2;
		me->dy+=Sine(me->facing*32)/2;

		if(RangeToTarget(me,goodguy)<64*FIXAMT)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(35,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,10,map,world);
				me->dx=-me->dx;
				me->dy=-me->dy;
				me->mind1=0;
				MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		if(me->mind1)	// hit a wall
		{

			x=(abs(me->dx)+abs(me->dy))*20/(FIXAMT*16);
			if(x<1)
				x=1;
			if(me->mind1&1)
				me->dx=-me->dx/2;
			if(me->mind1&2)
				me->dy=-me->dy/2;
			me->mind1=0;
			me->GetShot(me,me,0,0,x,map,world);
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,200);
			me->facing=Random(8);
		}

		x=(me->x+me->dx)/(TILE_WIDTH*FIXAMT);
		y=(me->y+me->dy)/(TILE_HEIGHT*FIXAMT);
		if(x>=0 && y>=0 && x<map->width && y<map->height &&
			!map->map[x+y*map->width].wall &&
			(!(GetTerrain(world,map->map[x+y*map->width].floor)->flags&TF_MINECART)))
		{
			// you would've gone offroad
			me->dx=0;
			me->dy=0;
			me->facing=Random(8);
		}

	}

	Clamp(&me->dx,FIXAMT*10);
	Clamp(&me->dy,FIXAMT*10);

	if(me->mind!=0)
	{
		if(me->seq==ANIM_IDLE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		if(me->seq==ANIM_MOVE)
			me->frmAdvance=(abs(me->dx)+abs(me->dy))*512/(FIXAMT*20);
	}
}

void AI_Chest(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->seq==ANIM_DIE && me->frm==4)
	{
		x=me->x-32*FIXAMT+Random(64*FIXAMT);
		y=me->y-24*FIXAMT+Random(48*FIXAMT);
		ExplodeParticles2(PART_DIRT,x,y,FIXAMT*30,20,5);
	}
	// Just sit there.
}

void AI_Jellofish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		MakeSound(SND_JELLOOUCH,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(RangeToTarget(me,goodguy)<320*FIXAMT || me->ouch>0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			if(me->ouch==0)
				me->mind1=60;	// for 2 seconds minimum
			else
				me->mind1=120;	// 4 seconds, because they HATE getting shot
			FaceGoodguy2(me,goodguy);
		}
		me->dx=0;
		me->dy=0;
	}
	else if(me->mind==1)
	{
		FaceGoodguy2(me,goodguy);

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;

		if(RangeToTarget(me,goodguy)<48*FIXAMT)
		{
			me->mind=2;
			me->facing=Random(8);
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->mind1=Random(20)+10;
		}
	}
	else if(me->mind==2)
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
			{
				me->mind=1;
				me->mind1=20;	// stay on trail a little longer
			}
		}
	}

	if(me->mind && RangeToTarget(me,goodguy)<(140*FIXAMT) && Random(5)==0 && me->reload==0)
	{
		// zap him!
		me->reload=8;
		MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,1200);
		LightningBolt(me->x,me->y-FIXAMT*30,goodguy->x,goodguy->y-FIXAMT*30);
		goodguy->GetShot(me,goodguy,0,0,2,map,world);
		return;
	}

	me->frmAdvance=128;
}

void AI_KingCone(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte spd;
	int i,x,y;
	Guy *g;

	if(me->reload)
		me->reload--;
	if(me->mind3)
		me->mind3--;

	if(me->ouch==4)
	{
		if(me->hp<=0)
			MakeSound(SND_KINGDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			me->reload=5;
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
			spd=Random(8)+1;
			for(i=0;i<8;i++)
				FireExactBullet(me,me->x,me->y,FIXAMT*128,Cosine(i*32)*spd,Sine(i*32)*spd,FIXAMT*4,0,120,0,BLT_GRENADE,me->friendly);
		}
		if(me->seq==ANIM_MOVE && me->frm==6 && me->reload==0)
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
			me->reload=5;
			ShakeScreen(10);
			spd=Random(24)+2;
			for(i=0;i<spd;i++)
			{
				x=me->x+(-128+Random(257))*FIXAMT;
				y=me->y+(-128+Random(257))*FIXAMT;
				if(x<me->x-FIXAMT*64 || x>me->x+FIXAMT*64 || y<me->y-FIXAMT*64 || y>me->y+FIXAMT*64)
				{
					g=AddBaby(x,y,FIXAMT*10,MONS_CONE,me);
					if(g && !g->CanWalk(g,g->x,g->y,map,world))
						g->type=MONS_NONE;
				}
			}

		}
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			me->reload=Random(5);
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		me->facing=0;
		return;	// can't do nothin' right now
	}

	if(!(me->mind&7))
		me->facing=Random(5);	// look in a different direction

	if(RangeToTarget(me,goodguy)<FIXAMT*800)
		me->mind2=1;

	if(me->mind3==0 && me->ouch && me->hp>30 && RangeToTarget(me,goodguy)<FIXAMT*600)
	{
		switch(Random(3))
		{
			case 0:
				MakeSound(SND_KINGCONES,me->x,me->y,SND_CUTOFF,1500);
				break;
			case 1:
				MakeSound(SND_KINGTICKLE,me->x,me->y,SND_CUTOFF,1500);
				break;
			case 2:
				MakeSound(SND_KINGSCOFF,me->x,me->y,SND_CUTOFF,1500);
				break;
		}
		me->mind3=60+Random(180);
	}

	if(me->mind2)
	{
		if(me->mind)
		{
			me->mind--;
			if(me->mind==0)
			{
				me->mind=120;
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->action=ACTION_BUSY;
				me->frmAdvance=128;
				me->facing=0;
			}
			if(me->mind==60 && me->mind2)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->action=ACTION_BUSY;
				me->frmAdvance=128;
				me->facing=0;
			}
		}
		else
			me->mind=120;
	}

	me->dx=0;
	me->dy=0;
}

void AI_Mailboxer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
			me->reload=5;
			x=Random(5);
			if(x==0)	// do another quick jab
			{
				me->frm=0;
				me->frmTimer=0;
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
			}
			else if(x==1)	// do a power punch
			{
				me->dx=Cosine(me->facing*32);
				me->dy=Sine(me->facing*32);
				me->frm=0;
				me->frmTimer=0;
				me->seq=ANIM_A1;
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==6 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,12,map,world);
			me->reload=5;
			me->mind1=1;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(RangeToTarget(me,goodguy)<(64*FIXAMT) && Random(8)==0)
		{
			// get him!
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
		if(Random(128)==0)
		{
			me->mind=1;		// occasionally wander
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(RangeToTarget(me,goodguy)<(64*FIXAMT) && Random(8)==0)
		{
			// get him!
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

		if(!(me->mind1--))	// time to get a new direction
		{
			if(Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(20)+1;
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

void AI_Piranha(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_FISHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm<4)
			{
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				x=me->x+Cosine(me->facing*32)*10;
				y=me->y+Sine(me->facing*32)*10;
				if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					MakeSound(SND_SPD1GNAW,me->x,me->y,SND_CUTOFF,1000);
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*2,Sine(me->facing*32)*2,2,map,world);
					me->frm=4;	// bounce back
					me->frmTimer=0;
				}
			}
			else
			{
				me->dx=-Cosine(me->facing*32)*6;
				me->dy=-Sine(me->facing*32)*6;
			}
			me->reload=10;
		}
		if(me->seq==ANIM_DIE)
		{
			me->z+=FIXAMT*2-me->dz;
			me->dz=0;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==2)		// when mind=2, pursue relentlessly
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(96*FIXAMT))
			{
				me->mind=1;	// start circling
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
			}
		}
		else
		{
			me->mind=0;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==0)	// random wandering
	{
		me->mind=2;
	}
	else if(me->mind==1)	// circling
	{
		FaceGoodguy2(me,goodguy);

		// move sideways
		me->dx=Cosine((me->facing*32+64)&255)*6;
		me->dy=Sine((me->facing*32+64)&255)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}

		if(goodguy && RangeToTarget(me,goodguy)>96*FIXAMT)
		{
			me->mind=2;	// chase him down
		}

		if((!me->reload) && Random(16)==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
		}
	}
}

void AI_Yugo(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	int diff,dir;
	static byte noiseLoop=0;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
	{
		if(me->mind>1)
		{
			player.vehicle=0;
			me->aiType=MONS_NONE;
			x=me->x/FIXAMT;
			y=me->y/FIXAMT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;
	}

	if(me->mind3)
		me->mind3--;

	if(me->mind>1)
	{
		me->ouch=goodguy->ouch;
		me->facing=me->mind2/32;	// mind2 holds the 'true' visual facing 0-255

		x=goodguy->facing*32;
		// mind2 is the visual facing
		// so rotate mind2 towards the player's actual facing to steer
		if(x>me->mind2)
		{
			diff=x-me->mind2;
			if(diff>128)
			{
				dir=-1;
				diff=256-diff;
			}
			else
				dir=1;
		}
		else if(x<me->mind2)
		{
			diff=me->mind2-x;
			if(diff>128)
			{
				dir=1;
				diff=256-diff;
			}
			else
				dir=-1;
		}
		else
		{
			diff=0;
			dir=0;
		}

		if(diff>8)
			diff=8;

		me->mind2=(me->mind2+diff*dir)&255;
	}

	if(me->mind==0)	// too recently driven, don't allow another drive until player steps away
	{
		if(RangeToTarget(me,goodguy)>32*FIXAMT)
			me->mind=1;	// okay, that's far enough
	}
	else if(me->mind==1)	// waiting to be driven
	{
		if(RangeToTarget(me,goodguy)<32*FIXAMT && player.vehicle==0)
		{
			if(!(GetTerrain(world,map->GetTile(me->mapx,me->mapy)->floor)->flags&TF_MINECART))
			{
				// somehow the car ended up on non-minecart ground.. ridiculous!
				for(x=me->mapx-1;x<=me->mapx+1;x++)
					for(y=me->mapy-1;y<=me->mapy+1;y++)
					{
						if(x>=0 && y>=0 && x<map->width && y<map->height &&
							(GetTerrain(world,map->map[x+y*map->width].floor)->flags&TF_MINECART) &&
							(map->map[x+y*map->width].wall==0) &&
							(!(GetItem(map->map[x+y*map->width].item)->flags&(IF_SOLID|IF_BULLETPROOF)))
							)
						{
							// found a spot that IS minecart neighboring
							me->x=(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
							me->y=(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
							me->mapx=x;
							me->mapy=y;
							x=me->mapx+2;
							y=me->mapy+2;
						}
					}
			}
			MakeSound(SND_CARSTART,goodguy->x,goodguy->y,SND_CUTOFF,1200);
			noiseLoop=0;
			me->mind=2;
			player.vehicle=VE_YUGO;
			goodguy->parent=me;
			goodguy->x=me->x;
			goodguy->y=me->y;
			me->dx=0;
			me->dy=0;
		}
	}
	else	// being driven
	{
		if(!editing && !player.cheated)
			profile.progress.driveDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);

		noiseLoop++;
		if(noiseLoop>=5)
		{
			noiseLoop=0;
			MakeSound(SND_CARGO,goodguy->x,goodguy->y,SND_CUTOFF,1200);
		}

		x=(me->x+me->dx)/(TILE_WIDTH*FIXAMT);
		y=(me->y+me->dy)/(TILE_HEIGHT*FIXAMT);
		if(x>=0 && y>=0 && x<map->width && y<map->height &&
			!map->map[x+y*map->width].wall &&
			(!(GetTerrain(world,map->map[x+y*map->width].floor)->flags&TF_MINECART)))
		{
			// you would've gotten off of driveable terrain
			me->mind3=0;
			me->x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
			me->y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
			me->mind=0;
			me->dx=0;
			me->dy=0;
			goodguy->dx=0;
			goodguy->dy=0;
			player.vehicle=0;
			goodguy->parent=NULL;
			me->frm=0;
			me->seq=ANIM_IDLE;
			me->frmTimer=0;
			me->frmAdvance=128;
			return;
		}

		x=(abs(me->dx)+abs(me->dy))*20/(FIXAMT*16);
		if(x<1)
			x=1;
		y=0;
		if(me->mind1&1)	// bounced a horizontal wall
		{
			me->dx=-me->dx/2;
			goodguy->GetShot(me,goodguy,0,0,x,map,world);	// bounce, ouch!  More damage at higher speed
			me->ouch=4;	// light the car up red, since you can't see bouapha
			y=1;
		}
		if(me->mind1&2)	// bounced a vertical wall
		{
			me->dy=-me->dy/2;
			if(!y)	// don't get hurt twice for one bounce
				goodguy->GetShot(me,goodguy,0,0,x,map,world);	// bounce, ouch!  More damage at higher speed
			me->ouch=4;	// light the car up red, since you can't see bouapha
		}
		me->mind1=0;

		if(goodguy->hp==0)
		{
			player.vehicle=0;
			me->aiType=MONS_NONE;
			x=me->x/FIXAMT;
			y=me->y/FIXAMT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			return;
		}

		if(!(GetTerrain(world,map->map[me->mapx+me->mapy*map->width].floor)->flags&TF_ICE) && me->z==0)
		{
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);

			if(me->mind3>60)	// set max speed higher if in turbo mode
			{
				x=FIXAMT*14;
				// and spew particles for power!!
				SpurtParticles(PART_HAMMER,0,me->x,me->y,FIXAMT*4,((me->mind2)+128)&255,6);
				SpurtParticles(PART_HAMMER,1,me->x,me->y,FIXAMT*4,((me->mind2)+128)&255,6);
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,200);
			}
			else
			{
				x=FIXAMT*9;
			}

			if((me->dx+Cosine(me->mind2)/2<x &&
				me->dx+Cosine(me->mind2)/2>-x) ||
				(me->dx<0 && Cosine(me->mind2)>0) ||
				(me->dx>0 && Cosine(me->mind2)<0))
				me->dx+=Cosine(me->mind2)/2;
			if((me->dy+Sine(me->mind2)/2<x &&
				me->dy+Sine(me->mind2)/2>-x) ||
				(me->dy<0 && Sine(me->mind2)>0) ||
				(me->dy>0 && Sine(me->mind2)<0))
				me->dy+=Sine(me->mind2)/2;
		}
		else if(me->z==0)
		{
			if(me->mind3>60)	// you can only accelerate on ice if in turbo mode
			{
				x=FIXAMT*14;
				// and spew particles for power!!
				SpurtParticles(PART_HAMMER,0,me->x,me->y,FIXAMT*4,((me->mind2)+128)&255,6);
				SpurtParticles(PART_HAMMER,1,me->x,me->y,FIXAMT*4,((me->mind2)+128)&255,6);
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,200);
				if((me->dx+Cosine(me->mind2)/32<x &&
					me->dx+Cosine(me->mind2)/32>-x))
					me->dx+=Cosine(me->mind2)/32;
				if((me->dy+Sine(me->mind2)/32<x &&
					me->dy+Sine(me->mind2)/32>-x))
					me->dy+=Sine(me->mind2)/32;
			}
		}
		if((GetTerrain(world,map->map[me->mapx+me->mapy*map->width].floor)->flags&TF_MUD) && me->z==0)
		{
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			if(me->dx!=0 || me->dy!=0)
			{
				SpurtParticles(PART_DIRT,0,me->x,me->y,FIXAMT*4,((me->mind2)+128)&255,3);
				SpurtParticles(PART_DIRT,1,me->x,me->y,FIXAMT*4,((me->mind2)+128)&255,3);
			}
		}
		if((GetTerrain(world,map->map[me->mapx+me->mapy*map->width].floor)->flags&TF_RUBBER) && me->z==0)
		{
			me->dz=FIXAMT*16;
			MakeSound(SND_BOING+Random(3),me->x,me->y,SND_CUTOFF,2);
		}
		if((GetTerrain(world,map->map[me->mapx+me->mapy*map->width].floor)->flags&TF_WATER) && me->z==0)
		{
			goodguy->dx=0;
			goodguy->dy=0;
			goodguy->z=0;
			goodguy->dz=0;
			me->aiType=MONS_NONE;
			x=me->x/FIXAMT;
			y=me->y/FIXAMT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			// drown player:
			if(player.weapon==WPN_PWRARMOR)
				player.weapon=0;
			goodguy->facing=(4+goodguy->facing)&7;
			goodguy->hp=0;
			SetPlayerHP(goodguy->hp);
			goodguy->seq=ANIM_A3;
			goodguy->frm=0;
			goodguy->frmAdvance=255;
			goodguy->frmTimer=0;
			goodguy->action=ACTION_BUSY;
			if(player.playAs==PLAY_LUNACHIK)
				MakeSound(SND_LUNADROWN,x,y,SND_CUTOFF|SND_ONE,65536);
			else if(player.playAs==PLAY_MYSTIC)
				MakeSound(SND_DROWNKM,x,y,SND_CUTOFF|SND_ONE,65536);
			else
				MakeSound(SND_DROWN,x,y,SND_CUTOFF|SND_ONE,65536);
			if(player.shield)
				CompleteGoal(24);	// fell in water while shielded
			return;
		}


		// confine the player to the car
		goodguy->x=me->x+me->dx;
		goodguy->y=me->y+me->dy;
		goodguy->dz=me->dz;
		goodguy->z=me->z;

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		if(me->seq==ANIM_MOVE)
			me->frmAdvance=(abs(me->dx)+abs(me->dy))*512/(FIXAMT*18);
	}
}

void AI_Punkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->reload==0)	// hits on this frame
	{
		FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,Cosine(me->facing*32)*7,Sine(me->facing*32)*7,8,map,world,me->friendly);
		me->reload=4;
	}

	if(me->mind==0)	// not currently aware of goodguy
	{

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}

		if(me->mind1)
			me->mind1--;
		else if(RangeToTarget(me,goodguy)<512*FIXAMT || me->ouch>0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start charging
			me->mind1=15+Random(15);
			MakeSound(SND_OYOYOY,me->x,me->y,SND_CUTOFF,900);
			FaceGoodguy(me,goodguy);
		}

		me->dx=0;
		me->dy=0;
	}
	else
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			me->mind=0;	// get bored again
			me->mind1=20+Random(40);
			return;
		}
		me->dx=Cosine(me->facing*32)*8;
		me->dy=Sine(me->facing*32)*8;

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
	}
}

void AI_SeaCuke(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_CUKEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_CUKEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<8 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(me,x,y,me->facing,BLT_DIRTSPIKE,me->friendly);
			me->reload=1;
		}
		if(me->seq==ANIM_A1 && me->frm>2 && me->reload==0)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,8,map,world);
			me->reload=3;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		FaceGoodguy2(me,goodguy);

		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(30)==0)
		{
			// get him!
			if(Random(2)==0)
				MakeSound(SND_RUFFIAN,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_HAVEATYOU,me->x,me->y,SND_CUTOFF,1200);
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
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0)
		{
			// dance at him
			FaceGoodguy(me,goodguy);
			MakeSound(SND_CUKEDANCE,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			me->reload=0;
			return;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE || me->frm==12)
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
	else if(me->mind==1)	// random wandering
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(30)==0)
		{
			// get him!
			FaceGoodguy(me,goodguy);
					if(Random(2)==0)
				MakeSound(SND_RUFFIAN,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_HAVEATYOU,me->x,me->y,SND_CUTOFF,1200);
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
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0)
		{
			// dance at him
			FaceGoodguy(me,goodguy);
			MakeSound(SND_CUKEDANCE,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			me->reload=0;
			return;
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if(Random(3)==0)
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

void AI_Sneaky2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SHARKDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SHARKOUCH,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=8 && me->frm<16)
		{
			me->frmAdvance=128;
			FireBulletZ(me,me->x,me->y,FIXAMT*12,((7-(me->frm-8))*32-16+(byte)Random(33))&255,BLT_SHARK,me->friendly);
			FireBulletZ(me,me->x,me->y,FIXAMT*12,((7-(me->frm-8))*32-16+(byte)Random(33))&255,BLT_SHARK,me->friendly);
		}
		else if(me->seq==ANIM_DIE)
		{
			FireBulletZ(me,me->x,me->y,FIXAMT*12,Random(256),BLT_SHARK,me->friendly);
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;
	}

	if(me->seq==ANIM_IDLE)
	{
		me->frmAdvance=32;
		me->dx=0;
		me->dy=0;
		if(me->mind==0)
			x=128*FIXAMT;	// detects at half-range until he's awakened once already
		else
			x=256*FIXAMT;
		if(RangeToTarget(me,goodguy)<x || me->ouch)
		{
			me->mind=1;
			me->mind1=40+(byte)Random(60);
			MakeSound(SND_SHARKATTACK,me->x,me->y,SND_CUTOFF,1200);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			return;
		}
	}
}

void AI_Underzoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*50;
			y=me->y+Sine(me->facing*32)*50;
			FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
			FireBullet(me,x,y,(me->facing*32+4)&255,BLT_ENERGY,me->friendly);
			FireBullet(me,x,y,(me->facing*32-4)&255,BLT_ENERGY,me->friendly);
			me->reload=15;
			me->mind=0;
		}
		if(me->seq==ANIM_DIE)
		{
			me->z+=FIXAMT*2-me->dz;
			me->dz=0;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
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

void AI_Starfish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,a;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		//if(me->hp>0)
		//	MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		//else
			//MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>1)
		{
			if(me->z<FIXAMT*10)
				me->z+=FIXAMT*2;
			me->dz=0;
			if(me->frm==9)
			{
				// if Bouapha is still near, keep blasting!
				if(RangeToTarget(me,goodguy)<256*FIXAMT)
				{
					me->frm=2;
				}
			}
			// shoot
			if(me->frm<=9 && me->reload==0)
			{
				me->reload=2;
				me->mind3+=4;	// rotate the angle of fire
				for(i=0;i<5;i++)
				{
					a=(me->mind3+(i*256)/5)&255;
					x=me->x+Cosine(a)*16;
					y=me->y+Sine(a)*16;
					FireBullet(me,x,y,a,BLT_ENERGY,me->friendly);
				}
				MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<256*FIXAMT)
	{
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
	if(me->mind==0)	// standing around
	{
		if(me->mind1==0 || me->ouch)
		{
			me->mind=1;
			if(!me->ouch)
				me->facing=Random(8);
			else
				FaceGoodguy(me,goodguy);
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->mind1=Random(50)+15;
			me->dx=0;
			me->dy=0;
		}
	}
	else	// rolling to a good home
	{
		if(me->frm==2)
		{
			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
		}
		if(me->frm==8 && me->frmTimer>127)
		{
			me->frm=2;
			me->frmTimer=0;
		}
		if(me->mind1==0)
		{
			me->mind=0;
			me->mind1=10+Random(50);
			me->frm=10;
			me->frmTimer=0;
			me->dx=0;
			me->dy=0;
		}
		if(me->ouch)	// if you get hit, head for the source
		{
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			me->mind1+=15;
		}
	}
}

void AI_WetSeal(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SEALOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SEALDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<32)
		{
			f=me->facing*32;
			switch(me->frm)
			{
				case 3:
					f-=20;
					break;
				case 5:
					f-=10;
					break;
				case 7:
					f-=4;
					break;
				case 9:
					f+=4;
					break;
				case 11:
					f+=10;
					break;
				case 13:
					f+=20;
					break;
				default:
					return;	// don't fire on this frame
			}
			f=f-8+Random(17);
			x=me->x+Cosine(f)*16;
			y=me->y+Sine(f)*16;
			FireBullet(me,x,y,f,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,x,y,SND_CUTOFF,1050);
			me->reload=20;
		}
		if(me->seq==ANIM_DIE && me->frm>=4)
		{
			for(f=0;f<8;f++)
				SpecialSnow(me->x+((-32+Random(65))<<FIXSHIFT),
							me->y+((-32+Random(65))<<FIXSHIFT));
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(Random(64)==0 && me->reload==0 && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				// get him!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

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

void AI_WetWilly(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WILLYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WILLYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=7)
		{
			MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			for(i=0;i<3;i++)
				FireBulletZ(me,x,y,FIXAMT*12,me->facing*32-16+(byte)Random(33),BLT_SHARK,me->friendly);

			me->reload=10;
		}

		if(me->seq==ANIM_DIE && me->frm==8)
		{
			for(x=0;x<16;x++)
				FireBullet(me,me->x,me->y,(byte)Random(256),BLT_SHARK,me->friendly);
			ExplodeParticles2(PART_WATER,me->x,me->y,FIXAMT*20,10,10);
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && Random(20)==0)
	{
		x=Random(256);
		if(x<64)	// make sound sometimes
		{
			if(x&1)
				MakeSound(SND_WILLYIDLE,me->x,me->y,SND_CUTOFF,10);
			else
				MakeSound(SND_WILLYIDLE2,me->x,me->y,SND_CUTOFF,10);
		}
		FireExactBullet(me,me->x,me->y,FIXAMT*14,Cosine(x)*2,Sine(x)*2,Random(4*FIXAMT),0,30,((x+16)&255)/32,BLT_SHARK,me->friendly);
	}

	if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(16)==0)
	{
		// get him!
		FaceGoodguy(me,goodguy);
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

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
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

void AI_CrabPuff(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,8);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->reload==0)
	{
		if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world,me->friendly))
		{
			me->reload=4;
		}
	}

	if(me->mind1)
		me->mind1--;
	else
	{
		me->mind1=5;
		FaceGoodguy2(me,goodguy);
	}

	me->dx=Cosine(me->facing*32)*3;
	me->dy=Sine(me->facing*32)*3;

	if(me->seq!=ANIM_MOVE)
	{
		MakeSound(SND_CRABCLICK,me->x,me->y,SND_CUTOFF,2);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
	}
}

void AI_Patch(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	byte summonTab[]={MONS_PUMPKIN,MONS_PUMPKIN,MONS_BOOMKIN,MONS_SQUASH,MONS_FROG2,MONS_BEETLE};
	byte timerTab[]={30*5,30,30*5,30*3,30*3,30*2};

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PATCHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==18)
		{
			me->seq=ANIM_A1;
			me->frm=0;
		}
		if(me->seq==ANIM_A1 && me->frm==7 && me->reload==0)
		{
			if(me->aiType<=MONS_PATCH4)
			g=AddBaby(me->x,me->y+FIXAMT*2,0,summonTab[me->aiType-MONS_PATCH],me);
			else
			g=AddBaby(me->x,me->y+FIXAMT*2,0,summonTab[me->aiType-MONS_PATCH5+4],me);
			if(g)
			{
				g->mind=1;
				g->mind1=30;
				g->facing=Random(8);
			}
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			me->reload=timerTab[me->aiType-MONS_PATCH];
		}
		return;
	}

	if((!me->reload) && Random(50)==0)
	{
		MakeSound(SND_EGGOPEN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
	}
}

void AI_Patty(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PATTYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PATTYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;
			if(Random(3))
			{
					g=AddBaby(me->x+((-64+Random(129))<<FIXSHIFT),
							me->y+((-64+Random(129))<<FIXSHIFT),0,MONS_CRABPUFF,me);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
					RemoveGuy(g);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>2 && me->frm<6 && me->reload==0)
		{
			// only actually fire if the range is reasonable to avoid creating excess bullets
			if(goodguy && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				// spit bubbles
				x=me->x+Cosine(me->facing*32)*50-FIXAMT*4+(Random(9)<<FIXSHIFT);
				y=me->y+Sine(me->facing*32)*50-FIXAMT*4+(Random(9)<<FIXSHIFT);
				f=me->facing*32-16+Random(33);

				FireBullet(me,x,y,f,BLT_BUBBLE,me->friendly);
			}
			me->reload=0;
		}
		if(me->seq==ANIM_A1 && me->frm>=3 && me->frm<7)
		{
			x=me->x+Cosine(me->facing*32)*48;
			y=me->y+Sine(me->facing*32)*48;
			x=x+Cosine((me->facing*32+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);
			y=y+Sine((me->facing*32+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);

			g=AddBaby(x,y,0,MONS_CRABPUFF,me);
			if(g && !g->CanWalk(g,x,y,map,world))
				RemoveGuy(g);

			x=me->x+Cosine(me->facing*32)*48;
			y=me->y+Sine(me->facing*32)*48;
			x=x+Cosine((me->facing*32+128+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);
			y=y+Sine((me->facing*32+128+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);

			g=AddBaby(x,y,0,MONS_CRABPUFF,me);
			if(g && !g->CanWalk(g,x,y,map,world))
				RemoveGuy(g);
		}
		return;	// can't do nothin' right now
	}

	x=RangeToTarget(me,goodguy);
	if(Random(72)==0 && me->reload==0 && x<600*FIXAMT)
	{
		MakeSound(SND_PATTYSUMMON,me->x,me->y,SND_CUTOFF,1200);
		FaceGoodguy(me,goodguy);
		// summon crabbies
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=1;
		return;
	}
	if(x<(350*FIXAMT) && Random(24)==0 && me->reload==0)
	{
		FaceGoodguy(me,goodguy);
		// spew bubbles
		MakeSound(SND_BLOWBUBBLE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=1;
		return;
	}

	if(me->mind1)
	{
		me->mind=0;
		me->mind1=0;
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		if(Random(5)==0)
		{
			FaceGoodguy(me,goodguy);
			me->facing=(me->facing-2)&7;
		}
		me->facing=(byte)Random(8);
		me->mind=Random(120)+1;

		// move sideways
		if(Random(2)==0)
		{
			me->dx=Cosine((me->facing*32+64)&255)*3;
			me->dy=Sine((me->facing*32+64)&255)*3;
		}
		else
		{
			me->dx=Cosine((me->facing*32+128+64)&255)*3;
			me->dy=Sine((me->facing*32+128+64)&255)*3;
		}
	}


	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;

		if(me->dx==0 && me->dy==0)
		{
			// move sideways
			if(Random(2)==0)
			{
				me->dx=Cosine((me->facing*32+64)&255)*3;
				me->dy=Sine((me->facing*32+64)&255)*3;
			}
			else
			{
				me->dx=Cosine((me->facing*32+128+64)&255)*3;
				me->dy=Sine((me->facing*32+128+64)&255)*3;
			}
		}
	}
}

void DozerFace(Guy *me,Guy *goodguy)
{
	FaceGoodguy(me,goodguy);

	// central turret position
	if(abs(goodguy->x-me->x)<FIXAMT*64 || goodguy->y>me->y+FIXAMT*512)
	{
		if(goodguy->y<me->y)
		{
			if(me->facing<6)
				me->facing=4;
			else
				me->facing=0;
		}
		else
			me->facing=2;
	}
	else if(goodguy->x<me->x)	// left turret positions
	{
		if(goodguy->y<me->y+FIXAMT*60)
			me->facing=4;
		else
			me->facing=3;
	}
	else	// right turret positions
	{
		if(goodguy->y<me->y+FIXAMT*60)
			me->facing=0;
		else
			me->facing=1;
	}
}

void AI_Dozer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,x2,y2;

	x=me->x/FIXAMT+me->rectx;
	x2=me->x/FIXAMT+me->rectx2;
	y=me->y/FIXAMT+me->recty;
	y2=me->y/FIXAMT+me->recty2;

	if(goodguy->y>me->y+FIXAMT*300)
		me->dy+=FIXAMT/16;
	else if(goodguy->y<me->y+FIXAMT*230)
		me->dy-=FIXAMT/16;
	else
		Dampen(&me->dy,FIXAMT/32);

	Dampen(&me->dy,FIXAMT/32);

	Clamp(&me->dy,FIXAMT*4);

	switch(me->mind)
	{
		case 0:	// just born
			me->facing=0;
			me->mind=1;
			break;
		case 1:	// fully armed
			me->facing=0;
			break;
		case 2:	// disarmed
			DozerFace(me,goodguy);
			me->facing+=19;
			break;
		case 3:	// degrilled
			DozerFace(me,goodguy);
			me->facing+=24;
			break;
	}

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		MakeSound(SND_DOZEROUCH,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm>=3 && me->frm<=14)
			{
				me->mind2+=4;
				FireBullet(me,me->x-FIXAMT*200+Cosine(me->mind2)*48,me->y+FIXAMT*160+Sine(me->mind2)*48,me->mind2,BLT_ENERGY,me->friendly);
			}
		}
		if(me->seq==ANIM_A1)
		{
			if(me->frm>=3 && me->frm<=14)
			{
				me->mind2-=4;
				FireBullet(me,me->x+FIXAMT*200+Cosine(me->mind2)*48,me->y+FIXAMT*160+Sine(me->mind2)*48,me->mind2,BLT_ENERGY,me->friendly);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			ShakeScreen(10);
			me->facing=0;
			BlowUpGuy(x-10,y-10,x2+10,y2+10,0,4);
		}
		if(me->seq==ANIM_A2)
		{
			// explode the arms
			ShakeScreen(3);
			BlowUpGuy(x-10,y-10,x+50,y2+10,0,2);
			BlowUpGuy(x2-50,y-10,x2+10,y2+10,0,2);
			if(me->frm==6)
			{
				me->action=ACTION_IDLE;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->mind=2;	// disarmed!
				DozerFace(me,goodguy);
				me->facing+=19;
			}
		}
		if(me->seq==ANIM_A3)
		{
			// explode the grill
			ShakeScreen(3);
			BlowUpGuy(x+30,y2-50,x2-30,y2+10,0,2);

			if(me->frm==5)
			{
				me->action=ACTION_IDLE;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->mind=3;	// degrilled
				DozerFace(me,goodguy);
				me->facing+=24;
			}
		}
		return;
	}

	if(goodguy->x>me->x-FIXAMT*200 && goodguy->x<me->x+FIXAMT*200 && goodguy->y>me->y-FIXAMT*160 && goodguy->y<me->y+FIXAMT*160)
		goodguy->GetShot(me,goodguy,0,0,1,map,world);

	if(me->mind3)
	{
		me->mind3--;
		if(!me->mind3)
		{
			me->mind3=30*5;
		}
		if(me->mind3<30 && (me->mind3&3)==0)
		{
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1200);
			FireExactBullet(me,me->x-FIXAMT*80,me->y-FIXAMT*90,FIXAMT*70,Cosine(0+me->mind3*3)*8,Sine(0+me->mind3*3)*8,FIXAMT*19,0,180,0,BLT_GRENADE,me->friendly);
		}
	}
	else
		me->mind3=30*5;

	if(me->hp<=me->maxHP*0.7 && me->mind<2)	// lose arms at 400hp
	{
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		return;
	}
	if(me->hp<=me->maxHP*0.4 && me->mind<3)	// lose grill at 200hp
	{
		me->seq=ANIM_A3;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		return;
	}

	switch(me->mind)
	{
		case 1:	// fully armed
			if(me->mind1)
			{
				me->mind1--;
				if(me->mind1==0)
					me->mind1=120;
				else if(me->mind1==90)
				{
					MakeSound(SND_DOZERSPIN,me->x,me->y,SND_CUTOFF,600);
					me->mind2=256-16;
					me->seq=ANIM_ATTACK;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->action=ACTION_BUSY;
				}
				else if(me->mind1==30)
				{
					MakeSound(SND_DOZERSPIN,me->x,me->y,SND_CUTOFF,600);
					me->mind2=128+16;
					me->seq=ANIM_A1;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->action=ACTION_BUSY;
				}
			}
			else
				me->mind1=120;
			break;
		case 2:	// disarmed
			if(me->mind1)
			{
				me->mind1--;
				if(me->mind1==0)
					me->mind1=120;
				else if((me->mind1>=90 && me->mind1<100) ||
						(me->mind1>=30 && me->mind1<40))
				{
					x=((me->facing-19)*32-20+Random(41))&255;
					MakeSound(SND_ARMORSHOOT,me->x,me->y,SND_CUTOFF,1200);
					FireExactBullet(me,me->x+Cosine((me->facing-19)*32)*64,
									me->y-FIXAMT*64+Sine((me->facing-19)*32)*48,
									FIXAMT*70,Cosine(x)*6,Sine(x)*6,-FIXAMT/2,0,90,x,BLT_BIGSHELL,me->friendly);
				}
			}
			else
				me->mind1=120;
			break;
		case 3:	// degrilled
			if(me->mind1)
			{
				me->mind1--;
				if(me->mind1==0)
					me->mind1=120;
				else if((me->mind1>=90 && me->mind1<100) ||
						(me->mind1>=30 && me->mind1<40))
				{
					x=((me->facing-24)*32-20+Random(41))&255;
					MakeSound(SND_ARMORSHOOT,me->x,me->y,SND_CUTOFF,1200);
					FireExactBullet(me,me->x+Cosine((me->facing-24)*32)*64,
									me->y-FIXAMT*64+Sine((me->facing-24)*32)*48,
									FIXAMT*70,Cosine(x)*6,Sine(x)*6,-FIXAMT/2,0,90,x,BLT_BIGSHELL,me->friendly);
				}
				if(me->mind1==5)
					LaunchMegabeam(me,me->x,me->y+FIXAMT*140,me->ID);
				if(((me->mind1>=110 && me->mind1<117) ||
					(me->mind1>=60 && me->mind1<67)) && (me->mind1&3)==0)
				{
					MakeSound(SND_ARMORSHOOT,me->x,me->y,SND_CUTOFF,1200);
					for(x=0;x<5;x++)
					{
						FireExactBullet(me,me->x-FIXAMT*90,
										me->y+FIXAMT*140,
										FIXAMT*20,Cosine(64+40-20*x)*6,Sine(64+40-20*x)*6,-FIXAMT/2,0,90,0,BLT_BIGSHELL,me->friendly);
						FireExactBullet(me,me->x+FIXAMT*90,
										me->y+FIXAMT*140,
										FIXAMT*20,Cosine(64+40-20*x)*6,Sine(64+40-20*x)*6,-FIXAMT/2,0,90,0,BLT_BIGSHELL,me->friendly);
					}
				}
			}
			else
				me->mind1=120;
			break;
	}
}

void AI_MutantZombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->mind3)
		me->mind3--;
	else
	{
		me->mind3=10;
		if(me->hp<me->maxHP && me->hp>0)
			me->hp++;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm<7)
		{
			for(x=0;x<5;x++)
				FireBullet(me,me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),Random(256),BLT_SPORE,me->friendly);
		}
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
		}
		if(me->seq==ANIM_ATTACK && me->frm>6)
		{
			Dampen(&me->dx,FIXAMT/2);
			Dampen(&me->dy,FIXAMT/2);
			if(me->frm==13 && me->frmTimer<64)
			{
				for(x=0;x<256;x+=8)
				{
					FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(x)*12,Sine(x)*12,0,0,16,x,BLT_SPORE,me->friendly);
					FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(x)*6,Sine(x)*6,0,0,16,x,BLT_SPORE,me->friendly);
				}
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>=7 && me->frm<13 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(8,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,1,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(Random(10)==0)
		FireBullet(me,me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),Random(256),BLT_SPORE,me->friendly);

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(200*FIXAMT) && Random(5)==0)
			{
				// get him!
				MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
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
}

void AI_Whatsit(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->parent)
	{
		// sitting on The Thing's tongue
		switch(me->parent->frm)
		{
			case 4:
			case 5:
				me->y+=10*FIXAMT;
				break;
			case 6:
			case 7:
				me->y+=15*FIXAMT;
				break;
			case 8:
				me->parent=NULL;	// I am free!
				if(me->hp>0)
				{
					me->seq=ANIM_MOVE;
					me->frm=0;
					me->frmTimer=0;
					if(me->aiType==MONS_DUMBLE)
					me->frmAdvance=192;
					else
					me->frmAdvance=128;
					me->mind1=200;
					me->mind=1;	// pop out in hunt mode
				}
				if(!me->CanWalk(me,me->x,me->y,map,world))
					me->aiType=MONS_NONE;	// vanish if this spot is occupied
				break;
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0 && me->frm==5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->aiType==MONS_DUMBLE)
			{
				FireExactBullet(me,x,y,FIXAMT*12,Cosine(me->facing*32+256-24)*8,Sine(me->facing*32+256-24)*8,-FIXAMT/4,0,60,me->facing*32,BLT_ENERGY,me->friendly);
				FireExactBullet(me,x,y,FIXAMT*12,Cosine(me->facing*32+256-12)*8,Sine(me->facing*32+256-12)*8,-FIXAMT/4,0,60,me->facing*32,BLT_ENERGY,me->friendly);
				FireExactBullet(me,x,y,FIXAMT*12,Cosine(me->facing*32+12)*8,Sine(me->facing*32+12)*8,-FIXAMT/4,0,60,me->facing*32,BLT_ENERGY,me->friendly);
				FireExactBullet(me,x,y,FIXAMT*12,Cosine(me->facing*32+24)*8,Sine(me->facing*32+24)*8,-FIXAMT/4,0,60,me->facing*32,BLT_ENERGY,me->friendly);
			}
			else
			FireExactBullet(me,x,y,FIXAMT*12,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,-FIXAMT/4,0,60,me->facing*32,BLT_BIGSHELL,me->friendly);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<31)
			me->mind1++;

		if(me->mind1>30 && Random(50)==0)
		{
			switch(Random(16))
			{
				case 0:
					MakeSound(SND_BABYSNORE,me->x,me->y,SND_CUTOFF,800);
					me->seq=ANIM_A1;	// take a nap
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=32;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
				case 2:
				case 3:

					me->seq=ANIM_A3;	// look right
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 4:
				case 5:
				case 6:
					me->seq=ANIM_A4;	// look left
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 7:
				case 8:
					me->mind=2;	// go into wander mode
					me->mind1=Random(40)+1;
					break;
				default:	// blink is very common
					me->seq=ANIM_A2;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
			}
		}
		if(RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			me->mind1=255;

			FaceGoodguy(me,goodguy);
		}
		me->dx=0;
		me->dy=0;
	}
	else if(me->mind==1)	// hunting mode
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(8)==0 && me->reload==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		if(Random(80)==0)
		{
			me->mind1=20;
			me->mind=2;	// wander
		}
		if (me->aiType==MONS_DUMBLE)
		{me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;}
		else
		{me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			if (me->aiType==MONS_DUMBLE)
			me->frmAdvance=144;
			else
			me->frmAdvance=96;
		}
	}
	else	// random wander mode
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			switch(Random(3))
			{
				case 0:
					if(goodguy)
						me->mind=1;	// hunt him
					break;
				case 1:
					me->mind=0;	// stop here and rest
					me->mind1=0;
					break;
				case 2:	// just wander a different way
					me->facing=(byte)Random(8);
					me->mind1=Random(20)+1;
					break;
			}
		}

		if (me->aiType==MONS_DUMBLE)
		{me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;}
		else
		{me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=96;
		}
	}
	// this is needed because his idle frame doesn't fit into his walking animation, so this skips it
	if(me->seq==ANIM_MOVE && me->frm==10 && me->frmTimer>127)
	{
		me->frm=0;
		me->frmTimer=0;
		if (me->aiType==MONS_DUMBLE)
		me->frmAdvance=144;
		else
		me->frmAdvance=96;
	}
}

void AI_ShockGhost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			MakeSound(SND_GHOSTOUCH,me->x,me->y,SND_CUTOFF,1200);
		}
		else
			MakeSound(SND_GHOSTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(Random(3)==0)
	{
		if(RangeToTarget(me,goodguy)<180*FIXAMT && me->hp>0)
		{
			goodguy->GetShot(me,goodguy,0,0,1,map,world);
			LightningBolt(
					me->x-FIXAMT*32+Random(FIXAMT*64),
					me->y-FIXAMT*82+Random(FIXAMT*64),
					goodguy->x-FIXAMT*32+Random(FIXAMT*64),
					goodguy->y-FIXAMT*52+Random(FIXAMT*64));
			MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,400);
		}
		else
		{
			// zap around yourself
			LightningBolt(
					me->x-FIXAMT*32+Random(FIXAMT*64),
					me->y-FIXAMT*82+Random(FIXAMT*64),
					me->x-FIXAMT*32+Random(FIXAMT*64),
					me->y-FIXAMT*82+Random(FIXAMT*64));
		}
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0)
		{
			me->reload=10;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0 && me->hp>0)	// hasn't spotted Bouapha yet
	{
		if((goodguy && RangeToTarget(me,goodguy)<500*FIXAMT) || (me->ouch>0))
		{
			MakeSound(SND_GHOSTYELL,me->x,me->y,SND_CUTOFF,1200);
			me->mind=1;
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;	// materialize
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
		}
		return;
	}
	else if(me->mind==1)	// has spotted him, give chase
	{
		FaceGoodguy3(me,goodguy);
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(RangeToTarget(me,goodguy)<250*FIXAMT && Random(60)==1)
		{
			// scream
			MakeSound(SND_GHOSTYELL,me->x,me->y,SND_CUTOFF,1200);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=72;
			me->dx=0;
			me->dy=0;
			return;
		}
	}
}

void AI_Mechazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*50;
			y=me->y+Sine(me->facing*32)*50;
			FireBullet(me,x,y,(me->facing*32-12+Random(25))&255,BLT_ENERGY,me->friendly);
			me->reload=15;
			me->mind=0;
		}
		if(me->seq==ANIM_DIE && me->frm>3 && Random(4)==0)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
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

void AI_Sucker(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int smlFactor,bigFactor;

	if(me->reload)
		me->reload--;

	if(me->mind1<255)
		me->mind1++;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=0;
	}

	if(me->aiType==MONS_SUCKER1 || me->aiType==MONS_SUCKER3)	// normal strength
	{
		smlFactor=128;
		bigFactor=1024;
	}
	else	// strong sucking
	{
		smlFactor=32;
		bigFactor=256;
	}

	if(me->aiType==MONS_SUCKER3 || me->aiType==MONS_SUCKER4)	// short range
		if(RangeToTarget(me,goodguy)>512*FIXAMT)
			return;

	if(goodguy)
	{
		if(goodguy->x<me->x)
		{
			goodguy->dx+=(me->mind1*FIXAMT/smlFactor);
			goodguy->dy+=(me->mind1*FIXAMT/bigFactor);
		}
		else
		{
			goodguy->dx-=(me->mind1*FIXAMT/smlFactor);
			goodguy->dy-=(me->mind1*FIXAMT/bigFactor);
		}
		if(goodguy->y<me->y)
		{
			goodguy->dy+=(me->mind1*FIXAMT/smlFactor);
			goodguy->dx-=(me->mind1*FIXAMT/bigFactor);
		}
		else
		{
			goodguy->dy-=(me->mind1*FIXAMT/smlFactor);
			goodguy->dx+=(me->mind1*FIXAMT/bigFactor);
		}
		Clamp(&goodguy->dx,FIXAMT*6);
		Clamp(&goodguy->dy,FIXAMT*6);
	}
}

void AI_Blower(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int smlFactor,bigFactor;

	if(me->reload)
		me->reload--;

	if(me->mind1<255)
		me->mind1++;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=0;
	}

	if(me->aiType==MONS_BLOWER1 || me->aiType==MONS_BLOWER3)	// normal strength
	{
		smlFactor=128;
		bigFactor=1024;
	}
	else	// strong blowing
	{
		smlFactor=32;
		bigFactor=256;
	}

	if(me->aiType==MONS_BLOWER3 || me->aiType==MONS_BLOWER4)	// short range
		if(RangeToTarget(me,goodguy)>512*FIXAMT)
			return;

	if(goodguy)
	{
		if(goodguy->x<me->x)
		{
			goodguy->dx-=(me->mind1*FIXAMT/smlFactor);
			goodguy->dy-=(me->mind1*FIXAMT/bigFactor);
		}
		else
		{
			goodguy->dx+=(me->mind1*FIXAMT/smlFactor);
			goodguy->dy+=(me->mind1*FIXAMT/bigFactor);
		}
		if(goodguy->y<me->y)
		{
			goodguy->dy-=(me->mind1*FIXAMT/smlFactor);
			goodguy->dx+=(me->mind1*FIXAMT/bigFactor);
		}
		else
		{
			goodguy->dy+=(me->mind1*FIXAMT/smlFactor);
			goodguy->dx-=(me->mind1*FIXAMT/bigFactor);
		}
		Clamp(&goodguy->dx,FIXAMT*6);
		Clamp(&goodguy->dy,FIXAMT*6);
	}
}

void AI_Beetle(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,4);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->reload==0)
	{
		if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world,me->friendly))
		{
			me->reload=4;
		}
	}

	if(me->mind1)
		me->mind1--;
	else
	{
		me->mind1=5;
		FaceGoodguy2(me,goodguy);
	}

	me->dx=Cosine(me->facing*32)*4;
	me->dy=Sine(me->facing*32)*4;

	if(me->seq!=ANIM_MOVE)
	{
		MakeSound(SND_CRABCLICK,me->x,me->y,SND_CUTOFF,2);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
	}
}

void AI_Scarab(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD3OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD3DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;
			if(Random(3))
			{
					g=AddBaby(me->x+((-64+Random(129))<<FIXSHIFT),
							me->y+((-64+Random(129))<<FIXSHIFT),0,MONS_BEETLE,me);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
					RemoveGuy(g);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>2 && me->frm<9 && me->reload==0)
		{
			// only actually fire if the range is reasonable to avoid creating excess bullets
			if(goodguy && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				// spit flames
				for(i=0;i<4;i++)
				{
					x=me->x+Cosine(me->facing*32)*50-FIXAMT*16+(Random(33)<<FIXSHIFT);
					y=me->y+Sine(me->facing*32)*50-FIXAMT*16+(Random(33)<<FIXSHIFT);
					FireBullet(me,x,y,me->facing,BLT_FLAME,me->friendly);
				}
			}
			me->reload=0;
		}
		if(me->seq==ANIM_A1 && me->frm>=3 && me->frm<7)
		{
			x=me->x+Cosine(me->facing*32)*48;
			y=me->y+Sine(me->facing*32)*48;
			x=x+Cosine((me->facing*32+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);
			y=y+Sine((me->facing*32+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);

			g=AddBaby(x,y,0,MONS_BEETLE,me);
			if(g && !g->CanWalk(g,x,y,map,world))
				RemoveGuy(g);

			x=me->x+Cosine(me->facing*32)*48;
			y=me->y+Sine(me->facing*32)*48;
			x=x+Cosine((me->facing*32+128+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);
			y=y+Sine((me->facing*32+128+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);

			g=AddBaby(x,y,0,MONS_BEETLE,me);
			if(g && !g->CanWalk(g,x,y,map,world))
				RemoveGuy(g);
		}
		return;	// can't do nothin' right now
	}

	x=RangeToTarget(me,goodguy);
	if(Random(72)==0 && me->reload==0 && x<600*FIXAMT)
	{
		MakeSound(SND_SPD3SPEW,me->x,me->y,SND_CUTOFF,1200);
		FaceGoodguy(me,goodguy);
		// summon beetles
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=1;
		return;
	}
	if(x<(350*FIXAMT) && Random(24)==0 && me->reload==0)
	{
		MakeSound(SND_SPD3CHOMP,me->x,me->y,SND_CUTOFF,1200);
		FaceGoodguy(me,goodguy);
		// spew flame
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=1;
		return;
	}

	if(me->mind1)
	{
		me->mind=0;
		me->mind1=0;
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		if(Random(5)==0)
		{
			FaceGoodguy2(me,goodguy);
		}
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;

		me->dx=Cosine((me->facing*32)&255)*2;
		me->dy=Sine((me->facing*32)&255)*2;
	}


	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;

		if(me->dx==0 && me->dy==0)
		{
			me->dx=Cosine((me->facing*32)&255)*2;
			me->dy=Sine((me->facing*32)&255)*2;
		}
	}
}

void AI_Frostbiter(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD2OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD2DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0)
		{
			// spit ice shot
			MakeSound(SND_SPD2SPIT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(me,x,y,me->facing*32,BLT_FREEZE,me->friendly);
			me->reload=40;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0 && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			// spit on him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(120)+1;
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

void AI_UnderMagmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=12 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*50;
			y=me->y+Sine(me->facing*32)*50;
			FireBullet(me,x,y,me->facing,BLT_FLAME2,me->friendly);
			me->reload=2;
			me->mind=0;
		}
		if(me->seq==ANIM_ATTACK && me->frm==13 && me->reload==0)
			me->reload=20;

		if(me->seq==ANIM_DIE)
		{
			me->z+=FIXAMT*2-me->dz;
			me->dz=0;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			// spit on him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
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

void AI_ParkedCar(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->mind==0)
	{
		me->facing=Random(8);
		me->mind=1;	// randomize the facings for art
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}
}

void AI_Traffic(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte ok[4];
	int diff,dir;
	static byte noiseLoop=0;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		me->friendly=1-me->friendly;
		FindVictims(me,(me->x+me->dx)>>FIXSHIFT,(me->y+me->dy)>>FIXSHIFT,16,-FIXAMT*10+Random(FIXAMT*21),-FIXAMT*10+Random(FIXAMT*21),20,map,world,me->friendly);
		me->friendly=1-me->friendly;
		FindVictims(me,(me->x+me->dx)>>FIXSHIFT,(me->y+me->dy)>>FIXSHIFT,16,-FIXAMT*10+Random(FIXAMT*21),-FIXAMT*10+Random(FIXAMT*21),20,map,world,me->friendly);
	}

	me->facing=me->mind2/32;	// mind2 holds the 'true' visual facing 0-255

	// mind2 is the visual facing, mind3 is the actual facing,
	// so rotate mind2 towards mind3
	if(me->mind3>me->mind2)
	{
		diff=me->mind3-me->mind2;
		if(diff>128)
		{
			dir=-1;
			diff=256-diff;
		}
		else
			dir=1;
	}
	else if(me->mind3<me->mind2)
	{
		diff=me->mind2-me->mind3;
		if(diff>128)
		{
			dir=1;
			diff=256-diff;
		}
		else
			dir=-1;
	}
	else
	{
		diff=0;
		dir=0;
	}

	if(diff>me->mind1/4)
		diff=me->mind1/4;

	me->mind2=(me->mind2+diff*dir)&255;

	if(!me->mind)	// not stuck
	{
		noiseLoop++;
		if(noiseLoop>=5)
		{
			noiseLoop=0;
			MakeSound(SND_CARGO,me->x,me->y,SND_CUTOFF,1);
		}
	}

	// if you've reached the center of a tile, time to decide where to go
	x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	if((me->x<=x && me->oldx>x) || (me->x>=x && me->oldx<x) ||
	   (me->y<=y && me->oldy>y) || (me->y>=y && me->oldy<y))
	{
		me->x=x;
		me->y=y;
		// figure out which directions are valid minecart paths
		if(me->mapx<map->width-1 && (GetTerrain(world,map->map[me->mapx+1+me->mapy*map->width].floor)->flags&TF_MINECART) &&
			!(GetTerrain(world,map->map[me->mapx+1+me->mapy*map->width].floor)->flags&TF_SOLID) &&
			!(GetItem(map->map[me->mapx+1+me->mapy*map->width].item)->flags&IF_SOLID))
			ok[0]=1;
		else
			ok[0]=0;
		if(me->mapy<map->height-1 && (GetTerrain(world,map->map[me->mapx+(me->mapy+1)*map->width].floor)->flags&TF_MINECART) &&
			!(GetTerrain(world,map->map[me->mapx+(me->mapy+1)*map->width].floor)->flags&TF_SOLID) &&
			!(GetItem(map->map[me->mapx+(me->mapy+1)*map->width].item)->flags&IF_SOLID))
			ok[1]=1;
		else
			ok[1]=0;
		if(me->mapx>0 && (GetTerrain(world,map->map[me->mapx-1+me->mapy*map->width].floor)->flags&TF_MINECART) &&
			!(GetTerrain(world,map->map[me->mapx-1+me->mapy*map->width].floor)->flags&TF_SOLID) &&
			!(GetItem(map->map[me->mapx-1+me->mapy*map->width].item)->flags&IF_SOLID))
			ok[2]=1;
		else
			ok[2]=0;
		if(me->mapy>0 && (GetTerrain(world,map->map[me->mapx+(me->mapy-1)*map->width].floor)->flags&TF_MINECART) &&
			!(GetTerrain(world,map->map[me->mapx+(me->mapy-1)*map->width].floor)->flags&TF_SOLID) &&
			!(GetItem(map->map[me->mapx+(me->mapy-1)*map->width].item)->flags&IF_SOLID))
			ok[3]=1;
		else
			ok[3]=0;

		ok[(((me->mind3/32)+4)&7)/2]=0;

		// count how many directions are available
		y=0;
		for(x=0;x<4;x++)
			if(ok[x])
				y++;

		if(y==0)	// no directions to go!
		{
			me->mind=1;
			me->dx=0;
			me->dy=0;
			return;
		}
		else
		{
			if(y==1)
			{
				// face the only valid direction
				for(x=0;x<4;x++)
					if(ok[x])
						me->mind3=x*64;
			}
			else
			{
				// if you can go straight, then that's what you do
				if(ok[me->mind3/64])
				{
					me->mind3=(me->mind3/64)*64;
				}
				else
				{
					// in the case of multiple directions, we must decide on one
					// based on the type of car
					if(me->aiType==MONS_TRAFFIC)
					{
						if(ok[(me->mind3/64+1)&3])	// direction to your right
							me->mind3=((me->mind3/64+1)&3)*64;
						else if(ok[(me->mind3/64/2+3)&3])	// direction to your left
							me->mind3=((me->mind3/64+3)&3)*64;
						else
						{
							// no valid direction except back!
							me->mind=1;
						}
					}
					else
					{
						if(ok[(me->mind3/64+3)&3])	// direction to your left
							me->mind3=((me->mind3/64+3)&3)*64;
						else if(ok[(me->mind3/64+1)&3])	// direction to your right
							me->mind3=((me->mind3/64+1)&3)*64;
						else
						{
							// no valid direction except back!
							me->mind=1;
						}
					}

				}
			}
		}
	}

	// move forward at that pace
	me->dx=Cosine(me->mind3)*(me->mind1/8);
	me->dy=Sine(me->mind3)*(me->mind1/8);

	if(me->mind1<64)
		me->mind1++;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
	}
}

void AI_ShroomLord(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,j;
	Guy *g;
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=8 && me->reload==0)
		{
			if(me->frm==6)
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
			i=(me->facing*32-32+me->frm)&255;
			for(j=-32+me->frm;j<33;j+=me->frm*2)
			{
				x=me->x+Cosine(i)*32;
				y=me->y+Sine(i)*24;
				if(me->aiType==MONS_HEALER)
					FireExactBullet(me,x,y,FIXAMT*20,Cosine(i)*8,Sine(i)*8,0,0,16,i,BLT_SPOREGOOD,me->friendly);
				else
					FireExactBullet(me,x,y,FIXAMT*20,Cosine(i)*8,Sine(i)*8,0,0,16,i,BLT_SPORE,me->friendly);
				i=(i+me->frm*2)&255;
			}
			me->reload=1;
		}
		if(me->seq==ANIM_DIE && me->frm==1 && me->frmTimer<64)
		{
			if(me->aiType==MONS_SHROOMLORD)
			{
				g=AddBaby(me->x+(32<<FIXSHIFT),me->y,0,MONS_SHROOM,me);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					RemoveGuy(g);
				g=AddBaby(me->x-(32<<FIXSHIFT),me->y,0,MONS_SHROOM,me);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					RemoveGuy(g);
				for(i=0;i<30;i++)
					BlowSmoke(me->x-32*FIXAMT+Random(64*FIXAMT),
							 me->y-32*FIXAMT+Random(64*FIXAMT),
									Random(64)<<FIXSHIFT,Random(FIXAMT*4));
			}
			else if(me->aiType==MONS_HEALER)
			{

				for(i=0;i<30;i++)
					BlowSmoke(me->x-32*FIXAMT+Random(64*FIXAMT),
							 me->y-32*FIXAMT+Random(64*FIXAMT),
									Random(64)<<FIXSHIFT,Random(FIXAMT*4));
				for(i=0;i<16;i++)
					FireExactBullet(me,me->x,me->y,FIXAMT*20,Cosine(i*16)*12,Sine(i*16)*12,0,0,16,i*16,BLT_SPOREGOOD,me->friendly);
			}
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
			if(Random(256)==0)
			{
				me->mind=1;		// occasionally rest
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
			me->mind=0;	// get back on track
		}

		me->dx=0;
		me->dy=0;
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
}

void AI_BigBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SCARIEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SCARIERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			BlowSmoke(me->x,me->y,me->z,FIXAMT*2);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm==2 && me->frmTimer<32)
			{
				g=AddBaby(me->x,me->y,me->z,MONS_BAT,me);
				if(g)
				{
					if(!g->CanWalk(g,g->x,g->y,map,world))
						g->type=MONS_NONE;
					g->z=me->z;
					g->facing=me->facing;
					g->action=ACTION_BUSY;
					g->seq=ANIM_A1;	// diving attack move
					g->frm=0;
					g->frmTimer=0;
					g->frmAdvance=128;
					g->dx=Cosine(g->facing*32)*12;
					g->dy=Sine(g->facing*32)*12;
				}
				me->reload=30;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly fly towards the player
	{
		if(goodguy)
		{
			if(me->mind1>0)
				me->mind1--;
			else
			{
				FaceGoodguy2(me,goodguy);
				me->mind1=8;
				if(Random(16)==0)
				{
					me->mind=1;		// occasionally wander
					me->mind1=60;
				}
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
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=60;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			if((goodguy) && Random(16)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(me->facing-1+Random(3))&7;
			me->mind1=8;
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
	if(me->reload==0 && Random(32)==0 && goodguy && RangeToTarget(me,goodguy)<512*FIXAMT)
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		MakeSound(SND_SCARIERSPIT,me->x,me->y,SND_CUTOFF,1200);
	}
}

void AI_HugeBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_UBEROUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_UBERDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0)
		{
			x=me->x-FIXAMT*32;
			y=me->y+FIXAMT*20;
			g=AddBaby(x,y,FIXAMT*40,MONS_BAT,me);
			if(g)
			{
				g->facing=3;
				g->action=ACTION_BUSY;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
			x=me->x;
			g=AddBaby(x,y,FIXAMT*40,MONS_BAT,me);
			if(g)
			{
				g->facing=2;
				g->action=ACTION_BUSY;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
			x=me->x+FIXAMT*32;
			g=AddBaby(x,y,FIXAMT*40,MONS_BAT,me);
			if(g)
			{
				g->facing=1;
				g->action=ACTION_BUSY;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
			me->reload=60;
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

		return;	// can't do nothin' right now
	}

	if(goodguy && me->reload==0 && RangeToTarget(me,goodguy)<512*FIXAMT)
	{
		// spit some bats
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		MakeSound(SND_UBERSPIT,me->x,me->y,SND_CUTOFF,1200);
	}
}

void AI_Werewolf(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WOLFOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WOLFDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->mind3)
		me->mind3--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->reload==0 && me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,8,map,world);
				me->reload=10;
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world);
			}
		}
		if(me->seq==ANIM_A2)
		{
			// constantly turn toward goodguy
			FaceGoodguy2(me,goodguy);
			if(me->frm<15 && me->frm>4)
			{
				x=Cosine(me->facing*32)*12;
				y=Sine(me->facing*32)*12;
			}
			else
			{
				x=0;
				y=0;
			}
			me->dx-=x;
			Dampen(&me->dx,FIXAMT+FIXAMT/2);
			me->dx+=x;
			me->dy-=y;
			Dampen(&me->dy,FIXAMT+FIXAMT/2);
			me->dy+=y;
		}
		if(me->seq==ANIM_A3 && me->frm==6 && me->reload==0)
		{
			// howl effect
			if(me->aiType==MONS_WOLF2)
			{
				me->reload=10;
				me->mind3=30*5;
			}
			else if(me->aiType==MONS_DIREWOLF)
			{
				for(i=0;i<4;i++)
				{
					x=me->x+Cosine(i*64)*64;
					y=me->y+Sine(i*64)*64;
					g=AddBaby(x,y,0,MONS_WOLF,me);
					MakeSound(SND_WOLFPOUNCE,me->x,me->y,SND_CUTOFF,1200);
					if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
						RemoveGuy(g);
					else if(g)
					{
						g->seq=ANIM_A3;	// arising from the ground
						g->frm=0;
						g->frmTimer=0;
						g->frmAdvance=128;
						g->action=ACTION_BUSY;
					}
				}
			}
			else if(me->aiType==MONS_ANUBIS)
			{
				RaiseMummies(me);
			}
		}
		if(me->seq==ANIM_A4 && me->frm>6 && me->reload==0)
		{
			// howl effect
			me->reload=2;
			FireBullet(me,me->x,me->y,me->facing*32-8+Random(16),BLT_FREEZE2,me->friendly);
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=190;
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(12)==0)
	{
		// get him!
		MakeSound(SND_WOLFPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=160;
		me->action=ACTION_BUSY;
		me->reload=0;
		me->dx=0;
		me->dy=0;
		return;
	}
	if(me->aiType==MONS_ARCTICWOLF && RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(24)==0)
	{
		// get him!
		MakeSound(SND_WOLFPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A4;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=160;
		me->action=ACTION_BUSY;
		me->reload=0;
		me->dx=0;
		me->dy=0;
		return;
	}
	if(me->aiType==MONS_WOLF2 && RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0)
	{
		// charge!!!
		MakeSound(SND_WOLFCHARGE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A2;
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
	if(me->aiType==MONS_ARCTICWOLF && RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0)
	{
		// charge!!!
		MakeSound(SND_WOLFCHARGE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A4;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	if(((me->aiType==MONS_WOLF2||me->aiType==MONS_DIREWOLF)||(me->aiType==MONS_ANUBIS && DyingSkellies(me))) && me->mind3==0 && me->mind2==0 && Random(128)==0)
	{
		// howl
		MakeSound(SND_WOLFHOWL,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A3;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}
	
	if(me->seq==ANIM_MOVE)
		me->frmAdvance=256;

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		FaceGoodguy(me,goodguy);

		x=4+2*(me->aiType==MONS_WOLF);
		me->dx=Cosine(me->facing*32)*x;
		me->dy=Sine(me->facing*32)*x;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128-32*(me->aiType==MONS_WOLF2);
		}
		if(Random(512)==0)
		{
			me->mind=1;		// occasionally wander
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=(byte)Random(30)+1;
		}

		x=4+2*(me->aiType==MONS_WOLF)-1*(me->aiType==MONS_ARCTICWOLF)-1*(me->aiType==MONS_DIREWOLF);
		me->dx=Cosine(me->facing*32)*x;
		me->dy=Sine(me->facing*32)*x;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128-32*(me->aiType==MONS_WOLF2);
		}
	}
}

byte TargetInSight(Guy *me,Map *map,Guy *goodguy)
{
	if(!goodguy || goodguy->type==MONS_NOBODY)
		return 0;

	switch(me->mind2)
	{
		case 0:
			if(goodguy->mapx>me->mapx && goodguy->mapx-me->mapx<12)
			{
				// on the right side anyway, and within the range
				if(abs(goodguy->mapy-me->mapy)<2+(goodguy->mapx-me->mapx)/(12/6))
				{
					// within the cone
					return map->FindGuy(me->mapx,me->mapy,8,goodguy);
				}
			}
			return 0;
			break;
		case 2:
			if(goodguy->mapy>me->mapy && goodguy->mapy-me->mapy<12)
			{
				// on the right side anyway, and within the range
				if(abs(goodguy->mapx-me->mapx)<2+(goodguy->mapy-me->mapy)/(12/6))
				{
					// within the cone
					return map->FindGuy(me->mapx,me->mapy,8,goodguy);
				}
			}
			return 0;
			break;
		case 4:
			if(goodguy->mapx<me->mapx && -goodguy->mapx+me->mapx<12)
			{
				// on the right side anyway, and within the range
				if(abs(goodguy->mapy-me->mapy)<2+(-goodguy->mapx+me->mapx)/(12/6))
				{
					// within the cone
					return map->FindGuy(me->mapx,me->mapy,8,goodguy);
				}
			}
			return 0;
			break;
		case 6:
			if(goodguy->mapy<me->mapy && -goodguy->mapy+me->mapy<12)
			{
				// on the right side anyway, and within the range
				if(abs(goodguy->mapx-me->mapx)<2+(-goodguy->mapy+me->mapy)/(12/6))
				{
					// within the cone
					return map->FindGuy(me->mapx,me->mapy,8,goodguy);
				}
			}
			return 0;
			break;
	}
	return 0;
}

void AI_Patrol(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte a;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// crush skull
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*10,Sine(me->facing*32)*10,12,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->reload==0 && (me->aiType==MONS_DPATROLLR || me->aiType==MONS_DPATROLUD))
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			a=me->facing*32-16+Random(32);
			FireExactBullet(me,x,y,FIXAMT*20,Cosine(a)*6,Sine(a)*6,FIXAMT*5,0,60,0,BLT_BOMB,me->friendly);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(abs(me->mind2-me->facing)==4)	// max distance
	{
		if(Random(2))
			me->facing++;
		else
			me->facing--;
		me->facing&=7;
	}
	else if(me->mind2==me->facing)
	{
	}
	else	// rotate towards true facing
	{
		int dir,diff;

		if(me->mind2>me->facing)
		{
			diff=me->mind2-me->facing;
			if(diff>4)
			{
				dir=-1;
				diff=8-diff;
			}
			else
				dir=1;
		}
		else
		{
			diff=me->facing-me->mind2;
			if(diff>4)
			{
				dir=1;
				diff=8-diff;
			}
			else
				dir=-1;
		}
		me->facing=(me->facing+dir)&7;
	}
	if(me->mind==0)	// newborn
	{
		me->facing=Random(8);
		if(me->aiType==MONS_PATROLLR || me->aiType==MONS_DPATROLLR)
			me->mind2=0;	// desired facing/also direction of movement
		else
			me->mind2=2;	// desired facing/also direction of movement

		me->mind=1;
	}
	else if(me->mind==1)	// pacing
	{
		if(me->mind1)	// bonked a wall
		{
			me->mind1=0;
			if(me->mind2<4)
				me->mind2+=4;
			else
				me->mind2-=4;
		}
		if(me->mind2!=me->facing)
		{
			me->dx=0;
			me->dy=0;
		}
		else
		{
			me->dx=Cosine(me->mind2*32);
			me->dy=Sine(me->mind2*32);
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}

		if(me->mind2==me->facing && TargetInSight(me,map,goodguy))
		{
			MakeSound(SND_MUMBLECRUSH,me->x,me->y,SND_CUTOFF,1000);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			if(goodguy->aiType==MONS_BOUAPHA)
				player.spotted=30*8;
			me->mind=2;	// start clapping!
		}
	}
	else	// staring at goodguy
	{
		if(TargetInSight(me,map,goodguy))
		{
			if(goodguy->aiType==MONS_BOUAPHA)
				player.spotted=30*8;
			MakeSound(SND_CLAP,me->x,me->y,SND_CUTOFF,1000);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
		}
		else
			me->mind=1;
	}
}

void AI_Weatherman(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	WeathermanWeather(me->x,me->y);
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(RangeToTarget(me,goodguy)<100*FIXAMT && Random(2)==0 && me->hp>0)
	{
		goodguy->GetShot(me,goodguy,0,0,2,map,world);
		LightningBolt(
				me->x-FIXAMT*32+Random(FIXAMT*64),
				me->y-FIXAMT*82+Random(FIXAMT*64),
				goodguy->x-FIXAMT*32+Random(FIXAMT*64),
				goodguy->y-FIXAMT*52+Random(FIXAMT*64));
		MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,400);
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
						 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=4 && me->frm<11 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			// throw ice and rain
			MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
			FireBulletZ(me,x,y,FIXAMT*12,(me->facing*32-16+(byte)Random(33))&255,BLT_SHARK,me->friendly);
			if(Random(2)==0)
				FireBulletZ(me,x,y,FIXAMT*12,(me->facing*32-16+(byte)Random(33))&255,BLT_FREEZE2,me->friendly);
		}
		if(me->seq==ANIM_A1 && goodguy->aiType==MONS_BOUAPHA)	// unga bunga!
		{
			me->mind1=255;

			if(goodguy->x<me->x)
			{
				goodguy->dx+=(me->mind1*FIXAMT/16);
				goodguy->dy+=(me->mind1*FIXAMT/128);
			}
			else
			{
				goodguy->dx-=(me->mind1*FIXAMT/16);
				goodguy->dy-=(me->mind1*FIXAMT/128);
			}
			if(goodguy->y<me->y)
			{
				goodguy->dy+=(me->mind1*FIXAMT/16);
				goodguy->dx-=(me->mind1*FIXAMT/128);
			}
			else
			{
				goodguy->dy-=(me->mind1*FIXAMT/16);
				goodguy->dx+=(me->mind1*FIXAMT/128);
			}
			Clamp(&goodguy->dx,FIXAMT*6);
			Clamp(&goodguy->dy,FIXAMT*6);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && Random(200)==0)
		{
			me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
			me->mind1=0;
		}
		if(RangeToTarget(me,goodguy)<320*FIXAMT || me->ouch>0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			me->mind1=255;	// for a long time
			FaceGoodguy(me,goodguy);

			// do angry animation
			me->action=ACTION_BUSY;
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;	// very fast
			MakeSound(SND_PYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
		}
		me->dx=0;
		me->dy=0;
	}
	else
	{
		FaceGoodguy2(me,goodguy);

		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0 && me->reload==0)
		{
			// fire ice & rain
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0 && me->reload==0)
		{
			// suck him in
			MakeSound(SND_UNGADANCE,me->x,me->y,SND_CUTOFF,1200);
			MakeSound(SND_PYGMYSPIN,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=96;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
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

void AI_HunterEgg(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->ouch==4)
	{
		MakeSound(SND_EGGOUCH,me->x,me->y,SND_CUTOFF,1200);

		if(me->seq!=ANIM_ATTACK)
		{
			MakeSound(SND_EGGOPEN,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=127;
			me->action=ACTION_BUSY;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0)
		{
			AddBaby(me->x+FIXAMT*20,me->y+FIXAMT*10,0,MONS_ALIEN,me);
			// then die
			MakeSound(SND_EGGDIE,me->x,me->y,SND_CUTOFF,1200);
			me->hp=1;
			me->GetShot(me,me,0,0,1,map,world);
			me->reload=10;
		}

		if(me->seq==ANIM_DIE)
		{
			FireBullet(me,me->x,me->y,(byte)Random(256),BLT_ACID,me->friendly);
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,4,10);
		}
		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<180*FIXAMT)
	{
		// goodguy got close, pop out
		MakeSound(SND_EGGOPEN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=127;
		me->action=ACTION_BUSY;
	}
}

void AI_PunkBunny(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *bouapha;

	bouapha=GetGuyOfAIType(MONS_BOUAPHA);
	if(!bouapha)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		MakeSound(SND_BUNNYDIE,me->x,me->y,SND_CUTOFF,800);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==3 || me->frm==6 || me->frm==9) && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;

			FireBullet(me,x,y,(me->facing*32-8+Random(17))&255,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,x,y,SND_CUTOFF,100);
			me->reload=3;
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*10;
			y=me->y+Sine(me->facing*32)*10;

			FireBullet(me,x,y,me->facing*32,BLT_BOMB,me->friendly);
			me->reload=3;
		}
		if(me->seq==ANIM_DIE)
		{
			for(x=0;x<8;x++)
				SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT),
							me->y+((-16+Random(33))<<FIXSHIFT));
		}
		return;
	}

	if(me->mind==0)	// not activated yet
	{
		if(RangeToTarget(me,bouapha)<72*FIXAMT)
		{
			me->mind=1;
			MakeNormalSound(SND_FOLLOWHI);
		}
	}
	else if(me->mind==1)	// following Bouapha
	{
		Dampen(&me->dx,FIXAMT/8);
		Dampen(&me->dy,FIXAMT/8);

		if(me->x<bouapha->x)
			me->dx+=FIXAMT;
		else
			me->dx-=FIXAMT;
		if(me->y<bouapha->y)
			me->dy+=FIXAMT;
		else
			me->dy-=FIXAMT;

		me->dx+=-FIXAMT/2+Random(FIXAMT);
		me->dy+=-FIXAMT/2+Random(FIXAMT);

		if(RangeToTarget(me,bouapha)<48*FIXAMT)	// got too close
		{
			me->mind=2;
			me->facing=Random(8);
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->mind1=Random(20)+10;
		}
	}
	else if(me->mind==2) // not following, got too close
	{
		me->mind1--;
		if(!me->mind1)
			me->mind=1;
	}
	else	// hunting for fresh meat
	{
		if(goodguy==NULL || goodguy->type==MONS_NOBODY || RangeToTarget(me,bouapha)>320*FIXAMT)	// got too far from papa!!
		{
			me->mind=1;
			return;
		}
		else
		{
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);

			if(me->x<goodguy->x)
				me->dx+=FIXAMT;
			else
				me->dx-=FIXAMT;
			if(me->y<goodguy->y)
				me->dy+=FIXAMT;
			else
				me->dy-=FIXAMT;

			me->dx+=-FIXAMT/2+Random(FIXAMT);
			me->dy+=-FIXAMT/2+Random(FIXAMT);

			if(RangeToTarget(me,goodguy)<280*FIXAMT && Random(10)==0)
			{
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->dx=0;
				me->dy=0;
				me->action=ACTION_BUSY;
				FaceGoodguy(me,goodguy);
				return;
			}
			if(Random(50)==0)
			{
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				me->action=ACTION_BUSY;
				FaceGoodguy(me,goodguy);
				return;
			}
		}
	}

	// if following Bouapha, and an enemy comes within sight, and you aren't already far from Bouapha
	if((me->mind==1 || me->mind==2) && RangeToTarget(me,goodguy)<320*FIXAMT &&
		RangeToTarget(me,bouapha)<100*FIXAMT)
	{
		me->mind=3;	// enter assault mode
	}

	Clamp(&me->dx,FIXAMT*4);
	Clamp(&me->dy,FIXAMT*4);

	if(me->dx>FIXAMT)
	{
		if(me->dy>FIXAMT)
			me->facing=1;
		else if(me->dy<-FIXAMT)
			me->facing=7;
		else
			me->facing=0;
	}
	else if(me->dx<-FIXAMT)
	{
		if(me->dy>FIXAMT)
			me->facing=3;
		else if(me->dy<-FIXAMT)
			me->facing=5;
		else
			me->facing=4;
	}
	else
	{
		if(me->dy>FIXAMT)
			me->facing=2;
		else if(me->dy<-FIXAMT)
			me->facing=6;
	}

	if(me->mind!=0)
	{
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Skeleton(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==12 && me->reload==0 && goodguy)
		{
			ShakeScreen(120);
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
			for(i=0;i<10;i++)
			{
				x=me->x+((256-Random(513))<<FIXSHIFT);
				y=me->y+((256-Random(513))<<FIXSHIFT);
				FireExactBullet(me,x,y,FIXAMT*80+(Random(40)<<FIXSHIFT),
								0,0,0,0,30,2,BLT_ROCK,me->friendly);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK)
			me->dz+=FIXAMT/4;
		return;	// can't do nothin' right now
	}

	// Randomy decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if((!Random(100)) && me->mind==0)
	{
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				// get him!
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->dz=FIXAMT*10;
				me->reload=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*1;
			me->dy=Sine(me->facing*32)*1;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=32;
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
				me->facing=(me->facing-1+Random(3))&7;
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}
}

void AI_Microzoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_MINIZOIDDIE,me->x,me->y,SND_CUTOFF,1200);
	}
	

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
			me->reload=15;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}
	
	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,4*Random(32));

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_MINIZOIDSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*4;
	me->dy=Sine(me->facing*32)*4;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
	}
}

void AI_HotShroom(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		/*
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
			*/
	}

	if(me->action==ACTION_BUSY)
	{
		if (me->aiType==MONS_DEATHCAP)
		{
			if(me->seq==ANIM_ATTACK && (me->frm==5 ) && me->reload==0 && goodguy)
			{
				for(i=0;i<4;i++)
				{
					FireBullet(me,me->x,me->y,i,BLT_FLAME,me->friendly);
				}
				me->mind4++;

			}
		}
		else
		{
			if(me->seq==ANIM_ATTACK && (me->frm==5 || me->frm==6) && me->reload==0 && goodguy)
			{
				for(i=0;i<8;i++)
				{
					FireBullet(me,me->x,me->y,i,BLT_FLAME,me->friendly);
				}
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(160*FIXAMT) && Random(16)==0)
	{
		// get him!
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
	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy(me,goodguy);

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
}

void AI_MiniWacko(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MINIWACKOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MINIWACKODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<196 && goodguy && (me->frm==5 || me->frm==11 || me->frm==17))
		{
			MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
			me->reload=10;
		}

		if(me->seq==ANIM_DIE && me->frm==5)
			me->frmAdvance=128;
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(8)==0)
			{
				// get him!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			if(RangeToTarget(me,goodguy)<64*FIXAMT)
				FaceGoodguy(me,goodguy);
			else
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
		if(me->mind1 && RangeToTarget(me,goodguy)>64*FIXAMT)
		{
			me->mind1=0;
			me->mind=1;
			me->mind2=Random(30)+10;
		}
		else
			me->mind1=0;
	}
	else
	{
		if(RangeToTarget(me,goodguy)<64*FIXAMT)
			me->mind2=1;
		me->mind2--;
		if(!me->mind2)
			me->mind=0;

		me->facing=(me->facing+1-Random(3))&7;
	}

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

void AI_JackFrost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->dx==0 && me->dy==0)
	{
		me->dx=FIXAMT*4;
		me->dy=FIXAMT*4;
	}

	JackFrostWeather(me->x,me->y);

	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;
	me->mind1=0;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_JACKDIE,me->x,me->y,SND_CUTOFF,2000);
	}


	if(me->mind2==0)
	{
		me->mind2=5;
		FaceGoodguy(me,goodguy);
		x=me->facing*32+32-Random(65);
		if(x<0)
			x+=256;
		if(x>255)
			x-=256;
		FireBullet(me,me->x,me->y,x,BLT_FREEZE2,me->friendly);
	}
	else
		me->mind2--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->frmTimer<64 && (goodguy))
		{
			if(me->AttackCheck(100,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				FaceGoodguy(me,goodguy);
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*12,
								Sine(me->facing*32)*12,20,map,world);
			}
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			//BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<100*FIXAMT)
		{
			MakeSound(SND_JACKATTACK,me->x,me->y,SND_CUTOFF,2000);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->action=ACTION_BUSY;
			return;
		}
	}
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
	}
}