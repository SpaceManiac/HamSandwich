//SLEEPLESS HOLLOW

void WanderAI(Guy *me,int wanderRate,int wanderTime,int unWanderRate,Map *map,world_t *world,Guy *goodguy,int speed)
{
	if(me->action==ACTION_BUSY)
		return;

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		FaceGoodguy(me,goodguy);

		me->dx=(Cosine(me->facing*32)*speed)/FIXAMT;
		me->dy=(Sine(me->facing*32)*speed)/FIXAMT;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		if(Random(wanderRate)==0)
		{
			me->mind=1;		// occasionally wander
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(unWanderRate)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=(byte)Random(wanderTime)+1;
		}

		me->dx=(Cosine(me->facing*32)*speed)/FIXAMT;
		me->dy=(Sine(me->facing*32)*speed)/FIXAMT;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Frog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,j;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_FROGOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_FROGDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->aiType==MONS_FROGURT && Random(3)==0)
	{
		x=me->x-10*FIXAMT+Random(21*FIXAMT);
		y=me->y-10*FIXAMT+Random(21*FIXAMT);
		FireBullet(me,x,y,0,BLT_FLAMEWALL,me->friendly);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && me->aiType==MONS_FROG)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,1,map,world);
			}
			else
			{
				x=me->x+Cosine(me->facing*32)*60;
				y=me->y+Sine(me->facing*32)*60;
				if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,1,map,world);
				}
			}
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && me->aiType==MONS_FROG2)
		{
			x=me->x+Cosine(me->facing*32)*10;
			y=me->y+Sine(me->facing*32)*10;
			i=Random(21);
			FireExactBullet(me,x,y,FIXAMT*100,Cosine(me->facing*32-10+i)*6,Sine(me->facing*32-10+i)*6,FIXAMT*0,
							0,60,me->facing,BLT_BIGSHELL,me->friendly);
			me->reload=40;
		}
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && me->aiType==MONS_FROG3)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(me,x,y,me->facing*32,BLT_FART,me->friendly);
			me->reload=5;
		}
		if(me->seq==ANIM_DIE && me->aiType==MONS_FROG3 && me->frm==2 && me->reload==0)
		{
			for(y=0;y<4;y++)
			{
				x=Random(256);
				FireBullet(me,me->x,me->y,x,BLT_FART,me->friendly);
			}
		}
		if(me->seq==ANIM_DIE && me->type==MONS_FROGURT && me->frm==4 && me->reload==0)
		{
			FireBullet(me,me->x,me->y,0,BLT_FLAMEWALL,me->friendly);
			me->reload=5;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0 && me->aiType!=MONS_FROGURT)	// heading for Bouapha
	{
		if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(8)==0 && me->aiType==MONS_FROG)
		{
			// get him!
			MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
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
		if(RangeToTarget(me,goodguy)<(400*FIXAMT) && Random(8)==0 && me->aiType==MONS_FROG2 && me->reload==0)
		{
			// get him!
			MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(8)==0 && me->aiType==MONS_FROG3 && me->reload==0)
		{
			// get him!
			MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
	}

	WanderAI(me,64,40,3,map,world,goodguy,FIXAMT*3);
}

void AI_MadCow(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_COWOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_COWDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>5)
		{
			me->facing=(me->facing+1)&7;
		}
		return;	// can't do nothin' right now
	}

	if(EatHay(me,1))
	{
		me->hp+=5;
		if(me->hp>me->maxHP)
			me->hp=me->maxHP;
		me->mind1=1;
	}

	x=me->maxHP-me->hp+5;	// more damaged=faster

	if(me->mind==0)
	{
		// walking
		if(goodguy && RangeToTarget(me,goodguy)<64*FIXAMT && map->FindGuy(me->mapx,me->mapy,5,goodguy))
		{
			goodguy->GetShot(me,goodguy,-4*FIXAMT+Random(8*FIXAMT),-4*FIXAMT+Random(8*FIXAMT),10,map,world);
			me->dx=0;
			me->dy=0;
			me->mind=1;	// switch to turning mode
			me->reload=2;
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			MakeSound(SND_COWMOO,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}

			return;
		}
		if(me->mind1)
		{
			// bumped a wall
			MakeSound(SND_TRICERBUMP,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}
			me->mind1=0;
			me->dx=0;
			me->dy=0;
			me->mind=1;	// switch to turning mode
			me->reload=2;

			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		else
		{
			if((me->dx==0 && me->dy==0) || (me->ouch))
			{
				me->dx=Cosine(me->facing*32)*x/5;
				me->dy=Sine(me->facing*32)*x/5;
			}
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=(x/5)*30+40;
			}
		}
	}
	else
	{
		// turning
		if(!me->reload)
		{
			me->facing=(me->facing+me->mind3-1)&7;
			if(me->facing==me->mind2)	// done turning
			{
				me->mind=0;
			}
			me->reload=2;
		}
	}
}

