#pragma once
#include "GameConfig.h"

// ==============================================================
// THUAT TOAN SINH ME CUNG (DFS MAZE)
// ==============================================================
inline void shuffleDirs(int* d) {
    for(int i = 0; i < 4; i++) d[i] = i;
    for(int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = d[i]; d[i] = d[j]; d[j] = temp;
    }
}

inline void carveMaze(int r, int c) {
    int dr[] = {-2, 2, 0, 0};
    int dc[] = {0, 0, -2, 2};
    int d[4]; shuffleDirs(d); 
    for(int i = 0; i < 4; i++) {
        int r_step = dr[d[i]], c_step = dc[d[i]];
        int nr = r + r_step, nc = c + c_step;
        if(nr >= 1 && nr <= 13 && nc >= 1 && nc <= 21) {
            if(maze[nr][nc] == 1) {
                maze[r + r_step/2][c + c_step/2] = 0; 
                maze[nr][nc] = 0; 
                carveMaze(nr, nc); 
            }
        }
    }
}

inline void generateRandomMaze() {
    for(int r = 0; r < 15; r++) for(int c = 0; c < 24; c++) maze[r][c] = 1;
    maze[1][1] = 0; carveMaze(1, 1);
    for(int r = 1; r <= 13; r++) { maze[r][22] = 0; if(r % 2 == 1 || (rand() % 100 < 40)) maze[r][21] = 0; }
    for(int r = 2; r < 13; r++) for(int c = 2; c < 22; c++) if(maze[r][c] == 1 && (rand() % 100 < 15)) maze[r][c] = 0;
    for(int i = 1; i <= 3; i++) for(int j = 1; j <= 3; j++) maze[i][j] = 0;
    for(int i = 11; i <= 13; i++) for(int j = 20; j <= 22; j++) maze[i][j] = 0;
    maze[4][5] = 3; maze[4][6] = 3; maze[10][17] = 3; maze[10][18] = 3;
    for(int r=0; r<15; r++) { maze[r][0] = 1; maze[r][23] = 1; }
    for(int c=0; c<24; c++) { maze[0][c] = 1; maze[14][c] = 1; }
}

inline Point getNextBFS(int startX, int startY, int targetX, int targetY) {
    if (startX == targetX && startY == targetY) return {startX, startY};
    Point parent[15][24];
    for(int i=0; i<15; i++) for(int j=0; j<24; j++) parent[i][j] = {-1, -1};
    Point queue[15*24]; int front = 0, rear = 0;
    queue[rear++] = {startX, startY}; parent[startY][startX] = {startX, startY};
    int dx[] = {0, 0, -1, 1}, dy[] = {-1, 1, 0, 0}; int found = 0;
    while(front < rear) {
        Point curr = queue[front++];
        if (curr.x == targetX && curr.y == targetY) { found = 1; break; }
        for(int i=0; i<4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < 24 && ny >= 0 && ny < 15) {
                if (maze[ny][nx] != 1 && maze[ny][nx] != 3 && parent[ny][nx].x == -1) {
                    parent[ny][nx] = curr; queue[rear++] = {nx, ny};
                }
            }
        }
    }
    if (!found) return {startX, startY};
    Point step = {targetX, targetY};
    while(parent[step.y][step.x].x != startX || parent[step.y][step.x].y != startY) step = parent[step.y][step.x];
    return step;
}

// ==============================================================
// TP2: THUAT TOAN VE CO BAN (BRESENHAM & MIDPOINT)
// ==============================================================

// Thuat toan Bresenham ve doan thang (Ap dung cho moi he so goc m)
inline void myLine(int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; } // Truong hop |m| <= 1
        if (e2 <= dx) { err += dx; y1 += sy; } // Truong hop |m| > 1
    }
}

// Thuat toan Midpoint ve duong tron (Doi xung 8 nhanh)
inline void myCircle(int xc, int yc, int r, int color) {
    int x = 0, y = r, p = 1 - r; // Tham so quyet dinh ban dau
    while (x <= y) {
        putpixel(xc+x, yc+y, color); putpixel(xc-x, yc+y, color);
        putpixel(xc+x, yc-y, color); putpixel(xc-x, yc-y, color);
        putpixel(xc+y, yc+x, color); putpixel(xc-y, yc+x, color);
        putpixel(xc+y, yc-x, color); putpixel(xc-y, yc-x, color);
        if (p < 0) p += 2 * x + 3; else { p += 2 * (x - y) + 5; y--; }
        x++;
    }
}

