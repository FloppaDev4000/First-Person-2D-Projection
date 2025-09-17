#include <stdio.h>
#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "input.hpp"
#include <list>

using namespace std;

// input actions
int action_MvLeft[2] = { KEY_A, KEY_LEFT };
int action_MvRight[2] = { KEY_D, KEY_RIGHT };
int action_MvUp[2] = { KEY_W, KEY_UP };
int action_MvDown[2] = { KEY_S, KEY_DOWN };

int action_rotLeft[1] = { KEY_Q };
int action_rotRight[2] = { KEY_E };

int action_rotUp[1] = { KEY_R };
int action_rotDown[1] = { KEY_F };

// length of points array
const int ptsLen = 7;

// class for Line
class Line
{
public:
    Vector2 pt1;
    Vector2 pt2;
    Line(Vector2 p, Vector2 p2)
    {
        pt1 = p;
        pt2 = p2;
    }
};

// class for player
class Player
{
public:
    Vector2 pos;
    float fov = 90.0;
    float rotation = 0.0; // degrees
};

// func headers
void drawPlayer(Player);
void drawSurfaces();
void drawMyLine(Line);
void drawPartialLine(Line, Vector2);
void drawPt(Vector2);
void drawOneWall(float, float, float, float, Color);
void drawWalls(Vector2[], Camera2D);
float distanceToPlayer(Vector2);
bool isBehindPlayer(Vector2);
Vector2 vecFromRadians(float);
float crossProduct(Vector2, Vector2);
void copyVecArray(Vector2[], Vector2[], int);
void rotateWorld(Vector2[], int, float);
void moveWorld(Vector2[], int, Vector2);

// initialize player
Player p = Player();

int main()
{
    // raylib setup
    const int screenWidth = 600;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes");
    SetTargetFPS(60);

    // create objects in scene
    p.pos = { 0.0, 0.0 };
    p.rotation = -PI / 2;

    // world boundary points
    Vector2 points[ptsLen] = {
    {200, 0},
    {143, 176},
    {-9, 197},
    {-140, 76},
    {-140, -76},
    {-9, -200},
    {143, -180} 
    };

    // set rotation to up
    Camera2D cam = Camera2D();
    cam.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    cam.zoom = 1.0f;
    cam.rotation = 0.0f;

    // loop
    while (!WindowShouldClose())
    {
        // input
        Vector2 dirInput = actionVector(action_MvLeft, action_MvRight, action_MvUp, action_MvDown);
        float rotInput = actionAxis(action_rotLeft, action_rotRight);

        // world moves around player
        rotateWorld(points, ptsLen, rotInput * -0.05);
        moveWorld(points, ptsLen, dirInput * -3);

        //--------------------------------------DRAW
        BeginDrawing();

        BeginMode2D(cam);

        ClearBackground(BLACK);

        drawSurfaces();
        drawWalls(points, cam);

        EndDrawing();
    }

    CloseWindow();

	return 0;
}

// CUSTOM DRAWING
void drawPlayer(Player player)
{
    Vector2 lookDirVec = { cos(player.rotation), sin(player.rotation) };

    DrawCircle(player.pos.x, player.pos.y, 4, { 255, 0, 255, 255 });
    DrawLine(player.pos.x, player.pos.y,
        player.pos.x + lookDirVec.x * 40, player.pos.y + lookDirVec.y * 40,
        { 255, 0, 255, 255 });

}
void drawMyLine(Line l)
{
    DrawLine(l.pt1.x, l.pt1.y, l.pt2.x, l.pt2.y, {255, 0, 255, 255});
}
void drawPartialLine(Line l, Vector2 cutoff)
{
    DrawLine(l.pt1.x, l.pt1.y, l.pt2.x, l.pt2.y, { 255, 0, 255, 255 });
}

void drawPt(Vector2 pt)
{
    DrawCircle(pt.x, pt.y, 4, { 255, 0, 255 ,255 });
}

void drawSurfaces()
{
    int boundX = GetScreenWidth();
    int boundY = GetScreenHeight();
    DrawRectangle(-boundX/2, -boundY/2, boundX, boundY/2, { 150, 150, 150, 255 });
    DrawRectangle(-boundX/2, 0, boundX, boundY/2, { 100, 100, 100, 255 });
}