void AI_MadCow2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_COWOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_COWDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->hp<me->maxHP)
		me->hp=me->maxHP;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			FireBullet(me,x,y,me->facing,BLT_FLAME2,0);
		}

		if(me->seq==ANIM_DIE && me->frm>5)
		{
			me->facing=(me->facing+1)&7;
		}
		return;	// can't do nothin' right now
	}

	if(EatHay(me,2))
	{
		me->aiType=MONS_MADCOW;
		me->hp=me->maxHP=MonsterHP(MONS_MADCOW);
		me->aiType=MONS_MADCOW;
		strcpy(me->name,"Annoyed Cow");
		MakeSound(SND_TRICERBUMP,me->x,me->y,SND_CUTOFF,1200);
		return;
	}

	if(Random(40)==0)
	{
		me->dx=0;
		me->dy=0;
		me->reload=2;
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		MakeSound(SND_COWMOO,me->x,me->y,SND_CUTOFF,1200);
		if(Random(2))
			me->facing=(me->facing+1)&7;
		else
			me->facing=(me->facing-1)&7;
		return;
	}
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_PumpkinFly(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->aiType==MONS_PUMPKINFLY)
		map->DimTorch(me->mapx,me->mapy,2);
	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			for(i=0;i<10;i++)
				FireBullet(me,me->x,me->y,0,BLT_BADSITFLAME,me->friendly);
			me->reload=100;
			if(me->aiType==MONS_PUMPKINFLY2)
				FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->aiType==MONS_PUMPKINFLY2 && me->mind2<255)
	{
		me->mind2++;
	}

	if(me->aiType==MONS_PUMPKINFLY2 && me->mind1 && me->mind2>10)
	{
		// explode on impact
		SmashTrees(me,2);
		me->hp=1;
		me->GetShot(me,me,0,0,1,map,world);
		return;
	}


	if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,0,0,0,map,world,me->friendly))
	{
		if(me->aiType==MONS_PUMPKINFLY2)
			SmashTrees(me,2);
		me->hp=1;
		me->GetShot(me,me,0,0,1,map,world);
		return;
	}

	if(Random(4)==0)
		FireBullet(me,me->x,me->y,Random(256),BLT_FLAME3,me->friendly);

	FaceGoodguy(me,goodguy);
	if(!goodguy)
	{
		return;
	}

	me->dx=Cosine(me->facing*32)*4;
	me->dy=Sine(me->facing*32)*4;

	if(me->aiType==MONS_PUMPKINFLY2)
	{
		me->dx*=2;
		me->dy*=2;
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

//marshmallow man!
void AI_MarshmallowMan(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
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
			MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
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

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

void AI_Reflection(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte c;
	mapTile_t *mapTile;

	mapTile=&map->map[me->mapx+me->mapy*map->width];

	if(me->reload)
		me->reload--;

	Dampen(&me->dx,PLYR_DECEL);
	Dampen(&me->dy,PLYR_DECEL);

	me->mind1=0;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_BOUAPHADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	c=FakeGetControls();
	DoPlayerFacing(c,me);

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A3)
		{
			if(me->frm<11)
			{
				me->z=FIXAMT*8;	// hover while spinning feet in the air before plunging into water
				me->dz=FIXAMT;
			}
			else
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,16);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			me->facing=(me->facing+1)&7;
		}

		return;
	}

	// not busy, let's see if you want to do something
	/*if((c&CONTROL_B1) && me->reload==0)	// pushed hammer throw button
	{
		me->seq=ANIM_ATTACK;	// even if unarmed
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=255;
		me->reload=5;
		me->action=ACTION_BUSY;
		return;
	}*/
	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(!(GetTerrain(world,mapTile->floor)->flags&TF_ICE))
		{
			me->dx+=Cosine(me->facing*32)*3/2;
			me->dy+=Sine(me->facing*32)*3/2;
			if((Cosine(me->facing*32)<0 && me->dx>0) || (Cosine(me->facing*32)>0 && me->dx<0))
				me->dx=0;
			if((Sine(me->facing*32)<0 && me->dy>0) || (Sine(me->facing*32)>0 && me->dy<0))
				me->dy=0;

		}
		else // ice is slippery
		{
			me->dx+=Cosine(me->facing*32)/4;
			me->dy+=Sine(me->facing*32)/4;
		}

		if(!(GetTerrain(world,mapTile->floor)->flags&TF_MUD))
		{
			Clamp(&me->dx,PLYR_MAXSPD+1);
			Clamp(&me->dy,PLYR_MAXSPD+1);
		}
		else
		{
			Clamp(&me->dx,PLYR_MAXSPD/2);
			Clamp(&me->dy,PLYR_MAXSPD/2);

			if(me->z==0)
			{
				SpurtParticles(PART_DIRT,0,me->x,me->y,0,((me->facing*32)+128)&255,8);
				SpurtParticles(PART_DIRT,1,me->x,me->y,0,((me->facing*32)+128)&255,8);
			}
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	else	// you aren't trying to move
	{
		// ice is slippery
		if(me->z==0 && me->dz==0 && (!(GetTerrain(world,mapTile->floor)->flags&TF_ICE)))
		{
			Dampen(&me->dx,PLYR_DECEL/2);
			Dampen(&me->dy,PLYR_DECEL/2);
		}
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Goodbone(Guy *me,Map *map,world_t *world,Guy *badguy)
{
	byte c,a;
	int i,x,y;
	mapTile_t *mapTile;

	mapTile=&map->map[me->mapx+me->mapy*map->width];

	if(me->reload)
		me->reload--;
	
	if(goodguy && goodguy->hp==0 && me->hp>0)
	{
		me->hp=1;
		me->GetShot(me,me,0,0,255,map,world);
	}
	Dampen(&me->dx,PLYR_DECEL);
	Dampen(&me->dy,PLYR_DECEL);

	me->mind1=0;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	c=FakeGetControls();
	DoPlayerFacing(c,me);

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A3)
		{
			if(me->frm<11)
			{
				me->z=FIXAMT*8;	// hover while spinning feet in the air before plunging into water
				me->dz=FIXAMT;
			}
			else
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,16);
			}
		}

		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && badguy)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,badguy))
			{
				badguy->GetShot(me,me,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,5,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && badguy)
		{
			x=me->x+Cosine(me->facing*32)*24;
			y=me->y+Sine(me->facing*32)*24;
			a=me->facing*32-(3)*5;
			for(i=0;i<3;i++)
			{
				FireBullet(me,x,y,a,BLT_GREEN,me->friendly);
				a+=10;
			}
			me->reload=10;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=160;

		return;
	}

	// not busy, let's see if you want to do something

	if((c&CONTROL_B1) && me->reload==0)	// pushed hammer throw button
	{
		MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->reload=0;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}
	if(badguy && badguy->hp && RangeToTarget(me,badguy)<40*FIXAMT)
	{
		MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
		FaceGoodguy(me,badguy);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=200;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(!(GetTerrain(world,mapTile->floor)->flags&TF_ICE))
		{
			me->dx+=Cosine(me->facing*32)*3/2;
			me->dy+=Sine(me->facing*32)*3/2;
			if((Cosine(me->facing*32)<0 && me->dx>0) || (Cosine(me->facing*32)>0 && me->dx<0))
				me->dx=0;
			if((Sine(me->facing*32)<0 && me->dy>0) || (Sine(me->facing*32)>0 && me->dy<0))
				me->dy=0;

		}
		else // ice is slippery
		{
			me->dx+=Cosine(me->facing*32)/4;
			me->dy+=Sine(me->facing*32)/4;
		}

		if(!(GetTerrain(world,mapTile->floor)->flags&TF_MUD))
		{
			Clamp(&me->dx,PLYR_MAXSPD);
			Clamp(&me->dy,PLYR_MAXSPD);
		}
		else
		{
			Clamp(&me->dx,PLYR_MAXSPD/2);
			Clamp(&me->dy,PLYR_MAXSPD/2);

			if(me->z==0)
			{
				SpurtParticles(PART_DIRT,0,me->x,me->y,0,((me->facing*32)+128)&255,8);
				SpurtParticles(PART_DIRT,1,me->x,me->y,0,((me->facing*32)+128)&255,8);
			}
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	else	// you aren't trying to move
	{
		// ice is slippery
		if(me->z==0 && me->dz==0 && (!(GetTerrain(world,mapTile->floor)->flags&TF_ICE)))
		{
			Dampen(&me->dx,PLYR_DECEL/2);
			Dampen(&me->dy,PLYR_DECEL/2);
		}
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Darkness(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->hp>0)
		map->BrightTorch(me->mapx,me->mapy,-16,2);

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
			map->BrightTorch(me->mapx,me->mapy,10,6);
			me->reload=100;
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0 && FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,Random(FIXAMT*2),Random(FIXAMT*2),1,map,world,me->friendly))
	{
		MakeSound(SND_ALIENOUCH,me->x,me->y,SND_CUTOFF,200);
		me->reload=2;
	}

	if(Random(4)==0)
		me->facing=(me->facing-1+Random(3))&7;

	me->dx=Cosine(me->facing*32)*4;
	me->dy=Sine(me->facing*32)*4;
}

void AI_Spark(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte wall[8];
	int i,c;

	if(me->reload)
		me->reload--;

	if(Random(2))
		ExplodeParticlesColor(7,me->x,me->y,me->z,1,1);

	// find out which sides have walls
	if(me->mapx<map->width-1)
	{
		wall[0]=(map->map[me->mapx+1+me->mapy*map->width].wall>0);
		if(me->mapy<map->height-1)
			wall[1]=(map->map[me->mapx+1+(me->mapy+1)*map->width].wall>0);
		else
			wall[1]=1;
		if(me->mapy>0)
			wall[7]=(map->map[me->mapx+1+(me->mapy-1)*map->width].wall>0);
		else
			wall[7]=1;
	}
	else
		wall[0]=1;
	if(me->mapy<map->height-1)
		wall[2]=(map->map[me->mapx+(me->mapy+1)*map->width].wall>0);
	else
		wall[2]=1;
	if(me->mapx>0)
	{
		wall[4]=(map->map[me->mapx-1+me->mapy*map->width].wall>0);
		if(me->mapy<map->height-1)
			wall[3]=(map->map[me->mapx-1+(me->mapy+1)*map->width].wall>0);
		else
			wall[3]=1;
		if(me->mapy>0)
			wall[5]=(map->map[me->mapx-1+(me->mapy-1)*map->width].wall>0);
		else
			wall[5]=1;
	}
	else
		wall[4]=1;
	if(me->mapy>0)
		wall[6]=(map->map[me->mapx+(me->mapy-1)*map->width].wall>0);
	else
		wall[6]=1;

	// shock neighboring walls for effect
	if(Random(3)==0)
	{
		if(wall[0])
			LightningBolt(me->x,me->y-me->z,((me->mapx+1)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(me->mapy*TILE_HEIGHT+Random(TILE_HEIGHT))*FIXAMT-FIXAMT*20);
		if(wall[2])
			LightningBolt(me->x,me->y-me->z,(me->mapx*TILE_WIDTH+Random(TILE_WIDTH))*FIXAMT,((me->mapy+1)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-FIXAMT*20);
		if(wall[4])
			LightningBolt(me->x,me->y-me->z,((me->mapx-1)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(me->mapy*TILE_HEIGHT+Random(TILE_HEIGHT))*FIXAMT-FIXAMT*20);
		if(wall[6])
			LightningBolt(me->x,me->y-me->z,(me->mapx*TILE_WIDTH+Random(TILE_WIDTH))*FIXAMT,((me->mapy-1)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-FIXAMT*20);
	}

	if(goodguy && RangeToTarget(me,goodguy)<48*FIXAMT && me->reload==0)
	{
		me->reload=20;
		LightningBolt(me->x,me->y-me->z,goodguy->x,goodguy->y-goodguy->z-FIXAMT*25);
		goodguy->GetShot(me,goodguy,0,0,1,map,world);
	}

	if(me->mind==1)	// just entered a new space
	{
		me->mind1++;
		if(me->mind1==50)
		{
			me->aiType=MONS_NONE;
			return;
		}
		c=0;
		for(i=0;i<8;i++)
			if(wall[i])
				c++;

		if(c==0)	// no walls neighboring at all!
		{
			//me->aiType=MONS_NONE;
			//return;
		}

		me->mind=0;

		if(wall[(me->facing+2)&7])	// huggable wall
		{
			if(wall[(me->facing)])	// but forward motion blocked
			{
				if(wall[(me->facing+6)&7])	// and wrong-way blocked
					me->facing=(me->facing+4)&7;	// so turn around
				else
					me->facing=(me->facing+6)&7;
			}
			else
				me->facing=me->facing;
		}
		else
		{
			// huggable wall is missing
			if(wall[(me->facing+3)&7])
				me->facing=(me->facing+2)&7;	// can cornerhug
			else if(wall[(me->facing+6)&7])	// can hold onto opposite wall
			{
				if(wall[(me->facing)])	// but forward motion blocked
				{
					if(wall[(me->facing+2)&7])	// and right-way blocked
						me->facing=(me->facing+4)&7;	// so turn around
					else
						me->facing=(me->facing+2)&7;
				}
				else
					me->facing=me->facing;
			}
			else if(wall[(me->facing+5)&7])	// can't do that either, but can cornerhug the wrong way
				me->facing=(me->facing+6)&7;
			else if(wall[(me->facing)&7])	// headed straight into a wall
				me->facing=(me->facing+2)&7;
			else
			{
				//me->aiType=MONS_NONE;
				//return;	// nothing to hug in any direction!
			}
		}
	}
	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;
	if(me->facing==0 || me->facing==4)
	{
		if(me->y<(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT)
			me->dy+=FIXAMT;
		else
			me->dy-=FIXAMT;
	}
	if(me->facing==2 || me->facing==6)
	{
		if(me->x<(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT)
			me->dx+=FIXAMT;
		else
			me->dx-=FIXAMT;
	}
}

void AI_LightSwitch(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->seq==ANIM_ATTACK)
	{
		if(me->frm>7)
			me->frm=7;
		map->DimTorch(me->mapx,me->mapy,3);
		//if(Random(3)==0)
			map->BrightTorch(me->mapx,me->mapy,16,2);
	}
}

void AI_LightBlaster(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte a;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->reload)
		me->reload--;

	if(me->seq==ANIM_DIE)
	{
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
	}
	if(me->seq==ANIM_ATTACK)
	{
		if(me->frm==7 && me->reload==0)
		{
			MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->frmAdvance=128;
			a=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
			FireExactBullet(me,me->x,me->y,FIXAMT*30,Cosine(a)*10,Sine(a)*10,0,0,60,a,BLT_BIGSHELL,me->friendly);
			FireExactBullet(me,me->x,me->y,FIXAMT*30,Cosine(a-4)*10,Sine(a-4)*10,0,0,60,a,BLT_BIGSHELL,me->friendly);
			FireExactBullet(me,me->x,me->y,FIXAMT*30,Cosine(a+4)*10,Sine(a+4)*10,0,0,60,a,BLT_BIGSHELL,me->friendly);

			map->BrightTorch(me->mapx,me->mapy,16,2);
			me->reload=10;
		}
		return;
	}
	if(me->seq==ANIM_IDLE)
	{
		if(RangeToTarget(me,goodguy)<FIXAMT*500)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=20;
		}
	}
}

void AI_LightSlide(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int cx,cy;
	Guy *g;

	if(me->mind1>2)
		me->mind1-=3;

	if(me->mind1>0)
	{
		map->BrightTorch(me->mapx,me->mapy,(me->mind1/20),2);
		me->seq=ANIM_ATTACK;
		me->frm=(me->mind1/10);
		if(me->mind1>=98)
		{
			me->frm=0;
			g=AddGuy(me->x,me->y,0,MONS_SPARK,me->friendly);
			if(g)
			{
				g->item=ITM_NONE;
				g->facing=6;
				g->dx=0;
				g->dy=-2*FIXAMT;
			}
			me->mind1=0;
		}
		me->frmTimer=0;
		me->frmAdvance=0;
	}
	else
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
	}

	SpreadCharge(me);

	if(me->mind==1)	// sliding
	{
		cx=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		cy=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;

		if(me->dx>0 && me->x>=cx && (me->mapx>=map->width-1 || !Walkable(me,me->mapx+1,me->mapy,map,world)))
		{
			me->x=cx;
			me->y=cy;
			me->dx=me->dy=0;
			me->mind=0;	// rest
		}
		if(me->dx<0 && me->x<=cx && (me->mapx<=0 || !Walkable(me,me->mapx-1,me->mapy,map,world)))
		{
			me->x=cx;
			me->y=cy;
			me->dx=me->dy=0;
			me->mind=0;	// rest
		}
		if(me->dy>0 && me->y>=cy && (me->mapy>=map->height-1 || !Walkable(me,me->mapx,me->mapy+1,map,world)))
		{
			me->x=cx;
			me->y=cy;
			me->dx=me->dy=0;
			me->mind=0;	// rest
		}
		if(me->dy<0 && me->y<=cy && (me->mapy<=0 || !Walkable(me,me->mapx,me->mapy-1,map,world)))
		{
			me->x=cx;
			me->y=cy;
			me->dx=me->dy=0;
			me->mind=0;	// rest
		}
		if(me->dx==0 && me->dy==0)
			me->mind=0;
	}
}

void AI_BigHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);

	}

	me->z=FIXAMT*55;
	me->dz=0;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm<4)
				me->z-=FIXAMT*5*me->frm;
			else
				me->z-=FIXAMT*5*(7-me->frm);

			if(me->frm==3 && me->reload==0 && me->aiType==MONS_BIGHEAD1)
			{
				FireExactBullet(me,me->x,me->y,me->z,0,FIXAMT*4,FIXAMT*8,0,120,64,BLT_GRENADE,me->friendly);
				me->reload=45;
			}
			if(me->aiType==MONS_BIGHEAD3)
			{
				x=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
				FireExactBullet(me,me->x,me->y,me->z,Cosine(x)*8,Sine(x)*8,-FIXAMT/2,0,60,x,BLT_BIGSHELL,me->friendly);
				me->reload=60;
			}
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
		return;
	}

	if(me->mind==0)
	{
		if(goodguy && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch>0))
		{
			me->mind=1;
		}
	}
	if(me->mind==1)	// active
	{
		if(goodguy)
		{
			// calculate desired location (want to be above Bouapha)
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/4;
			if(me->y<y)
				me->dy+=FIXAMT/4;
			if(me->x>x)
				me->dx-=FIXAMT/4;
			if(me->y>y)
				me->dy-=FIXAMT/4;

			Clamp(&me->dx,FIXAMT*5);
			Clamp(&me->dy,FIXAMT*5);
		}

		if(RangeToTarget(me,goodguy)<300*FIXAMT && goodguy->y>me->y && me->reload==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}

	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_BigHeadBod(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->parent)
	{
		if(me->parent->hp==0 || me->parent->type==MONS_NONE)
			me->aiType=MONS_NONE;	// gone!

		me->x=me->parent->x;
		me->y=me->parent->y-1;
		me->z=0;
		if(me->parent->dx<FIXAMT)
		{
			me->facing=3;
		}
		else if(me->parent->dx>FIXAMT)
		{
			me->facing=1;
		}
		else
		{
			me->facing=2;
		}
	}
	else
		me->aiType=MONS_NONE;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_BigHead2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	me->facing=(me->mind2/32);

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);

	}

	me->z=FIXAMT*55;
	me->dz=0;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm<4)
				me->z-=FIXAMT*5*me->frm;
			else
				me->z-=FIXAMT*5*(7-me->frm);

			if(me->frm==3 && me->reload==0)
			{
				FireExactBullet(me,me->x,me->y,me->z,0,FIXAMT*4,FIXAMT*8,0,120,64,BLT_GRENADE,me->friendly);
				me->reload=45;
			}
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
		return;
	}

	if(RangeToTarget(me,goodguy)<90*FIXAMT)
	{
		goodguy->GetShot(me,goodguy,0,0,4,map,world);
		me->mind=1;
		me->mind2+=128;
	}

	if(me->mind==0)
	{
		if(goodguy && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch>0))
		{
			me->mind=Random(3)+1;
			me->mind1=0;
			me->mind2=Random(256);
		}
	}
	switch(me->mind)
	{
		case 1:	// go straight
			if(me->mind1)
			{
				me->mind2+=128;	// go the opposite way
				me->mind=2+Random(3);	// and be in one of the turny modes
			}
			else
			{
				if(Random(30)==0)
					me->mind=Random(4)+1;	// change modes sometimes
			}
			break;
		case 2:	// turning left
			if(me->mind1)
			{
				me->mind2+=128;	// go the opposite way
				me->mind=2+Random(3);	// and be in one of the turny modes
			}
			me->mind2-=4;
			if(Random(30)==0)
				me->mind=Random(4)+1;	// change modes sometimes
			break;
		case 3:	// turning right
			if(me->mind1)
			{
				me->mind2+=128;	// go the opposite way
				me->mind=2+Random(3);	// and be in one of the turny modes
			}
			me->mind2+=4;
			if(Random(30)==0)
				me->mind=Random(4)+1;	// change modes sometimes
			break;
		case 4: // chasing
			if(me->mind1)
			{
				me->mind2+=128;	// go the opposite way
				me->mind=2+Random(3);	// and be in one of the turny modes
			}
			byte a=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
			me->mind2=TurnToward(me->mind2,a,4);
			if(Random(30)==0)
				me->mind=Random(4)+1;	// change modes sometimes
			break;
	}
	me->mind1=0;
	me->dx=Cosine(me->mind2)*7;
	me->dy=Sine(me->mind2)*7;
	if(Random(3)==0)
	{
		byte a;

		a=Random(256);
		FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(a)*2,Sine(a)*2,FIXAMT*-1,0,30*6,0,BLT_BADSITFLAME,me->friendly);
	}
}

