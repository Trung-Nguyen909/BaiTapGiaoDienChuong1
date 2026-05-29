#include "GameConfig.h"
#include "AudioSystem.h"
#include "Algorithms.h"

// Khoi tao cac bien toan cuc dung chung (Duy nhat o day)
Entity player;
Entity boss;
Bullet bullets[MAX_BULLETS];
Explosion explosions[10];
int maze[15][24];
long lastItemSpawnTime;

// ==============================================================
// CAC HAM GIAO DIEN UI & DRAWING
// ==============================================================

void drawIcon(int type, int cx, int cy) {
    int s = 11; 
    if (type == 21) { 
        setfillstyle(SOLID_FILL, YELLOW); setcolor(BLACK); 
        bar(cx + 2, cy - 5, cx + 8, cy + 8); rectangle(cx + 2, cy - 5, cx + 8, cy + 8);
        int pts1[] = {cx + 2, cy - 5, cx + 8, cy - 5, cx + 5, cy - 13, cx + 2, cy - 5}; fillpoly(4, pts1);
        bar(cx - 6, cy + 2, cx + 0, cy + 15); rectangle(cx - 6, cy + 2, cx + 0, cy + 15);
        int pts2[] = {cx - 6, cy + 2, cx + 0, cy + 2, cx - 3, cy - 6, cx - 6, cy + 2}; fillpoly(4, pts2);
    } 
    else if (type == 22) { 
        setcolor(CYAN);
        myLine(cx - s, cy - s, cx + s, cy - s, CYAN);
        myLine(cx + s, cy - s, cx + s, cy + 2, CYAN);
        myLine(cx + s, cy + 2, cx, cy + s + 4, CYAN);
        myLine(cx, cy + s + 4, cx - s, cy + 2, CYAN);
        myLine(cx - s, cy + 2, cx - s, cy - s, CYAN);
        myFloodFill(cx, cy, LIGHTBLUE, CYAN);
        setfillstyle(SOLID_FILL, WHITE);
        bar(cx - 2, cy - s + 3, cx + 2, cy + s - 2);
        bar(cx - s + 3, cy - 2, cx + s - 3, cy + 2);
    } 
    else if (type == 23) { 
        setcolor(LIGHTRED); setfillstyle(SOLID_FILL, LIGHTRED);
        fillellipse(cx - s/2, cy - s/4, s/2 + 2, s/2 + 2);
        fillellipse(cx + s/2, cy - s/4, s/2 + 2, s/2 + 2);
        int pts[] = {cx - s - 2, cy - s/4, cx + s + 2, cy - s/4, cx, cy + s + 3, cx - s - 2, cy - s/4};
        fillpoly(4, pts);
    } 
    else if (type == 24) { 
        setfillstyle(SOLID_FILL, LIGHTRED); setcolor(YELLOW); 
        bar(cx - 6, cy - 3, cx + 6, cy + 11); rectangle(cx - 6, cy - 3, cx + 6, cy + 11);
        int pts[] = {cx - 6, cy - 3, cx + 6, cy - 3, cx, cy - 14, cx - 6, cy - 3}; fillpoly(4, pts); 
        myLine(cx - 6, cy + 5, cx + 6, cy + 5, YELLOW);
        myLine(cx - 6, cy + 8, cx + 6, cy + 8, YELLOW);
    }
}

void spawnExplosion(float x, float y, int color) {
    for (int i = 0; i < 10; i++) {
        if (!explosions[i].active) {
            explosions[i].x = x; explosions[i].y = y;
            explosions[i].timer = 1; explosions[i].maxTimer = 12;
            explosions[i].color = color; explosions[i].active = 1; 
            playExplode(); 
            break;
        }
    }
}

