#pragma once
#include "pch.h"
#include "Vector.h"

//Particle Info
static float DEFAULT_PARTICLE_MASS = 1.0f;
static float DEFAULT_PARTICLE_DRAG_COEFFICIENT = 0.43f;
static float PARTICLE_COLLIDER_SIZE = 2.0f;
static float PARTICLE_SEARCH_DISTANCE = 100.0f;
static Vector2f WORLD_SIZE = Vector2f(1000.0f, 1000.0f);
static Vector2i GRID_SIZE = Vector2i(12, 12);
static float WORLD_EDGE = 110.0f;

//Simulation stuff
static int STARTING_PARTICLE_COUNT = 5000;
static int MAX_PARTICLE_COUNT = 5000;
static float KERNEL_HEIGHT = 16.0f;
static Vector2f GRAVITY = Vector2f(0.0f, 9.81f);
static float REST_LENGTH = 1.0f;
static float DAMPENING = -0.75f;
static float GAS_CONSTANT = 8371.0f;
static float REST_DENSITY = 1500.0f;
static float VISCOSITY_CONSTANT = 1000.0f;