void AI_ScaredyBat(Guy *me,Map *map,world_t *world,Guy *badguy)
{
	int x;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(map->FindGuy(me->mapx,me->mapy,10,goodguy) && RangeToTarget(me,goodguy)<128*FIXAMT)
	{
		x=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
		x=(x+128)&255;	// face away!
		me->dx=Cosine(x)*8;
		me->dy=Sine(x)*8;
	}
	else if(badguy && map->FindGuy(me->mapx,me->mapy,10,badguy) && RangeToTarget(me,badguy)<128*FIXAMT)
	{
		x=AngleFrom(me->x,me->y,badguy->x,badguy->y);
		x=(x+128)&255;	// face away!
		me->dx=Cosine(x)*8;
		me->dy=Sine(x)*8;
	}
	else	// nobody too near, settle
	{
		me->dx=(me->dx*3)/4;
		me->dy=(me->dy*3)/4;
		if(me->z>0)
			me->dz-=FIXAMT;
		else
			me->dz=-FIXAMT;
	}

	if(badguy && RangeToTarget(me,badguy)<RangeToTarget(me,goodguy) && map->FindGuy(me->mapx,me->mapy,10,badguy))
		FaceGoodguy(me,badguy);
	else if(map->FindGuy(me->mapx,me->mapy,10,goodguy))
		FaceGoodguy(me,goodguy);

	if(me->dx==0 && me->dy==0)
	{
		if(me->z>FIXAMT)
		{
			if(me->frmAdvance>64)
				me->frmAdvance-=4;
			else
				me->frmAdvance+=4;
		}
		else
			if(me->frmAdvance>32)
				me->frmAdvance--;
			else if(me->frm==2)
				me->frmAdvance=0;
	}
	else
		me->frmAdvance=128;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
	}
}

