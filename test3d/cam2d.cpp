#include <iostream>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

using namespace std;

class Cam2d
{
	Vector2 position = { 0.0, 0.0 };
	float fov = 90.0;
};