void drawLeftPanel() {
    setfillstyle(SOLID_FILL, DARKGRAY); bar(10, MAZE_Y, 205, MAZE_Y + 450);
    setcolor(YELLOW); rectangle(10, MAZE_Y, 205, MAZE_Y + 450);
    setfillstyle(SOLID_FILL, BLUE); bar(11, MAZE_Y + 1, 204, MAZE_Y + 40);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2); setcolor(WHITE); outtextxy(40, MAZE_Y + 10, (char*)"THONG TIN");

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2); setcolor(YELLOW); outtextxy(20, MAZE_Y + 60, (char*)"HP CUA BAN:");
    setfillstyle(SOLID_FILL, RED); bar(20, MAZE_Y + 90, 195, MAZE_Y + 110);
    int w = (int)((float)player.health / player.maxHealth * 175); if(w < 0) w = 0;
    setfillstyle(SOLID_FILL, GREEN); bar(20, MAZE_Y + 90, 20 + w, MAZE_Y + 110);
    setcolor(WHITE); rectangle(20, MAZE_Y + 90, 195, MAZE_Y + 110);
    char hpTxt[20]; sprintf(hpTxt, "%d/%d", player.health, player.maxHealth);
    settextstyle(SMALL_FONT, HORIZ_DIR, 4); outtextxy(80, MAZE_Y + 93, hpTxt); 

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2); setcolor(YELLOW); outtextxy(20, MAZE_Y + 140, (char*)"TOC DO:");
    char spdTxt[50];
    if (player.speed_level == 0) { setcolor(LIGHTGRAY); sprintf(spdTxt, "0 - DUNG"); }
    else if (player.speed_level == 1) { setcolor(LIGHTBLUE); sprintf(spdTxt, "1 - CHAM"); }
    else if (player.speed_level == 2) { setcolor(LIGHTGREEN); sprintf(spdTxt, "2 - CHUAN"); }
    else if (player.speed_level == 3) { setcolor(LIGHTRED); sprintf(spdTxt, "3 - NHANH"); }
    outtextxy(20, MAZE_Y + 170, spdTxt);

    setcolor(YELLOW); outtextxy(20, MAZE_Y + 220, (char*)"DAO CU:");
    long currentTime = clock();
    int rapidLeft = (player.rapidFireEndTime > currentTime) ? (player.rapidFireEndTime - currentTime) / 1000 : 0;
    int shieldLeft = (player.shieldEndTime > currentTime) ? (player.shieldEndTime - currentTime) / 1000 : 0;
    int critLeft = (player.critEndTime > currentTime) ? (player.critEndTime - currentTime) / 1000 : 0;

    if (rapidLeft == 0 && shieldLeft == 0 && critLeft == 0) { 
        setcolor(WHITE); outtextxy(20, MAZE_Y + 250, (char*)"- Khong co -"); 
    } else {
        int yOffset = MAZE_Y + 250; char buffTxt[50];
        if (rapidLeft > 0) { setcolor(YELLOW); sprintf(buffTxt, "[BAN NHANH] %ds", rapidLeft); outtextxy(20, yOffset, buffTxt); yOffset += 30; }
        if (shieldLeft > 0) { setcolor(CYAN); sprintf(buffTxt, "[KHIEN CUNG] %ds", shieldLeft); outtextxy(20, yOffset, buffTxt); yOffset += 30; }
        if (critLeft > 0) { setcolor(LIGHTRED); sprintf(buffTxt, "[CHI MANG] %ds", critLeft); outtextxy(20, yOffset, buffTxt); }
    }
}

void drawRightPanel() {
    int startX = 1075; 
    setfillstyle(SOLID_FILL, DARKGRAY); bar(startX, MAZE_Y, startX + 185, MAZE_Y + 450);
    setcolor(YELLOW); rectangle(startX, MAZE_Y, startX + 185, MAZE_Y + 450);
    setfillstyle(SOLID_FILL, BLUE); bar(startX + 1, MAZE_Y + 1, startX + 184, MAZE_Y + 40);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2); setcolor(WHITE); outtextxy(startX + 30, MAZE_Y + 10, (char*)"DIEU KHIEN"); 

    setcolor(LIGHTCYAN); outtextxy(startX + 10, MAZE_Y + 70, (char*)"- Mui Ten:"); 
    setcolor(WHITE); outtextxy(startX + 10, MAZE_Y + 100, (char*)"  Di chuyen xe");
    setcolor(LIGHTCYAN); outtextxy(startX + 10, MAZE_Y + 150, (char*)"- SPACE:"); 
    setcolor(WHITE); outtextxy(startX + 10, MAZE_Y + 180, (char*)"  Ban dan");
    setcolor(LIGHTCYAN); outtextxy(startX + 10, MAZE_Y + 230, (char*)"- Phim 0-1-2-3:"); 
    setcolor(WHITE); outtextxy(startX + 10, MAZE_Y + 260, (char*)"  Chuyen toc do");
}