void AI_Tree(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
		if(me->seq==ANIM_IDLE)
		{
			me->seq=ANIM_A1;
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=32;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==7)
		{
			for(x=0;x<16;x++)
				FireBullet(me,me->x,me->y,x*16,BLT_ENERGY,me->friendly);
			BlowUpGuy(me->x,me->y,me->x+1,me->y+1,0,1);
		}
		return;	// can't do nothin' right now
	}

	me->mind++;
	if(me->mind>=4)
	{
		me->mind=0;
		MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,100);
		x=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
		FireExactBullet(me,me->x,me->y,FIXAMT*90,Cosine(x)*6,Sine(x)*6,-FIXAMT*2,0,90,(byte)x,BLT_ENERGY,me->friendly);
	}
}

void AI_Horseradish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
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
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{

			AddBaby(me->x,me->y,FIXAMT*80,MONS_PUMPKINFLY2,me);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<48*FIXAMT)
		goodguy->GetShot(me,goodguy,0,0,2,map,world);

	if(me->mind==0)
	{
		if(goodguy && RangeToTarget(me,goodguy)<600*FIXAMT)
			me->mind=2;
	}
	else if(me->mind==2)		// when mind=0, singlemindedly drift towards Bouapha
	{
		if(goodguy)
		{
			if(me->reload==0 && Random(50)==0)
			{
				MakeSound(SND_VAMPATTK,me->x,me->y,SND_CUTOFF,800);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=160;
				me->action=ACTION_BUSY;
				me->dx=me->dy=0;
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
				me->frmAdvance=180;
			}
			if(me->mind1>0 && RangeToTarget(me,goodguy)>(64*FIXAMT))
			{
				me->mind=1;	// bumped a wall, so randomly maneuver
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
		if(me->reload==0 && Random(50)==0)
		{
			MakeSound(SND_VAMPATTK,me->x,me->y,SND_CUTOFF,800);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=160;
			me->action=ACTION_BUSY;
			FaceGoodguy(me,goodguy);
			me->mind=2;
			me->dx=me->dy=0;
			return;
		}
		if(!(me->mind2--))	// time to get a new direction
		{
			me->mind=2;	// get back on track
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
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
		me->mind=2;
		return;
	}

	me->mind1=0;
}

void AI_Sparky(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WOLFOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WOLFDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			me->frmAdvance=190;
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<FIXAMT*100 && me->reload==0)
	{
		me->reload=8;
		LightningBolt(me->x,me->y-FIXAMT*30,goodguy->x,goodguy->y-FIXAMT*30);
		goodguy->GetShot(me,goodguy,Cosine(me->mind3+64)*10,Sine(me->mind3+64)*10,4,map,world);
	}
	else if(me->reload==0)
	{
		me->reload=8;
		LightningBolt(me->x,me->y-FIXAMT*30,me->x-FIXAMT*15+Random(FIXAMT*30+1),me->y-FIXAMT*30-FIXAMT*10+Random(FIXAMT*20+1));
	}

	if(me->mind==0)
	{
		if(me->ouch || RangeToTarget(me,goodguy)<200*FIXAMT)
		{
			me->mind=1;
			me->mind3=Random(256);
			me->facing=2;
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
		return;
	}

	if(me->mind1&1)
	{
		if(me->dx>0)
			me->mind3=64+Random(128);
		else
			me->mind3=192+Random(128);
	}
	if(me->mind1&2)
	{
		if(me->dy>0)
			me->mind3=128+Random(128);
		else
			me->mind3=Random(128);
	}
	me->mind3=TurnToward(me->mind3,AngleFrom(me->x,me->y,goodguy->x,goodguy->y),4);

	me->facing=me->mind3/32;
	me->dx=Cosine(me->mind3)*5;
	me->dy=Sine(me->mind3)*5;
	me->mind1=0;

	if(me->seq==ANIM_MOVE)
		me->frmAdvance=128;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
	}
}

void AI_Grue(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;

	if(!goodguy)
		return;

	// heal in darkness
	if(map->map[me->mapx+me->mapy*map->width].templight<-3)
	{
		if(me->hp<me->maxHP)
		{
			i=me->hp;
			me->hp+=5;
			if(me->hp>me->maxHP)
				me->hp=me->maxHP;
			ShowEnemyLife(me->name,i*128/me->maxHP,me->hp*128/me->maxHP,1);
		}
	}

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
			if(Random(2))
			{
				AddBaby(me->x+((-8+Random(17))<<FIXSHIFT),
						me->y+((-8+Random(17))<<FIXSHIFT),0,MONS_SPIDER,me);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0 && goodguy)
		{
			FaceGoodguy(me,goodguy);
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,10,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1)
		me->mind1--;

	if(map->map[goodguy->mapx+goodguy->mapy*map->width].templight<-8)	// if you are in the dark, you are good eats
	{
		me->mind=2;	// dash at you
		me->mind1=3;
	}

	switch(me->mind)
	{
		case 0:	// normal heading towards you
			FaceGoodguy(me,goodguy);
			if(me->mind1==0)
			{
				me->mind=1;
				me->facing=Random(8);
				me->mind1=10+Random(40);
			}
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->frmAdvance=128;
			break;
		case 1:	// normal wandering
			if(me->mind1==0)
			{
				me->mind=0;
				me->mind1=10+Random(90);
			}
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->frmAdvance=128;
			break;
		case 2:	// dashing at you
			if(me->mind1==0)
			{
				me->mind=1;
				me->mind1=10+Random(40);
			}
			f=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
			me->facing=f/32;
			me->dx=Cosine(f)*8;
			me->dy=Sine(f)*8;
			me->frmAdvance=200;
			break;
	}

	if(!me->poison)
	{
		if(Random(100)==0)
			AddBaby(me->x+((-8+Random(17))<<FIXSHIFT),me->y+((-8+Random(17))<<FIXSHIFT),0,MONS_SPIDER,me);
		if(Random(300)==0)
			AddBaby(me->x+((-8+Random(17))<<FIXSHIFT),me->y+((-8+Random(17))<<FIXSHIFT),0,MONS_BIGSPDR,me);
	}

	if(goodguy)
	{

		x=RangeToTarget(me,goodguy);
		if(x<(128*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SPD3CHOMP,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// bite him
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=200;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}
	}

	if(me->mind!=2)	// when dashing, he ignores light, but otherwise wants to avoid it
	{
		x=(me->x+me->dx)/(TILE_WIDTH*FIXAMT);
		y=(me->y+me->dy)/(TILE_HEIGHT*FIXAMT);
		if(map->map[me->mapx+me->mapy*map->width].templight<-5 &&
			map->map[x+y*map->width].templight>=-5)	// you are in darkness now, but wouldn't be on moving
		{
			me->dx=0;
			me->dy=0;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			return;
		}
		if(map->map[me->mapx+me->mapy*map->width].templight>=-5)	// if you are in light at any time, run
		{
			me->dx=(me->dx*3)/2;
			me->dy=(me->dy*3)/2;
			me->frmAdvance=160;
		}
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Spook(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;
	
	if(me->mind4)
		me->mind4--;

	if(me->aiType==MONS_SPOOKLEY2)
	{
		if(!me->mind4)
		me->aiType=MONS_SPOOKLEY;
		// heal in ghost form
		if(me->hp<me->maxHP && me->mind2==0)
		{
			i=me->hp;
			me->hp++;
			if(me->hp>me->maxHP)
				me->hp=me->maxHP;
			ShowEnemyLife(me->name,i*128/me->maxHP,me->hp*128/me->maxHP,1);
		}
		map->DimTorch(me->mapx,me->mapy,6);
	}
	else
	{
		if(me->hp<(int)me->mind3)	// been hurt too much, ghost out
		{
			me->aiType=MONS_SPOOKLEY2;
			me->mind4=200+Random(8)*10;
			MakeSound(SND_GHOSTTPORT,me->x,me->y,SND_CUTOFF,2000);
			map->BrightTorch(me->mapx,me->mapy,16,6);
			if(me->mind3>50)
				me->mind3-=50;
			else
				me->mind3=0;
		}
	}
	if(me->dx==0 && me->dy==0)
	{
		me->dx=FIXAMT*4;
		me->dy=FIXAMT*4;
	}

	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;
	me->mind1=0;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_POLTERDIE,me->x,me->y,SND_CUTOFF,2000);
		else
			MakeSound(SND_POLTEROUCH,me->x,me->y,SND_CUTOFF,2000);
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
		FireBullet(me,me->x,me->y,x,BLT_ACID,me->friendly);
	}
	else
		me->mind2--;

	if(Random(10)==0)
		StinkySteam(me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),
					me->z+FIXAMT*40,FIXAMT*2);
	if(Random(100)==0)
		AddBaby(me->x,me->y,me->z,MONS_GHOST,me);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(goodguy)
	{
		if(me->AttackCheck(70,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy) && goodguy->poison==0)
		{
			MakeSound(SND_THINGVOMIT,me->x,me->y,SND_CUTOFF,1200);
			goodguy->poison=30;
		}
	}
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
	}
}