void drawOneWall(float xStart, float xEnd, float extrude1, float extrude2, Color color)
{
    // draw 2 triangles
    Vector2 pts[4] = {
        {xStart, extrude1},
        {xEnd, extrude2},
        {xStart, -extrude1},
        {xEnd, -extrude2}
    };

    // draw 2 triangles for quad
    DrawTriangle(pts[0], pts[1], pts[2], color);
    DrawTriangle(pts[1], pts[3], pts[2], color);
}

// calculate and draw all walls
void drawWalls(Vector2 pts[], Camera2D cam)
{
    float distances[ptsLen];
    float extrudes[ptsLen];

    Vector2 editedPts[ptsLen];
    copyVecArray(editedPts, pts, ptsLen);

    // GET DISTANCES, CALCULATE NEW POINT VALUES
    for (int idx = 0; idx < ptsLen; idx++)
    {
        distances[idx] = distanceToPlayer(pts[idx]);
        if (distances[idx]<= 0.0f){ distances[idx] = 0.01f; }

        // calculate X to account for perspective, set Y to 0
        editedPts[idx].x = editedPts[idx].x * 250 / distances[idx];
        editedPts[idx].y = 0.0f;

        // calculate wall heights
        extrudes[idx] = 11000.0 / distances[idx];
    }

    // DRAW
    for (int idx = 0; idx < ptsLen; idx++)
    {
        //---------------------------------DRAW WALLS
        // loop back around at last index
        int nextIdx = idx + 1;
        if (nextIdx >= ptsLen) { nextIdx = 0; }
        
        // draw all valid walls
        if (distances[idx] > 0 && distances[nextIdx] > 0) // both points are infront of camera
{           
            // color based on values
            Color c = {200, 0.3*distances[idx], 80 + 20 * idx, 255};

            drawOneWall(editedPts[idx].x, editedPts[nextIdx].x, extrudes[idx], extrudes[nextIdx], c);
        }

        //---------------------------------DRAW LINES

        // draw point at correct location
        //drawPt(editedPts[idx]);

        // draw line for wall corner
        //DrawLine(editedPts[idx].x, extrudes[idx], editedPts[idx].x, -extrudes[idx], { 255, 0, 255, 255 });

    }

    return;
}

//---------------------------------------------------------OTHER FUNCTIONS

// calculate distance to player
float distanceToPlayer(Vector2 pt)
{
    Vector2 dir1 = vecFromRadians(p.rotation - PI/2); // forward direction
    Vector2 displace = pt - p.pos; // direction to point in question

    float c = crossProduct(dir1, displace); // calculate distance
    return c;
}

// check if point behind player
bool isBehindPlayer(Vector2 pt)
{
    return distanceToPlayer(pt) <= 0.0f;
}

// Radians to Vector2 direction
Vector2 vecFromRadians(float rotation)
{
    return { cos(rotation), sin(rotation) };
}

// 2D version of cross product
float crossProduct(Vector2 a, Vector2 b) {
    return a.x * b.y - a.y * b.x;
}

// copy Vector2 array to second Vector2 array
void copyVecArray(Vector2 output[], Vector2 input[], int length)
{
    for (int i = 0; i < length; i++)
    {
        output[i] = input[i];
    }
}

// rotate each world around player as pivot
void rotateWorld(Vector2 array[], int length, float rotation)
{
    for (int i = 0; i < length; i++)
    {
        array[i] = Vector2Rotate(array[i], rotation);
    }
}

// move each world point based on input
void moveWorld(Vector2 array[], int length, Vector2 movement)
{
    if (movement.x == 0.0f && movement.y == 0.0f)
    {
        return;
    }
    if (!CheckCollisionPointPoly(p.pos - movement, array, length))
    {
        cout << "COLLISION!" << endl;
        return;
    }

    for (int i = 0; i < length; i++)
    {
        array[i] += movement;
    }
}

// find X of pt on line based on Y
float interpolateX(float x1, float y1, float x2, float y2, float y) {

    return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
}