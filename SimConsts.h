#pragma once
#include "pch.h"
#include "Vector.h"

//Particle Info
static float DEFAULT_PARTICLE_MASS = 10.0f;
static float DEFAULT_PARTICLE_DRAG_COEFFICIENT = 0.43f;
static float PARTICLE_COLLIDER_SIZE = 5.0f;
static float PARTICLE_INTERACTION_DISTANCE = 75.0f;
static Vector2f WORLD_SIZE = Vector2f(800.0f, 800.0f);
static Vector2i GRID_SIZE = Vector2i(4, 4);
static float WORLD_EDGE = 10.0f;

//Simulation stuff
static int STARTING_PARTICLE_COUNT = 500;
static int MAX_PARTICLE_COUNT = 10000;
static float KERNEL_HEIGHT = 16.0f;
static Vector2f GRAVITY = Vector2f(0.0f, 9.81f);
static float DAMPENING = -0.75f;

/*
* If a highly viscous behavior is desired, sigma can be increased.
For less viscous fluids, only beta should be set to a non-zero value
//*/
static float VISCOSITY_CONSTANT_BETA = 0.0f;
static float VISCOSITY_CONSTANT_SIGMA = 0.9f;
/* STIFFNESS */ static float GAS_CONSTANT = 0.009f;
/* K NEAR */ static float NEAR_STIFFNESS_PARAM = 0.1f;
static float REST_DENSITY = 1000.0F;

//static float VISCOSITY_CONSTANT_BETA = 0.1f;
//static float VISCOSITY_CONSTANT_SIGMA = 0.05f;
///* STIFFNESS */ static float GAS_CONSTANT = 0.004;
///* K NEAR */ static float NEAR_STIFFNESS_PARAM = 0.01f;
//static float REST_DENSITY = 10.0f;
