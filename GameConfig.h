#pragma once
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h> 
#include <mmsystem.h> // Thu vien am thanh

#define WIDTH 1280
#define HEIGHT 720
#define PI 3.14159265
#define GRID_SIZE 35
#define MAZE_X 220
#define MAZE_Y 50
#define MAX_BULLETS 20

// ==============================================================
// 1. CAU TRUC DU LIEU
// ==============================================================

struct Bullet { float x, y, angle; int active, isPlayer; };

struct Entity { 
    float x, y, angle; 
    int health, maxHealth, speed_level; 
    long lastShotTime; 
    long rapidFireEndTime; 
    long shieldEndTime;    
    long critEndTime;      
};

struct Explosion { float x, y; int timer, maxTimer, active, color; };
struct Point { int x, y; };

// Khai bao bien toan cuc (extern) de dung chung giua cac module
extern Entity player;
extern Entity boss;
extern Bullet bullets[MAX_BULLETS];
extern Explosion explosions[10];
extern int maze[15][24];
extern long lastItemSpawnTime;
