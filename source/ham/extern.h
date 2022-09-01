#ifndef HAM_EXTERN_H
#define HAM_EXTERN_H

struct SDL_RWops;
struct HamSandwichMetadata;

// Callbacks that games should provide to Ham.
extern struct HamExtern
{
	const HamSandwichMetadata* (*GetHamSandwichMetadata)();

	// ---- hammusic ----
	// Should call PlaySongFile or StopSong, or the current song will loop.
	void (*ChooseNextSong)();
	bool (*ConfigMusicEnabled)();

	// ---- jamulsound ----
	bool (*ConfigSoundEnabled)();
	int (*ConfigNumSounds)();
	SDL_RWops* (*SoundLoadOverride)(int);

	// ---- mgldraw ----
	void (*SetGameIdle)(bool);
} g_HamExtern;

#define HAM_EXTERN_FULFILL \
	extern const HamSandwichMetadata* GetHamSandwichMetadata(void); g_HamExtern.GetHamSandwichMetadata = &GetHamSandwichMetadata; \
	extern void ChooseNextSong(void); g_HamExtern.ChooseNextSong = &ChooseNextSong; \
	extern bool ConfigMusicEnabled(void); g_HamExtern.ConfigMusicEnabled = &ConfigMusicEnabled; \
	extern bool ConfigSoundEnabled(void); g_HamExtern.ConfigSoundEnabled = &ConfigSoundEnabled; \
	extern int ConfigNumSounds(void); g_HamExtern.ConfigNumSounds = &ConfigNumSounds; \
	extern SDL_RWops* SoundLoadOverride(int); g_HamExtern.SoundLoadOverride = &SoundLoadOverride; \
	extern void SetGameIdle(bool); g_HamExtern.SetGameIdle = &SetGameIdle;

#endif  // HAM_EXTERN_H