void drawBossPanel() {
    setcolor(LIGHTRED); settextstyle(TRIPLEX_FONT, HORIZ_DIR, 3);
    outtextxy(MAZE_X, MAZE_Y + 540, (char*)"BOSS HP");
    setfillstyle(SOLID_FILL, DARKGRAY); bar(MAZE_X, MAZE_Y + 575, MAZE_X + 840, MAZE_Y + 615); 
    int w = (int)((float)boss.health / boss.maxHealth * 840); if(w < 0) w = 0;
    setfillstyle(SOLID_FILL, RED); bar(MAZE_X, MAZE_Y + 575, MAZE_X + w, MAZE_Y + 615);
    setcolor(WHITE); rectangle(MAZE_X, MAZE_Y + 575, MAZE_X + 840, MAZE_Y + 615);
}

void drawTank(Entity e, int bodyColor, int darkColor, char* name) {
    float a = e.angle; int cx = e.x, cy = e.y;
    float scale = 1.35; 
    
    float tL[6][2] = { {-12,-14}, {10,-14}, {13,-11}, {13,-8}, {-13,-8}, {-14,-11} };
    drawRotatedPoly(cx, cy, a, scale, 6, tL, BLACK, DARKGRAY);
    float tR[6][2] = { {-12,14}, {10,14}, {13,11}, {13,8}, {-13,8}, {-14,11} };
    drawRotatedPoly(cx, cy, a, scale, 6, tR, BLACK, DARKGRAY);
    
    float rad = a * PI / 180.0, c = cos(rad), s = sin(rad);
    for(int i = -10; i <= 10; i += 4) {
        int lx1 = cx + (int)((i*scale)*c + (-14*scale)*s), ly1 = cy - (int)((i*scale)*s - (-14*scale)*c);
        int lx2 = cx + (int)((i*scale)*c + (-8*scale)*s),  ly2 = cy - (int)((i*scale)*s - (-8*scale)*c);
        myLine(lx1, ly1, lx2, ly2, BLACK); 
        int rx1 = cx + (int)((i*scale)*c + (8*scale)*s),   ry1 = cy - (int)((i*scale)*s - (8*scale)*c);
        int rx2 = cx + (int)((i*scale)*c + (14*scale)*s),  ry2 = cy - (int)((i*scale)*s - (14*scale)*c);
        myLine(rx1, ry1, rx2, ry2, BLACK); 
    }

    float hull[4][2] = { {-11,-8}, {11,-8}, {11,8}, {-11,8} }; drawRotatedPoly(cx, cy, a, scale, 4, hull, BLACK, bodyColor);
    float vent[4][2] = { {-10,-5}, {-5,-5}, {-5,5}, {-10,5} }; drawRotatedPoly(cx, cy, a, scale, 4, vent, BLACK, BLACK);
    float gun[4][2] = { {0,-2}, {22,-2}, {22,2}, {0,2} }; drawRotatedPoly(cx, cy, a, scale, 4, gun, BLACK, LIGHTGRAY);
    float muzzle[4][2] = { {18,-4}, {25,-4}, {25,4}, {18,4} }; drawRotatedPoly(cx, cy, a, scale, 4, muzzle, BLACK, darkColor);
    float turret[8][2] = { {-6,-7}, {4,-7}, {8,-3}, {8,3}, {4,7}, {-6,7}, {-9,3}, {-9,-3} }; drawRotatedPoly(cx, cy, a, scale, 8, turret, BLACK, bodyColor);
    
    int hx = cx + (int)(-2*scale * c), hy = cy - (int)(-2*scale * s);
    setcolor(BLACK); setfillstyle(SOLID_FILL, darkColor); fillellipse(hx, hy, (int)(3*scale), (int)(3*scale));
    
    if (e.shieldEndTime > clock()) { setcolor(CYAN); circle(cx, cy, (int)(18*scale)); circle(cx, cy, (int)(19*scale)); }
    setcolor(WHITE); settextstyle(SMALL_FONT, HORIZ_DIR, 4); outtextxy(cx - 10, cy - (int)(25*scale), name);
}

void shootBullet(Entity &shooter, int isPlayer) {
    long currentTime = clock();
    int cooldown = (shooter.rapidFireEndTime > currentTime) ? 200 : 650; 
    if (currentTime - shooter.lastShotTime > cooldown) {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) {
                bullets[i].active = 1; bullets[i].isPlayer = isPlayer; bullets[i].angle = shooter.angle;
                float rad = shooter.angle * PI / 180.0;
                bullets[i].x = shooter.x + 25 * cos(rad); bullets[i].y = shooter.y - 25 * sin(rad);
                shooter.lastShotTime = currentTime; 
                playShoot();
                break;
            }
        }
    }
}

