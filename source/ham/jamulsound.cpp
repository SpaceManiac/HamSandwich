#include "jamulsound.h"
#include <stdio.h>
#include <limits.h>
#include <memory>
#include <vector>
#include <utility>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "hammusic.h"
#include "log.h"
#include "audiofx.h"
#include "appdata.h"
#include "extern.h"
#include "owned_mixer.h"
#include "string_extras.h"

struct Sound
{
	owned::MIX_Audio sample;
};

struct Channel
{
	int soundNum;
	int priority;
	owned::MIX_Track track;
	owned::MIX_Audio sample;
};

namespace
{
	bool soundIsOn = false;
	bool soundEnabled = true;
	int configNumSounds = 32;
	bool musicEnabled = true;

	int sndVolume = 128;
	int musVolume = 255;

	owned::MIX_Mixer mixer;
	owned::MIX_Track musicTrack;
	owned::MIX_Audio musicAudio;

	std::vector<Sound> soundList;

	int NUM_SOUNDS = 0;
	std::vector<Channel> schannel;
}  // namespace

void SetJamulSoundEnabled(bool enable, int numSounds)
{
	soundEnabled = enable;
	configNumSounds = numSounds;
}

bool JamulSoundInit(int numBuffers)
{
	int i;

	if (!soundEnabled)
	{
		LogDebug("sound disabled in config");
		return false;
	}
	if (!SDL_Init(SDL_INIT_AUDIO))
	{
		LogError("SDL_Init(AUDIO): %s", SDL_GetError());
		return false;
	}
	SDL_setenv_unsafe("TIMIDITY_CFG", "assets/supreme/midi/timidity.cfg", true);
	if (!MIX_Init())
	{
		LogError("Mix_Init: %s", SDL_GetError());
	}

	mixer = owned::MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!mixer)
	{
		LogError("MIX_CreateMixerDevice: %s", SDL_GetError());
		return false;
	}

	SDL_AudioSpec spec;
	if (MIX_GetMixerFormat(mixer.get(), &spec))
	{
		LogDebug("audio format: freq=%d, channels=%d, format=0x%x %s\n", spec.freq, spec.channels, spec.format, SDL_GetAudioFormatName(spec.format));
	}
	else
	{
		LogError("MIX_GetMixerFormat: %s", SDL_GetError());
	}

	musicTrack = owned::MIX_CreateTrack(mixer.get());

	NUM_SOUNDS = configNumSounds;  // Only copied over here to avoid problems if SetJamulSoundEnabled is called after init.

	soundList.clear();
	soundList.reserve(numBuffers);
	schannel.resize(NUM_SOUNDS);
	for(i=0;i<NUM_SOUNDS;i++)
	{
		schannel[i].priority=INT_MIN;
		schannel[i].soundNum=-1;
		schannel[i].track = owned::MIX_CreateTrack(mixer.get());
	}
	return soundIsOn = true;
}

void JamulSoundExit(void)
{
	if(soundIsOn)
	{
		soundIsOn = false;
		JamulSoundPurge();
		StopSong();
		schannel.clear();
		soundList.clear();
		musicTrack.reset();
		mixer.reset();
	}
}

bool SoundIsAvailable()
{
	return soundIsOn;
}

void JamulSoundUpdate(void)
{
	int i;

	if(!soundIsOn)
		return;

	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].soundNum>-1)
		{
			if(!MIX_TrackPlaying(schannel[i].track.get()))
			{
				schannel[i].soundNum=-1;
				schannel[i].priority=INT_MIN;
			}
		}
	}
}

