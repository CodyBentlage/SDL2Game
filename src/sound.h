void initSounds(void);
void loadMusic(const char *filename);
void playMusic(int loop);
void playSound(int id, int channel);
void playAlarm(int id, int channel);
void playBeamSound(int id, int channel, bool loop);
void setMusicVolume(int volume);
void pauseMusic(void);
void resumeMusic(void);