void drawMenuTank(int cx, int cy, int dir, int color1, int color2) {
    int s = 8; 
    setcolor(BLACK); setfillstyle(SOLID_FILL, BLACK); fillellipse(cx, cy + 19*s, 32*s, 5*s);
    float angle = (dir == 1) ? 22.0 : 158.0; float rad = angle * PI / 180.0;
    int gStart_x = cx + 8*s*dir, gStart_y = cy - 12*s;
    int gEnd_x = gStart_x + 40*s * cos(rad), gEnd_y = gStart_y - 40*s * sin(rad);
    setfillstyle(SOLID_FILL, color2); fillellipse(gStart_x, gStart_y, 6*s, 6*s);
    for(int i = -s; i <= s; i++) myLine(gStart_x, gStart_y + i, gEnd_x, gEnd_y + i, LIGHTGRAY);
    myLine(gStart_x, gStart_y - s, gEnd_x, gEnd_y - s, WHITE); myLine(gStart_x, gStart_y + s, gEnd_x, gEnd_y + s, DARKGRAY);
    int dx = (int)(3*s * sin(rad)), dy = (int)(3*s * cos(rad));
    int mPts[] = {gEnd_x - dx, gEnd_y - dy, gEnd_x + dx, gEnd_y + dy, gEnd_x + dx + (int)(8*s*cos(rad)), gEnd_y + dy - (int)(8*s*sin(rad)), gEnd_x - dx + (int)(8*s*cos(rad)), gEnd_y - dy - (int)(8*s*sin(rad)), gEnd_x - dx, gEnd_y - dy};
    setfillstyle(SOLID_FILL, color2); fillpoly(5, mPts);
    setfillstyle(SOLID_FILL, BLACK); bar(cx - 27*s, cy + 6*s, cx + 27*s, cy + 20*s); 
    setfillstyle(SOLID_FILL, DARKGRAY); bar(cx - 24*s, cy + 8*s, cx + 24*s, cy + 18*s); 
    setfillstyle(SOLID_FILL, LIGHTGRAY); fillellipse(cx - 25*s, cy + 8*s, 3*s, 3*s); fillellipse(cx + 25*s, cy + 8*s, 3*s, 3*s);
    for(int i = -20; i <= 20; i += 10) { setfillstyle(SOLID_FILL, LIGHTGRAY); fillellipse(cx + i*s, cy + 14*s, 5*s, 5*s); setfillstyle(SOLID_FILL, DARKGRAY); fillellipse(cx + i*s, cy + 14*s, 3*s, 3*s); setfillstyle(SOLID_FILL, YELLOW); fillellipse(cx + i*s, cy + 14*s, 1*s, 1*s); }
    int b1[] = {cx - 30*s, cy + 6*s, cx + 30*s, cy + 6*s, cx + 24*s, cy - 2*s, cx - 24*s, cy - 2*s, cx - 30*s, cy + 6*s}; setfillstyle(SOLID_FILL, color2); fillpoly(5, b1);
    int b2[] = {cx - 24*s, cy - 2*s, cx + 24*s, cy - 2*s, cx + 14*s, cy - 10*s, cx - 16*s, cy - 10*s, cx - 24*s, cy - 2*s}; setfillstyle(SOLID_FILL, color1); fillpoly(5, b2);
    setcolor(DARKGRAY); myLine(cx - 10*s, cy - 4*s, cx + 10*s, cy - 4*s, DARKGRAY); myLine(cx - 12*s, cy - 6*s, cx + 12*s, cy - 6*s, DARKGRAY);
    int tur[] = {cx - 12*s, cy - 10*s, cx + 10*s, cy - 10*s, cx + 6*s, cy - 20*s, cx - 8*s, cy - 20*s, cx - 12*s, cy - 10*s}; setfillstyle(SOLID_FILL, color1); fillpoly(5, tur);
    setfillstyle(SOLID_FILL, color2); bar(cx - 6*s, cy - 22*s, cx + 2*s, cy - 20*s); fillellipse(cx - 2*s, cy - 23*s, 2*s, 1*s);
    setcolor(YELLOW); setfillstyle(SOLID_FILL, YELLOW); fillellipse(cx - 2*s*dir, cy - 15*s, 2*s, 2*s); setfillstyle(SOLID_FILL, RED); fillellipse(cx - 2*s*dir, cy - 15*s, 1*s, 1*s);
    setcolor(DARKGRAY); myLine(cx - 6*s*dir, cy - 20*s, cx - 10*s*dir, cy - 35*s, DARKGRAY); setfillstyle(SOLID_FILL, RED); fillellipse(cx - 10*s*dir, cy - 35*s, 1*s, 1*s); 
}

