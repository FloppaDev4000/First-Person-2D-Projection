#include <stdio.h>
#include <iostream>
#include "raylib.h"

using namespace std;

class Transformm
{
public:
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Transformm()
	{
		position = { 0.0, 0.0, 0.0 };
		rotation = { 0.0, 0.0, 0.0 };
		scale =    { 1.0, 1.0, 1.0 };
	}

	Transformm(Vector3 pos, Vector3 rot, Vector3 scl)
	{
		position = pos;
		rotation = rot;
		scale = scl;
	}
};
