#pragma once
#include "GameConfig.h"

// ==============================================================
// MODULE XU LY AM THANH
// ==============================================================

inline void playBGM() { mciSendString("close bgm", NULL, 0, NULL); mciSendString("open \"bgm.mp3\" type mpegvideo alias bgm", NULL, 0, NULL); mciSendString("play bgm repeat", NULL, 0, NULL); }
inline void stopBGM() { mciSendString("stop bgm", NULL, 0, NULL); }

inline void playMenuBGM() { mciSendString("close menubgm", NULL, 0, NULL); mciSendString("open \"menu.mp3\" type mpegvideo alias menubgm", NULL, 0, NULL); mciSendString("play menubgm repeat", NULL, 0, NULL); }
inline void stopMenuBGM() { mciSendString("stop menubgm", NULL, 0, NULL); }

inline void playTutorialBGM() { mciSendString("close tutorialbgm", NULL, 0, NULL); mciSendString("open \"tutorial.mp3\" type mpegvideo alias tutorialbgm", NULL, 0, NULL); mciSendString("play tutorialbgm repeat", NULL, 0, NULL); }
inline void stopTutorialBGM() { mciSendString("stop tutorialbgm", NULL, 0, NULL); }

inline void playWin() { mciSendString("close win", NULL, 0, NULL); mciSendString("open \"win.mp3\" type mpegvideo alias win", NULL, 0, NULL); mciSendString("play win from 0", NULL, 0, NULL); }
inline void stopWin() { mciSendString("stop win", NULL, 0, NULL); }

inline void playLose() { mciSendString("close lose", NULL, 0, NULL); mciSendString("open \"lose.mp3\" type mpegvideo alias lose", NULL, 0, NULL); mciSendString("play lose from 0", NULL, 0, NULL); }
inline void stopLose() { mciSendString("stop lose", NULL, 0, NULL); }

inline void playShoot() { mciSendString("close shoot", NULL, 0, NULL); mciSendString("open \"shoot.mp3\" type mpegvideo alias shoot", NULL, 0, NULL); mciSendString("play shoot from 0", NULL, 0, NULL); }

inline void playExplode() { mciSendString("close explode", NULL, 0, NULL); mciSendString("open \"explode.mp3\" type mpegvideo alias explode", NULL, 0, NULL); mciSendString("play explode from 0", NULL, 0, NULL); }

inline void playItem() { mciSendString("close item", NULL, 0, NULL); mciSendString("open \"item.mp3\" type mpegvideo alias item", NULL, 0, NULL); mciSendString("play item from 0", NULL, 0, NULL); }