void drawMenu() {
    cleardevice();
    setfillstyle(SOLID_FILL, LIGHTCYAN); bar(0, 0, WIDTH, 400); 
    setcolor(LIGHTRED); setfillstyle(SOLID_FILL, LIGHTRED); fillellipse(WIDTH/2, 380, 160, 160);
    setcolor(YELLOW); setfillstyle(SOLID_FILL, YELLOW); fillellipse(WIDTH/2, 380, 120, 120);
    setcolor(DARKGRAY); setfillstyle(SOLID_FILL, DARKGRAY);
    int m1[] = {-100, 400, 200, 150, 500, 400, -100, 400}; fillpoly(4, m1);
    int m2[] = {300, 400, 700, 100, 1100, 400, 300, 400};  fillpoly(4, m2);
    int m3[] = {800, 400, 1150, 180, 1400, 400, 800, 400}; fillpoly(4, m3);
    setfillstyle(SOLID_FILL, GREEN); bar(0, 400, WIDTH, HEIGHT); 
    setfillstyle(SOLID_FILL, BLACK); bar(0, 400, WIDTH, 403); 

    drawMenuTank(260, 420, 1, YELLOW, BROWN); drawMenuTank(1020, 420, -1, LIGHTBLUE, BLUE); 
    setfillstyle(SOLID_FILL, LIGHTRED); fillellipse(680, 250, 6, 6);
    setfillstyle(SOLID_FILL, YELLOW); fillellipse(550, 250, 4, 4); fillellipse(450, 250, 4, 4);

    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 6); 
    char* title = (char*)"IRON MAZE: TANK BOSS"; int tx = (WIDTH - textwidth(title)) / 2, ty = 60;
    setcolor(BLACK); outtextxy(tx + 4, ty + 4, title); setcolor(YELLOW); outtextxy(tx, ty, title);        

    int boxW = 550, boxH = 220, bx = (WIDTH - boxW) / 2, by = 470;
    setfillstyle(SOLID_FILL, BLACK); bar(bx, by, bx + boxW, by + boxH); 
    setfillstyle(SOLID_FILL, DARKGRAY); bar(bx - 5, by - 5, bx + boxW - 5, by + boxH - 5); 
    setcolor(YELLOW); rectangle(bx - 5, by - 5, bx + boxW - 5, by + boxH - 5);
    setcolor(WHITE); rectangle(bx - 3, by - 3, bx + boxW - 7, by + boxH - 7);

    setcolor(WHITE); settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    char* opt1 = (char*)"1. BAT DAU CHOI", *opt2 = (char*)"2. HUONG DAN CHI TIET";
    outtextxy((WIDTH - textwidth(opt1)) / 2 - 5, by + 35, opt1); outtextxy((WIDTH - textwidth(opt2)) / 2 - 5, by + 95, opt2);
    setcolor(LIGHTGREEN); settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    char* prompt = (char*)"Nhan phim 1 hoac 2 de chon..."; outtextxy((WIDTH - textwidth(prompt)) / 2 - 5, by + 170, prompt);
}