void AI_Speedy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte a;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WOLFOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WOLFDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			me->frmAdvance=190;
		return;	// can't do nothin' right now
	}

	if(me->mind==0 && (me->ouch || RangeToTarget(me,goodguy)<400*FIXAMT))
	{
		me->mind=1;
		me->mind3=Random(256);
		me->facing=2;
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

	if((me->mind1))	// shoot when you hit a wall
	{
		a=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
		FireExactBullet(me,me->x,me->y,me->z+FIXAMT*20,Cosine(a)*6,Sine(a)*6,0,0,60,a,BLT_BIGSHELL,me->friendly);
	}
	if(RangeToTarget(me,goodguy)<FIXAMT*30)
		goodguy->GetShot(me,goodguy,Cosine(me->mind3+64)*10,Sine(me->mind3+64)*10,4,map,world);
	if(me->mind1&1)
	{
		if(me->dx>0)
			me->mind3=64+Random(128);
		else
			me->mind3=192+Random(128);
	}
	if(me->mind1&2)
	{
		if(me->dy>0)
			me->mind3=128+Random(128);
		else
			me->mind3=Random(128);
	}
	me->facing=me->mind3/32;
	me->dx=Cosine(me->mind3)*16;
	me->dy=Sine(me->mind3)*16;
	me->mind1=0;

	if(me->seq==ANIM_MOVE)
		me->frmAdvance=256;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
	}
}

