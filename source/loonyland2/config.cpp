#include "config.h"
#include <stdio.h>
#include <stdlib.h>

config_t config;

void LoadConfig(void)
{
	FILE *f;
	char buf[127];
	byte p,i;
	int n;

	config.music=1;
	config.sound=1;
	config.numSounds=32;
	config.numBullets=512;
	config.numGuys=256;
	config.numParticles=512;
	config.joystick=1;
	config.hiscores=1;
	config.camera=1;
	config.shading=1;

	f=fopen("config.txt","rt");
	if(!f)
	{
		return;
	}
	else
	{
		while(fgets(buf,127,f))
		{
			p=0;
			for(i=0;i<strlen(buf);i++)
				if(buf[i]=='=')
				{
					p=i;
					break;
				}
			if(p==0)
				continue;	// this line is useless
			buf[p]='\0';
			n=atoi(&buf[p+1]);
			if(!strcmp(buf,"music"))
			{
				config.music=(byte)n;
			}
			if(!strcmp(buf,"sound"))
			{
				config.sound=(byte)n;
			}
			if(!strcmp(buf,"joystick"))
			{
				config.joystick=(byte)n;
			}
			if(!strcmp(buf,"bullets"))
			{
				if(n<128)
					n=128;
				config.numBullets=n;
			}
			if(!strcmp(buf,"enemies"))
			{
				if(n<128)
					n=128;
				config.numGuys=n;
			}
			if(!strcmp(buf,"particles"))
			{
				if(n<16)
					n=16;
				config.numParticles=n;
			}
			if(!strcmp(buf,"sounds"))
			{
				if(n<4)
					n=4;
				config.numSounds=n;
			}
			if(!strcmp(buf,"hiscores"))
			{
				config.hiscores=(byte)n;
			}
			if(!strcmp(buf,"camera"))
			{
				config.camera=(byte)n;
			}
			if(!strcmp(buf,"shading"))
			{
				config.shading=(byte)n;
			}
		}
		fclose(f);
	}
}

bool ConfigSoundEnabled()
{
	return config.sound;
}

int ConfigNumSounds()
{
	return config.numSounds;
}
