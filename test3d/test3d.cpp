// test3d.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "input.hpp"
#include <vector>

using namespace std;

void printPoints();
void printDistances(float[]);


int action_MvLeft[2] = { KEY_A, KEY_LEFT };
int action_MvRight[2] = { KEY_D, KEY_RIGHT };
int action_MvUp[2] = { KEY_W, KEY_UP };
int action_MvDown[2] = { KEY_S, KEY_DOWN };

int action_rotLeft[1] = { KEY_Q };
int action_rotRight[2] = { KEY_E, KEY_R };

int action_UiAccept[2] = { KEY_SPACE, KEY_ENTER };

class Cam2d
{
public:
    Vector2 position = { 0.0, 0.0 };
    float fov = 90.0;
    float rotation = 0.0; // radians: 0 to 2PI
};

void drawPlayer(Cam2d);
void drawLines(Vector2[], Cam2d);
float interpolateX(float, float, float, float, float);

Vector2 points[] = {
    {100, 50},
    {700, 50},
    {700, 400},
    {100, 400}
};

int main2()
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes");
    SetTargetFPS(60);

    Cam2d cam = Cam2d();
    cam.position = { 400.0, 225.0 };

    // set rotation to up
    Camera2D rotato = Camera2D();
    rotato.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    
    rotato.zoom = 0.5f;
    rotato.rotation = 0.0f;

    printPoints();

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // input
        Vector2 dirInput = actionVector(action_MvDown, action_MvUp, action_MvLeft, action_MvRight);
        float rotInput = actionAxis(action_rotLeft, action_rotRight);

        cam.position += Vector2Rotate(dirInput, cam.rotation) * 3;
        cam.rotation += rotInput * 0.05;

        rotato.target = cam.position;
        rotato.rotation = -cam.rotation * RAD2DEG - 90;

        if (actionPressed(action_UiAccept))
        {
            cout << "POINTS:";
            printPoints();
        }


        //--------------------------------------DRAW
        BeginDrawing();

        

        BeginMode2D(rotato);

        ClearBackground(BLACK);

        // draw player
        drawPlayer(cam);


        // draw wall
        drawLines(points, cam);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void drawPlayer(Cam2d cam)
{
    Vector2 lookDirVec = { cos(cam.rotation), sin(cam.rotation) };

    DrawCircle(cam.position.x, cam.position.y, 4, { 255, 0, 255, 255 });
    DrawLine(cam.position.x, cam.position.y,
        cam.position.x + lookDirVec.x * 40, cam.position.y + lookDirVec.y * 40,
        { 255, 0, 255, 255 });
}

void drawLines(Vector2 arr[], Cam2d cam)
{
    int len = arrayLen(arr);

    vector<float> distances(4);

    // get distances of lines
    for (int i = 0; i < 4; i++)
    {
        Vector2 pt = points[i];
        float dist = cam.position.y - pt.y;

        // fix negative pts
        if (dist <= 0.0f)
        {
            //points[i].y = GetScreenWidth() / 2.0f;
            printf("AAAAA!");
            dist = 0.01f;
        }

        distances[i] = dist;
    }
    float* d = &distances[0];
    printDistances(d);

    for (int i = 0; i < 4; i++)
    {
        //points[i].x = points[i].x / distances[i];
    }

    for (int i = 0; i < 3; i++)
    {
        Vector2 pt = points[i];
        Vector2 pt2 = points[i + 1];
        DrawLine(pt.x/distances[i], pt.y, pt2.x / distances[i], pt2.y, { 255, 0, 255, 255 });
    }
    DrawLine(points[3].x / distances[3], points[3].y, points[0].x / distances[0], points[0].y, {255, 0, 255, 255});

    // draw points
    for (int i = 0; i < 4; i++)
    {
        DrawCircle(points[i].x, points[i].y, 5, { 255, 0, 255, 255 });
    }

}

// find X of pt on line based on Y
float interpolateX(float x1, float y1, float x2, float y2, float y) {

    return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
}

void printPoints()
{
    for (int i = 0; i < 4; i++)
    {
        cout << "POINT " << i << ": X" << points[i].x << " Y" << points[i].y << endl;
    }
}

void printDistances(float arr[])
{
    for (int i = 0; i < 4; i++)
    {
        cout << "DIST POINT " << i << ": " << arr[i] << endl;
    }
}