// pan: -128 (left) to 127 (right)
// vol: -255 (silent) to 0 (no attenuation)
bool JamulSoundPlay(int which, long pan, long vol, int playFlags, int priority)
{
	char s[32];
	int i,chosen,lowpriority;

	if(!soundIsOn)
		return false;

	JamulSoundUpdate();

	vol = (vol + 255) * ((playFlags & SND_MUSICVOLUME) ? musVolume : sndVolume) / 255;
	pan += 128;
	if (pan < 0)
		pan = 0;
	if (pan > 255)
		pan = 255;

	if (vol < 0)
		return false;

	size_t wantSize = which + 1;
	if (soundList.size() < wantSize)
	{
		soundList.resize(wantSize);
	}

	if(soundList[which].sample==NULL)
	{
		// See if sound loading is overridden for this sound...
		owned::SDL_IOStream rw = g_HamExtern.SoundLoadOverride ? g_HamExtern.SoundLoadOverride(which) : nullptr;
		if (!rw)
		{
			// If not, try to load it from a file instead
			ham_sprintf(s,"sound/snd%03d.wav",which);
			rw = AppdataOpen(s);
			if (!rw)
			{
				return false;
			}
		}

		// Now try to load it
		soundList[which].sample = owned::MIX_LoadAudio_IO(mixer.get(), rw.get(), true);
		if(soundList[which].sample==NULL)
		{
			LogError("LoadAudio(%d): %s", which, SDL_GetError());
			return false;
		}
	}

	if(playFlags&SND_ONE)
	{
		for(i=0;i<NUM_SOUNDS;i++)
			if(schannel[i].soundNum==which)
			{
				// JamulSoundUpdate above ensures that sounds that finish are
				// erased from schannel.
				if (playFlags&SND_CUTOFF)
				{
					MIX_StopTrack(schannel[i].track.get(), 0);
					schannel[i].soundNum=-1;
					schannel[i].priority=INT_MIN;
				}
				else
					return false;
			}
	}

	if(playFlags & SND_MAXPRIORITY)
		priority = MAX_SNDPRIORITY;

	chosen = -1;
	lowpriority = priority;
	// see if there are any spots of lower priority,
	for (i = 0; i < NUM_SOUNDS; i++)
	{
		if (schannel[i].soundNum == -1)
		{
			chosen = i;
			break; // can't beat that
		}
		else if (schannel[i].priority <= lowpriority || (chosen == -1 && (playFlags & SND_CUTOFF) && schannel[i].soundNum == which))
		{
			chosen = i;
			lowpriority = schannel[i].priority;
		}
	}

	if(chosen==-1)	// no sounds of lower priority to kick out, give up
		return false;

	MIX_Track* track = schannel[chosen].track.get();
	MIX_Audio* playing = soundList[which].sample.get();

	// if you're replacing a sound, stop it first
	if(schannel[chosen].soundNum!=-1)
		MIX_StopTrack(track, 0);

	float freqRatio = 1.f;
	if (playFlags & SND_RANDOM)
	{
		freqRatio = 1.f - 0.2f + 0.4f * SDL_randf();
	}
#if 0 // SDL3 TODO
	if (playFlags & SND_BACKWARDS)
	{
		schannel[chosen].sample = FxBackwards(playing);
		playing = schannel[chosen].sample.get();
	}
#endif
	if (playFlags & SND_DOUBLESPEED)
	{
		freqRatio *= 2.f;
	}

	SDL_PropertiesID options = SDL_CreateProperties();
	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, (playFlags & SND_LOOPING) ? -1 : 0);
	MIX_SetTrackAudio(track, playing);
	MIX_SetTrackGain(track, vol / 255.f);
	MIX_StereoGains gains = { .left = 1.f - pan / 256.f, .right = pan / 256.f };
	MIX_SetTrackStereo(track, &gains);
	MIX_SetTrackFrequencyRatio(track, freqRatio);
	MIX_PlayTrack(track, options);
	SDL_DestroyProperties(options);

	schannel[chosen].soundNum=which;
	schannel[chosen].priority=priority;
	return true;
}

bool JamulSoundStop(int which)
{
	int i;

	if(!soundIsOn)
		return true;

	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].soundNum==which)
		{
			MIX_StopTrack(schannel[i].track.get(), 0);
			schannel[i].soundNum=-1;
			schannel[i].priority=INT_MIN;
		}
	}

	return true;
}

// now here is all the big sound manager stuff, that allows multiple sounds at once
void JamulSoundPurge(void)
{
	if (!soundIsOn)
		return;

	for (int i = 0; i < NUM_SOUNDS; i++)
	{
		if (schannel[i].soundNum != -1)
			MIX_StopTrack(schannel[i].track.get(), 0);
		schannel[i].soundNum = -1;
		schannel[i].priority = INT_MIN;
		schannel[i].sample.reset();
	}

	for (size_t i = 0; i < soundList.size(); i++)
	{
		if (soundList[i].sample)
		{
			soundList[i].sample.reset();
		}
	}
}

void GoPlaySound(int num,long pan,long vol,int flags,int priority)
{
	JamulSoundPlay(num, pan, vol, flags, priority);
}

void JamulSoundVolume(int v)
{
	sndVolume=v;
}

void JamulSoundChangeSoundVolume(int snd, int volume)
{
	int v = (volume + 255) * sndVolume / 255;

	for (int i = 0; i < NUM_SOUNDS; i++)
	{
		if (schannel[i].soundNum == snd)
		{
			MIX_SetTrackGain(schannel[i].track.get(), v / 255.f);
		}
	}
}

// ----------------------------------------------------------------------------

void SetHamMusicEnabled(bool enabled)
{
	musicEnabled = enabled;
}

void UpdateMusic()
{
	if (musicEnabled && musicAudio && !MIX_TrackPlaying(musicTrack.get()))
	{
		MIX_PlayTrack(musicTrack.get(), 0);
		if (g_HamExtern.ChooseNextSong)
			g_HamExtern.ChooseNextSong();
	}
}

void SetMusicVolume(int vol)
{
	musVolume = vol;
	MIX_SetTrackGain(musicTrack.get(), musVolume / 255.f);
}

int GetMusicVolume()
{
	return musVolume;
}

void PlaySongFile(const char* fullname)
{
	if (!musicEnabled)
		return;

	StopSong();

	owned::SDL_IOStream rw = AppdataOpen(fullname);
	if (!rw)
	{
		return;
	}

	musicAudio = owned::MIX_LoadAudio_IO(mixer.get(), std::move(rw), false);
	if (!musicAudio)
	{
		LogError("Mix_LoadMUS(%s): %s", fullname, SDL_GetError());
		return;
	}

	MIX_SetTrackGain(musicTrack.get(), musVolume / 255.f);
	MIX_SetTrackAudio(musicTrack.get(), musicAudio.get());
	if (!MIX_PlayTrack(musicTrack.get(), 0))
	{
		LogError("MIX_PlayTrack(%s): %s", fullname, SDL_GetError());
	}
	UpdateMusic();
}

void StopSong()
{
	if (musicAudio)
	{
		MIX_StopTrack(musicTrack.get(), 0);
		musicAudio.reset();
	}
}

bool IsSongPlaying()
{
	return musicAudio && MIX_TrackPlaying(musicTrack.get());
}