void drawTutorial() {
    cleardevice();
    setcolor(YELLOW); settextstyle(TRIPLEX_FONT, HORIZ_DIR, 5);
    char* title = (char*)"HUONG DAN CHI TIET"; int titleX = (WIDTH - textwidth(title)) / 2;
    outtextxy(titleX, 40, title); myLine(titleX - 20, 90, titleX + textwidth(title) + 20, 90, WHITE);

    int baseX = 320; 
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    setcolor(LIGHTCYAN); outtextxy(baseX, 130, (char*)"- DI CHUYEN:"); setcolor(WHITE); outtextxy(baseX + 180, 130, (char*)"Su dung phim mui ten (Len, Xuong, Trai, Phai)");
    setcolor(LIGHTCYAN); outtextxy(baseX, 180, (char*)"- BAN DAN:"); setcolor(WHITE); outtextxy(baseX + 180, 180, (char*)"Phim SPACE (Dau cach)");
    setcolor(LIGHTCYAN); outtextxy(baseX, 230, (char*)"- TOC DO:"); setcolor(WHITE); outtextxy(baseX + 180, 230, (char*)"Phim 0 (Dung), 1 (Cham), 2 (Chuan), 3 (Max)");
    
    setcolor(LIGHTRED); outtextxy(baseX - 30, 280, (char*)"LUU Y: Boss se truy duoi lien tuc. Ban trung Boss de thang.");
    outtextxy(baseX - 30, 310, (char*)"       Dung vao Boss hoac het mau se thua. Dan Boss gay 20 DMG.");

    setcolor(YELLOW); outtextxy(baseX, 370, (char*)"HE THONG DAO CU:");
    drawIcon(21, baseX + 50, 430); setcolor(WHITE); outtextxy(baseX + 80, 420, (char*)"BAN NHANH: Giam thoi gian nap dan lien thanh.");
    drawIcon(22, baseX + 50, 480); setcolor(WHITE); outtextxy(baseX + 80, 470, (char*)"KHIEN CUNG: Tao vong vo boc mien nhiem sat thuong.");
    drawIcon(23, baseX + 50, 530); setcolor(WHITE); outtextxy(baseX + 80, 520, (char*)"HOI MAU: Hoi ngay lap tuc 20 HP.");
    drawIcon(24, baseX + 50, 580); setcolor(WHITE); outtextxy(baseX + 80, 570, (char*)"CHI MANG: Dan bieu tuong lua, gay 25 DMG/vien.");

    setcolor(LIGHTGRAY); char* backMsg = (char*)"Nhan ENTER de quay lai Menu..."; outtextxy((WIDTH - textwidth(backMsg))/2, 660, backMsg);
}

// ==============================================================
// GAME LOOP CHINH
// ==============================================================

