#pragma once
#include "Vector.h"

class Particle;

class PhysicsModel
{
	//TODO : NOT MINE
	std::vector<int> m_particleSprings;
	//----------------------------------------------------------------------------------------------------------------------
	/// \brief updateSpringIndex  checks if there is a spring with index from and if so changes it to to
	///                           if to == -1 then it deletes that spring index from particleSprings
	/// \param[in] _from          index of spring to change
	/// \param[in] _to            index of spring to change to
	 //----------------------------------------------------------------------------------------------------------------------
	void updateSpringIndex(int _from, int _to);
	//TODO : NOT MINE
	typedef struct spring { int indexi, indexj; float L; int count; bool alive; } Spring;


private:
	bool isResting;
	Vector2f previousPosition;
	Vector2f position;
	Vector2f acceleration;
	Vector2f velocity;
	Vector2f externalForce;
	float density;
	float pressure;
	float viscosity;
	float mass;
	float nearDensity;

	float initialDensity;

	float Poly6(float radius_square);
	float Spiky(float radius);
	float ViscoKernel(float radius);

	void CalculateDoubleDensity(float DeltaTime);
	void CalculatePressure();
	void CalculateViscosity(float DeltaTime);

	void EnforceEdges();

public:

	Vector2f netForce;

	PhysicsModel();
	PhysicsModel(Vector2f position, Vector2f velocity);
	std::vector<Particle*> LocalParticles;

	Vector2f GetPosition();
	void SetPosition(Vector2f position);
	Vector2f GetVelocity();
	void SetVelocity(Vector2f velocity);
	float GetMass();
	void SetMass(float mass);
	void UpdateSPH(float DeltaTime);

	bool IsResting();

	void ApplyExternalForce(Vector2f force);
	void Reset();
};

