#include "common.h"
#include "sound.h"

static void loadSounds(void);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;

void initSounds(void)
{
	memset(sounds, 0, sizeof(Mix_Chunk *) * SND_MAX);

	music = NULL;

	loadSounds();
}

void loadMusic(const char *filename)
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(filename);
}

void playMusic(int loop)
{
	Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void pauseMusic() {
	Mix_PauseMusic();
}

void resumeMusic() {
	Mix_ResumeMusic();
}

void playSound(int id, int channel)
{
	Mix_PlayChannel(channel, sounds[id], 0);
}

void playBeamSound(int id, int channel, bool loop)
{
    if (loop) {
        Mix_PlayChannel(channel, sounds[id], -1); // -1 indicates looping indefinitely
    } else {
        Mix_PlayChannel(channel, sounds[id], 0); // 0 plays the sound once
    }
}

static void loadSounds(void)
{
	sounds[SND_PLAYER_FIRE] = Mix_LoadWAV("sound/playerFire.ogg");
	sounds[SND_PLAYER_FIRE_LASER] = Mix_LoadWAV("sound/laser.ogg");
	sounds[SND_ALIEN_FIRE] = Mix_LoadWAV("sound/alienFire.ogg");
	sounds[SND_PLAYER_DIE] = Mix_LoadWAV("sound/playerDies.ogg");
	sounds[SND_SHIP_HIT] = Mix_LoadWAV("sound/shipHit.ogg");
	sounds[SND_ALIEN_DIE] = Mix_LoadWAV("sound/alienDies.ogg");
	sounds[SND_POINTS] = Mix_LoadWAV("sound/342749__rhodesmas__notification-01.ogg");
	sounds[SND_HYPER_DRIVE] = Mix_LoadWAV("sound/hyperDrive.ogg");
	sounds[SND_SHIP_DOWN] = Mix_LoadWAV("sound/shipDown.ogg");
	sounds[SND_LASER_BEAM] = Mix_LoadWAV("sound/laserBeam.ogg");

	// Set individual volume levels for each sound
	Mix_VolumeChunk(sounds[SND_PLAYER_FIRE], 40);
	Mix_VolumeChunk(sounds[SND_PLAYER_FIRE_LASER], 20);
	Mix_VolumeChunk(sounds[SND_LASER_BEAM], 40);
	Mix_VolumeChunk(sounds[SND_ALIEN_FIRE], 20);
	Mix_VolumeChunk(sounds[SND_PLAYER_DIE], 128);
	Mix_VolumeChunk(sounds[SND_SHIP_HIT], 20);
	Mix_VolumeChunk(sounds[SND_ALIEN_DIE], 15);
	Mix_VolumeChunk(sounds[SND_POINTS], 20);
	Mix_VolumeChunk(sounds[SND_HYPER_DRIVE], 128);
	Mix_VolumeChunk(sounds[SND_SHIP_DOWN], 128);
}

void setMusicVolume(int volume)
{
	Mix_VolumeMusic(volume);
}