// Thuat toan to mau da giac de quy
inline void myFloodFill(int x, int y, int fill_color, int boundary_color) {
    int current_color = getpixel(x, y);
    if (current_color != boundary_color && current_color != fill_color) {
        putpixel(x, y, fill_color);
        myFloodFill(x + 1, y, fill_color, boundary_color);
        myFloodFill(x - 1, y, fill_color, boundary_color);
        myFloodFill(x, y + 1, fill_color, boundary_color);
        myFloodFill(x, y - 1, fill_color, boundary_color);
    }
}

// ==============================================================
// TP3: HINH HOC FRACTAL (DUONG CONG KOCH)
// ==============================================================

// Thuat toan de quy sinh duong cong Koch trang tri Menu va Rao can
inline void drawKoch(int n, int x1, int y1, int x5, int y5, int color) {
    if (n == 0) myLine(x1, y1, x5, y5, color);
    else {
        int x2 = x1 + (x5 - x1)/3, y2 = y1 + (y5 - y1)/3;
        int x4 = x1 + 2*(x5 - x1)/3, y4 = y1 + 2*(y5 - y1)/3;
        int x3 = (int)(0.5*(x1 + x5) + sqrt(3.0)/6*(y1 - y5));
        int y3 = (int)(0.5*(y1 + y5) + sqrt(3.0)/6*(x5 - x1));
        drawKoch(n-1, x1,y1, x2,y2, color); drawKoch(n-1, x2,y2, x3,y3, color);
        drawKoch(n-1, x3,y3, x4,y4, color); drawKoch(n-1, x4,y4, x5,y5, color);
    }
}

// Ung dung Fractal tao hieu ung no (Tia lua vang ra)
inline void drawExplosionFractal(float x, float y, float len, float angle, int depth, int color) {
    if (depth == 0) return;
    float rad = angle * PI / 180.0;
    float ex = x + len * cos(rad), ey = y - len * sin(rad);
    myLine((int)x, (int)y, (int)ex, (int)ey, color);
    drawExplosionFractal(ex, ey, len * 0.6, angle - 30, depth - 1, color);
    drawExplosionFractal(ex, ey, len * 0.6, angle + 30, depth - 1, color);
}

// ==============================================================
// TP4: PHEP BIEN DOI AFFINE (AFFINE TRANSFORMATIONS)
// ==============================================================

// Ham ket hop 3 phep bien doi: Co gian (Scale) -> Quay (Rotate) -> Tinh tien (Translate)
inline void drawRotatedPoly(int cx, int cy, float angle, float scale, int n, float pts[][2], int borderColor, int fillColor) {
    int poly[30]; 
    float rad = angle * PI / 180.0;
    float c = cos(rad), s = sin(rad); // Ma tran quay (Rotation Matrix)
    for(int i = 0; i < n; i++) {
        float px = pts[i][0] * scale;
        float py = pts[i][1] * scale;
        poly[i*2]     = cx + (int)(px * c + py * s);
        poly[i*2 + 1] = cy - (int)(px * s - py * c); 
    }
    poly[n*2] = poly[0]; poly[n*2 + 1] = poly[1];
    setcolor(borderColor); setfillstyle(SOLID_FILL, fillColor); fillpoly(n, poly);
}

inline int isValidMove(float x, float y, int margin) {
    int left = (x - margin - MAZE_X) / GRID_SIZE, right = (x + margin - MAZE_X) / GRID_SIZE;
    int top = (y - margin - MAZE_Y) / GRID_SIZE, bottom = (y + margin - MAZE_Y) / GRID_SIZE;
    if (left < 0 || right >= 24 || top < 0 || bottom >= 15) return 0;
    if (maze[top][left] == 1 || maze[top][left] == 3 || maze[top][right] == 1 || maze[top][right] == 3 ||
        maze[bottom][left] == 1 || maze[bottom][left] == 3 || maze[bottom][right] == 1 || maze[bottom][right] == 3) return 0;
    return 1;
}