void AI_IronSkull(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_CLANG,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			// still chase, but slowly
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy3(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/4;
			if(me->y<y)
				me->dy+=FIXAMT/4;
			if(me->x>x)
				me->dx-=FIXAMT/4;
			if(me->y>y)
				me->dy-=FIXAMT/4;

			Clamp(&me->dx,FIXAMT*1);
			Clamp(&me->dy,FIXAMT*1);

			if(me->frm==1 && me->frmTimer<64)
			{
				// shoot laser
				x=me->x;
				y=me->y+FIXAMT*8;
				LaunchMegabeam(me,x,y,me->ID);
			}
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
		return;
	}

	if(me->mind==0)
	{
		if(goodguy && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch>0))
		{
			me->mind=1;
		}
	}
	if(me->mind==1)	// active
	{
		if(Random(100)==0)
			AddBaby(me->x,me->y,0,MONS_BONEHEAD,me);

		if(goodguy)
		{
			// calculate desired location (want to be above Bouapha)
			x=goodguy->x;
			y=goodguy->y-72*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/4;
			if(me->y<y)
				me->dy+=FIXAMT/4;
			if(me->x>x)
				me->dx-=FIXAMT/4;
			if(me->y>y)
				me->dy-=FIXAMT/4;

			Clamp(&me->dx,FIXAMT*3.2);
			Clamp(&me->dy,FIXAMT*2.4);
		}

		if(me->reload==8)
		{
			// left eye fires
			f=(me->facing*32-32+me->mind1)&255;
			// can't fire backwards
			if(f>128)
			{
				if(f<128+64)
					f=128;
				else
					f=0;
			}
			FireExactBullet(me,me->x-32*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
						Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_BIGSHELL,me->friendly);
		}
		if(!me->reload)
		{
			// right eye fires
			f=(me->facing*32+32-me->mind1)&255;
			// can't fire backwards
			if(f>128)
			{
				if(f<128+64)
					f=128;
				else
					f=0;
			}
			FireExactBullet(me,me->x+32*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
						Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_BIGSHELL,me->friendly);
			me->reload=16;
			me->mind1+=8;
			if(me->mind1>=64)
				me->mind1=0;
		}

		if(Random(128)==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			return;
		}
	}

	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Spatula(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int x,y;
	byte minions;

	if(!goodguy)
		return;

	minions=ArrangeBats(AngleFrom(me->x,me->y,goodguy->x,goodguy->y));

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_COUNTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_COUNTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>5 && me->frm<9 && me->reload==0)
		{
			me->reload=30;
			if(minions<8)
			{
				g=AddBaby(me->x,me->y+FIXAMT*40,FIXAMT*20,MONS_BATGUARD,me);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					RemoveGuy(g);
				else if(g)
				{
					g->mind=64;	// current angle
					g->parent=me;
				}
			}
		}

		if(me->seq==ANIM_DIE)
		{
			if(me->reload>4)
				me->reload=4;

			if(me->reload==0)
			{
				g=GetGuyOfType(MONS_BATGUARD);
				if(g!=NULL)
				{
					g->hp=1;
					g->GetShot(0,me,0,0,100,map,world);
				}
				me->reload=4;
			}
		}
		if(me->seq==ANIM_DIE && me->frm>3)
		{
			me->frmAdvance=128;
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z-Random(FIXAMT*60),FIXAMT);
		}

		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(minions<8 && me->reload==0)
	{
		MakeSound(SND_COUNTSHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		return;
	}
	if(me->mind==0)	// waiting for Bouapha
	{
		if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch>0)
		{
			MakeSound(SND_COUNTHELLO,me->x,me->y,SND_CUTOFF,1200);
			me->mind=1;
		}
		me->mind1=60;
	}
	if(me->mind==1)	// wandering mode
	{
		// calculate desired location (want to be above Bouapha)
		x=goodguy->x;
		y=goodguy->y;

		FaceGoodguy3(me,goodguy);
		if(me->x<x)
			me->dx+=FIXAMT/8;
		if(me->y<y)
			me->dy+=FIXAMT/8;
		if(me->x>x)
			me->dx-=FIXAMT/8;
		if(me->y>y)
			me->dy-=FIXAMT/8;

		Clamp(&me->dx,FIXAMT*3);
		Clamp(&me->dy,FIXAMT*4);
	}
	if(me->mind1==0)
	{
		me->mind1=120;
		me->mind2=30;	// while non-zero, all bats constantly shoot
	}
	else
		me->mind1--;

	if(me->mind2)
		me->mind2--;
}

void AI_BatGuard(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_BATDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm<6 && me->reload==0)
		{
			FireBullet(me,me->x,me->y,me->mind,BLT_ENERGY_BOUNCE,me->friendly);
			MakeSound(SND_MATTIESHOOT,me->x,me->y,SND_CUTOFF,200);
			me->reload=2;
		}
		if(me->seq==ANIM_A1  &&me->frm==5 && me->parent && me->parent->mind2!=0)
			me->frm=4;	// stay there until done

		return;	// can't do nothin' right now
	}

	if(me->AttackCheck(16,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
	{
		goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,4,map,world);
	}
	if(me->parent && me->parent->mind2>0)
	{
		MakeSound(SND_BATDIVE,me->x,me->y,SND_CUTOFF,200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		return;
	}
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=110;
	}
}

void DoMove2(Guy *me,int move,int frmAdv,byte busy,int dx,int dy)
{
	me->seq=move;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=frmAdv;
	if(busy)
		me->action=ACTION_BUSY;
	else
		me->action=ACTION_IDLE;
	me->dx=dx;
	me->dy=dy;
}