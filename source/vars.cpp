#include "vars.h"
#include "special.h"
#include "player.h"
#include "progress.h"

#define VA_SET	0
#define VA_ADD	1
#define VA_SUB	2
#define VA_MUL	3
#define VA_DIV	4
#define VA_AND	5
#define VA_OR	6
#define VA_XOR  7
#define VA_SHL	8
#define VA_SHR  9
#define VA_RND	10
#define VA_TILE 11
#define VA_MOD	12

static int tmpVar;

char *VarName(int v)
{
	static char s[6];

	if(v<0 || v>=VAR_LOCAL+NUM_VARS)
	{
		strcpy(s,"DUD");
		return s;
	}
	if(v<VAR_LOCAL)
		sprintf(s,"G%d",v);
	else
		sprintf(s,"V%d",v-VAR_LOCAL);

	return s;
}

void SetVar(byte v,int value)
{
	if(v==255)
		tmpVar=value;
	else if(v<NUM_VARS)
		player.worldProg->var[v]=value;
	else if(v<VAR_LOCAL+NUM_VARS)
		player.var[v-VAR_LOCAL]=value;
}

int GetVar(byte v)
{
	if(v==255)
		return tmpVar;
	else if(v<NUM_VARS)
		return player.worldProg->var[v];
	else if(v<VAR_LOCAL+NUM_VARS)
		return player.var[v-VAR_LOCAL];
	else
		return 0;
}

int GetSpecialVar(char c)
{
	if(c>='a' && c<='z')
		c+='A'-'a';
	if(!goodguy)
		return 0;

	switch(c)
	{
		case 'X':
			return goodguy->mapx;
			break;
		case 'Y':
			return goodguy->mapy;
			break;
		case 'R':
			return player.rage/256;
			break;
		case 'L':
			return goodguy->hp;
			break;
		case 'C':
			return player.coins;
			break;
		case 'B':
			return player.brains;
			break;
		case 'K':
			return player.candles;
			break;
		case 'P':
			return player.worldProg->percentage;
			break;
	}
	return 0;
}

int GetSpecialVarT(char c)
{
	if(c>='a' && c<='z')
		c+='A'-'a';
	if(!TaggedMonster())
		return 0;

	switch(c)
	{
		case 'X':
			return TaggedMonster()->mapx;
			break;
		case 'Y':
			return TaggedMonster()->mapy;
			break;
		case 'L':
			return TaggedMonster()->hp;
			break;
	}
	return 0;
}

byte CompareVar(byte v,byte flags,int value)
{
	int c;

	c=GetVar(v);

	if(c==value)
		return 1;
	else if(c<value && (flags&TF_LESS))
		return 1;
	else if(c>value && (flags&TF_MORE))
		return 1;
	else
		return 0;
}

static byte error;

int DoTheMath(int start,byte action,int num)
{
	switch(action)
	{
		case VA_SET:
			return num;
			break;
		case VA_ADD:
			return start+num;
			break;
		case VA_SUB:
			return start-num;
			break;
		case VA_MUL:
			return start*num;
			break;
		case VA_MOD:
			if(num!=0)
				return start%num;
			else
			{
				error=1;
				return 0;
			}
			break;
		case VA_DIV:
			if(num!=0)
				return start/num;
			else
			{
				error=1;
				return 0;
			}
			break;
		case VA_AND:
			return start&num;
			break;
		case VA_OR:
			return start|num;
			break;
		case VA_XOR:
			return start^num;
			break;
		case VA_SHL:
			return start<<num;
			break;
		case VA_SHR:
			return start>>num;
			break;
		case VA_RND:
			if((num-start)>=0)
				return Random((num-start)+1)+start;
			else
			{
				error=1;
				return 0;
			}
			break;
		case VA_TILE:
			if(start>=0 && start<curMap->width && num>=0 && num<curMap->height)
				return curMap->map[start+num*curMap->width].floor;
			else
				return 0;
			break;
	}
	return start;	// if the action is invalid somehow
}

byte VarMath(byte finalV,char *func)
{
	char tmp[32],c;
	byte pos,i,j,action,operatorOk;
	int num,result;

	strcpy(tmp,func);	// make a copy

	pos=0;
	action=VA_SET;
	operatorOk=0;
	error=0;

	while(1)
	{
		if(tmp[pos]>='0' && tmp[pos]<='9' && !operatorOk)	// numbers
		{
			for(i=pos;i<32;i++)
				if(tmp[i]<'0' || tmp[i]>'9')	// find where the number ends
				{
					j=i;
					break;
				}
			c=tmp[j];
			tmp[j]='\0';
			num=atoi(&tmp[pos]);
			tmp[j]=c;
			pos=j;

			result=DoTheMath(result,action,num);
			operatorOk=1;
		}
		else if(tmp[pos]=='-' && !operatorOk)
		{
			for(i=pos+1;i<32;i++)
				if(tmp[i]<'0' || tmp[i]>'9')	// find where the number ends
				{
					j=i;
					break;
				}
			c=tmp[j];
			tmp[j]='\0';
			num=atoi(&tmp[pos]);
			tmp[j]=c;
			pos=j;

			result=DoTheMath(result,action,num);
			operatorOk=1;
		}
		else if(tmp[pos]=='-' && operatorOk)
		{
			action=VA_SUB;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='+' && operatorOk)
		{
			action=VA_ADD;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='*' && operatorOk)
		{
			action=VA_MUL;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='/' && operatorOk)
		{
			action=VA_DIV;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='%' && operatorOk)
		{
			action=VA_MOD;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='&' && operatorOk)
		{
			action=VA_AND;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='|' && operatorOk)
		{
			action=VA_OR;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='^' && operatorOk)
		{
			action=VA_XOR;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='<' && operatorOk)
		{
			action=VA_SHL;
			operatorOk=0;
			pos++;
		}
		else if(tmp[pos]=='>' && operatorOk)
		{
			action=VA_SHR;
			operatorOk=0;
			pos++;
		}
		else if((tmp[pos]=='R' || tmp[pos]=='r') && operatorOk)
		{
			action=VA_RND;
			operatorOk=0;
			pos++;
		}
		else if((tmp[pos]=='T' || tmp[pos]=='t') && operatorOk)
		{
			action=VA_TILE;
			operatorOk=0;
			pos++;
		}
		else if((tmp[pos]=='g' || tmp[pos]=='G') && !operatorOk)
		{
			num=GetVar(tmp[pos+1]-'0');

			result=DoTheMath(result,action,num);
			operatorOk=1;
			pos+=2;
		}
		else if((tmp[pos]=='v' || tmp[pos]=='V') && !operatorOk)
		{
			num=GetVar(tmp[pos+1]-'0'+VAR_LOCAL);

			result=DoTheMath(result,action,num);
			operatorOk=1;
			pos+=2;
		}
		else if((tmp[pos]=='p' || tmp[pos]=='P') && !operatorOk)
		{
			num=GetSpecialVar(tmp[pos+1]);

			result=DoTheMath(result,action,num);
			operatorOk=1;
			pos+=2;
		}
		else if((tmp[pos]=='t' || tmp[pos]=='T') && !operatorOk)
		{
			num=GetSpecialVarT(tmp[pos+1]);

			result=DoTheMath(result,action,num);
			operatorOk=1;
			pos+=2;
		}
		else if(tmp[pos]==' ')
			pos++;
		else if(tmp[pos]=='\0' && operatorOk)
		{
			SetVar(finalV,result);
			return 1;	// all done
		}
		else
			return 0;	// the equation has an error
		if(error==1)
			return 0;
	}
	return 0;	// unreachable line
}