int main() {
    initwindow(WIDTH, HEIGHT, (char*)"Tank Boss Pro - Nguyen Bao Trung");
    srand(time(NULL));
    int page = 0, gameState = 0; 
    bool isGameInitialized = false;

    bool isMenuMusicPlaying = false; 
    bool isTutorialMusicPlaying = false;

    while (true) {
        if (gameState == 0) {
            if (!isMenuMusicPlaying) {
                playMenuBGM();
                isMenuMusicPlaying = true;
            }

            setactivepage(page); drawMenu(); setvisualpage(page); page = 1 - page;
            
            if (GetAsyncKeyState('1') & 0x8000) { 
                gameState = 2; 
                stopMenuBGM(); 
                isMenuMusicPlaying = false; 
                delay(200); 
            }
            if (GetAsyncKeyState('2') & 0x8000) { 
                gameState = 1; 
                stopMenuBGM(); 
                isMenuMusicPlaying = false; 
                delay(200); 
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        } 
        else if (gameState == 1) {
            if (!isTutorialMusicPlaying) {
                playTutorialBGM();
                isTutorialMusicPlaying = true;
            }

            setactivepage(page); drawTutorial(); setvisualpage(page); page = 1 - page;
            
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) { 
                gameState = 0; 
                stopTutorialBGM(); 
                isTutorialMusicPlaying = false; 
                delay(200); 
            }
        } 
        else if (gameState == 2) {
            if (!isGameInitialized) {
                generateRandomMaze(); 
                player.health = 100; player.maxHealth = 100; player.speed_level = 2; player.angle = 0; player.x = MAZE_X + 52; player.y = MAZE_Y + 52;
                boss.health = 400; boss.maxHealth = 400; boss.speed_level = 1; boss.angle = 180; boss.x = MAZE_X + 787; boss.y = MAZE_Y + 472;
                player.rapidFireEndTime = 0; player.shieldEndTime = 0; player.critEndTime = 0;
                for(int i=0; i<MAX_BULLETS; i++) bullets[i].active = 0;
                for(int i=0; i<10; i++) explosions[i].active = 0;
                lastItemSpawnTime = clock(); isGameInitialized = true;
                
                playBGM(); 
            }

            setactivepage(page); cleardevice();

            if (clock() - lastItemSpawnTime > 2500) {
                lastItemSpawnTime = clock();
                int itemCount = 0;
                for (int r = 0; r < 15; r++) for (int c = 0; c < 24; c++) if (maze[r][c] >= 21 && maze[r][c] <= 24) itemCount++;
                if (itemCount < 4) { 
                    for (int attempt = 0; attempt < 20; attempt++) {
                        int rr = rand() % 15, cc = rand() % 24;
                        if (maze[rr][cc] == 0) { maze[rr][cc] = 21 + rand() % 4; break; }
                    }
                }
            }

            drawLeftPanel(); drawRightPanel(); drawBossPanel();
            
            for (int r = 0; r < 15; r++) {
                for (int c = 0; c < 24; c++) {
                    int px = c * GRID_SIZE + MAZE_X, py = r * GRID_SIZE + MAZE_Y;
                    if (maze[r][c] == 1) { 
                        setfillstyle(SOLID_FILL, DARKGRAY); bar(px, py, px + GRID_SIZE, py + GRID_SIZE);
                        setcolor(LIGHTGRAY); rectangle(px, py, px + GRID_SIZE, py + GRID_SIZE);
                    } 
                    else if (maze[r][c] >= 21 && maze[r][c] <= 24) drawIcon(maze[r][c], px + 17, py + 17);
                    else if (maze[r][c] == 3) { drawKoch(3, px, py + 17, px + 35, py + 17, LIGHTMAGENTA); drawKoch(3, px + 17, py, px + 17, py + 35, LIGHTBLUE); }
                }
            }

            float base_speed = 3.0 * player.speed_level; float nx = player.x, ny = player.y;

            if (GetAsyncKeyState('0') & 0x8000) player.speed_level = 0;
            if (GetAsyncKeyState('1') & 0x8000) player.speed_level = 1;
            if (GetAsyncKeyState('2') & 0x8000) player.speed_level = 2;
            if (GetAsyncKeyState('3') & 0x8000) player.speed_level = 3;

            if (GetAsyncKeyState(VK_UP) & 0x8000)    { ny -= base_speed; player.angle = 90; }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000)  { ny += base_speed; player.angle = 270; }
            if (GetAsyncKeyState(VK_LEFT) & 0x8000)  { nx -= base_speed; player.angle = 180; }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { nx += base_speed; player.angle = 0; }
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) shootBullet(player, 1);

            if (isValidMove(nx, ny, 12)) { player.x = nx; player.y = ny; }

            int pC = (player.x - MAZE_X) / GRID_SIZE, pR = (player.y - MAZE_Y) / GRID_SIZE;
            if (maze[pR][pC] >= 21 && maze[pR][pC] <= 24) {
                int itemType = maze[pR][pC]; maze[pR][pC] = 0; 
                playItem(); 
                
                long duration = (rand() % 6 + 10) * 1000; long currentTime = clock();
                if (itemType == 21) { if (player.rapidFireEndTime < currentTime) player.rapidFireEndTime = currentTime + duration; else player.rapidFireEndTime += duration; } 
                else if (itemType == 22) { if (player.shieldEndTime < currentTime) player.shieldEndTime = currentTime + duration; else player.shieldEndTime += duration; }
                else if (itemType == 23) { player.health += 20; if (player.health > player.maxHealth) player.health = player.maxHealth; }
                else if (itemType == 24) { if (player.critEndTime < currentTime) player.critEndTime = currentTime + duration; else player.critEndTime += duration; }
            }

            float dx = player.x - boss.x, dy = player.y - boss.y; boss.angle = atan2(-dy, dx) * 180 / PI; if (boss.angle < 0) boss.angle += 360;
            int bGridX = ((int)boss.x - MAZE_X) / GRID_SIZE, bGridY = ((int)boss.y - MAZE_Y) / GRID_SIZE;
            int pGridX = ((int)player.x - MAZE_X) / GRID_SIZE, pGridY = ((int)player.y - MAZE_Y) / GRID_SIZE;
            if(bGridX < 0) bGridX = 0; if(bGridX > 23) bGridX = 23; if(bGridY < 0) bGridY = 0; if(bGridY > 14) bGridY = 14;
            if(pGridX < 0) pGridX = 0; if(pGridX > 23) pGridX = 23; if(pGridY < 0) pGridY = 0; if(pGridY > 14) pGridY = 14;

            Point nextStep = getNextBFS(bGridX, bGridY, pGridX, pGridY);
            float targetX = nextStep.x * GRID_SIZE + MAZE_X + GRID_SIZE/2.0f, targetY = nextStep.y * GRID_SIZE + MAZE_Y + GRID_SIZE/2.0f;
            float bSpeed = boss.speed_level * 2.8f; 

            if (boss.x < targetX) { boss.x += bSpeed; if (boss.x > targetX) boss.x = targetX; } else if (boss.x > targetX) { boss.x -= bSpeed; if (boss.x < targetX) boss.x = targetX; }
            if (boss.y < targetY) { boss.y += bSpeed; if (boss.y > targetY) boss.y = targetY; } else if (boss.y > targetY) { boss.y -= bSpeed; if (boss.y < targetY) boss.y = targetY; }
            if (rand() % 30 == 0) shootBullet(boss, 0);
            if (abs(player.x - boss.x) < 25 && abs(player.y - boss.y) < 25) player.health = 0; 

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    float rad = bullets[i].angle * PI / 180.0; bullets[i].x += 18 * cos(rad); bullets[i].y -= 18 * sin(rad); 
                    if (bullets[i].isPlayer && player.critEndTime > clock()) setfillstyle(SOLID_FILL, LIGHTRED); else setfillstyle(SOLID_FILL, YELLOW);
                    fillellipse(bullets[i].x, bullets[i].y, 4, 4);

                    int bC = (bullets[i].x - MAZE_X) / GRID_SIZE, bR = (bullets[i].y - MAZE_Y) / GRID_SIZE;
                    if (bC < 0 || bC >= 24 || bR < 0 || bR >= 15 || maze[bR][bC] == 1 || maze[bR][bC] == 3) {
                        bullets[i].active = 0; spawnExplosion(bullets[i].x, bullets[i].y, YELLOW); continue;
                    }
                    
                    if (bullets[i].isPlayer == 1) { 
                        if (abs(bullets[i].x - boss.x) < 25 && abs(bullets[i].y - boss.y) < 25) {
                            int dmg = (player.critEndTime > clock()) ? 25 : 10; boss.health -= dmg; bullets[i].active = 0; spawnExplosion(bullets[i].x, bullets[i].y, LIGHTRED);
                        }
                    } else { 
                        if (abs(bullets[i].x - player.x) < 20 && abs(bullets[i].y - player.y) < 20) {
                            if (player.shieldEndTime <= clock()) player.health -= 20; bullets[i].active = 0; spawnExplosion(bullets[i].x, bullets[i].y, YELLOW); 
                        }
                    }
                }
            }

            for (int i = 0; i < 10; i++) {
                if (explosions[i].active) {
                    float len = explosions[i].timer * 1.5f; 
                    for (int a = 0; a < 360; a += 45) drawExplosionFractal(explosions[i].x, explosions[i].y, len, a, 3, explosions[i].color);
                    explosions[i].timer++; if (explosions[i].timer > explosions[i].maxTimer) explosions[i].active = 0;
                }
            }

            drawTank(player, LIGHTGREEN, GREEN, (char*)"P1");
            drawTank(boss, LIGHTRED, RED, (char*)"BOSS");

            setvisualpage(page); page = 1 - page;

            if (player.health <= 0 || boss.health <= 0) {
                stopBGM(); 
                gameState = 3; 
                if (player.health <= 0) playLose(); 
                else playWin(); 
            }
        }
        else if (gameState == 3) {
            setactivepage(page); cleardevice();
            settextstyle(TRIPLEX_FONT, HORIZ_DIR, 5); setcolor(YELLOW);
            char* msg = (player.health <= 0) ? (char*)"GAME OVER!" : (char*)"BOSS DEFEATED!";
            int tw = textwidth(msg), th = textheight(msg); outtextxy((WIDTH - tw) / 2, (HEIGHT - th) / 2, msg);
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2); setcolor(WHITE); outtextxy((WIDTH - textwidth((char*)"Nhan ENTER de ve Menu")) / 2, (HEIGHT - th) / 2 + 80, (char*)"Nhan ENTER de ve Menu");
            
            setvisualpage(page); page = 1 - page;
            
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) { 
                gameState = 0; 
                isGameInitialized = false; 
                stopWin(); 
                stopLose();
                delay(300); 
            }
        }
        delay(30); 
    }
    closegraph(); return 0;
}
