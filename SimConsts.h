#pragma once
#include "pch.h"
#include "Vector.h"

//Particle Info
static constexpr float DEFAULT_PARTICLE_MASS = 10.0f;
static constexpr float DEFAULT_PARTICLE_DRAG_COEFFICIENT = 0.43f;
static constexpr float PARTICLE_COLLIDER_SIZE = 0.5f;
static constexpr float PARTICLE_INTERACTION_DISTANCE = 100.0f;
static Vector2f WORLD_SIZE = Vector2f(1366.0f, 768.0f);
static Vector2i GRID_SIZE = Vector2i(16, 9);
static constexpr float WORLD_EDGE = 16.0f;

//Simulation stuff
static constexpr int STARTING_PARTICLE_COUNT = 500;
static constexpr int MAX_PARTICLE_COUNT = 2000;
static constexpr float KERNEL_HEIGHT = 16.0f;
static constexpr float DAMPENING = -0.75f;
static Vector2f GRAVITY = Vector2f(0.0f, 9.81f);

/* VISCOSITY */static float VISCOSITY_CONSTANT = 0.9f;
/* STIFFNESS */ static float GAS_CONSTANT = 0.004f;
/* DIFF STIFFNESS */ static float NEAR_STIFFNESS = 0.1f;
/* DENSITY */static float REST_DENSITY = 1000.0f;
