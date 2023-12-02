#pragma once

#include "common/types.h"
#include "EngineScripting/commands/Command.h"
#include "components/Transform.h"
#include "components/Force.h"
#include <glm/vec3.hpp>

// TODO: MoveTo and OrientTo (All movements commands) should have a common class
class OrientTo : public Command
{
public:
	OrientTo();
	virtual ~OrientTo() {};

	virtual bool Initialize(rapidjson::Value& document);
	// For subcommands initialization, avoids searching scene every time
	void Initialize(TransformComponent* pTransform, ForceComponent* pForce, 
					bool isPositionOriented, glm::vec3 orientTo, glm::vec3 maxSpeed);
	void Initialize(TransformComponent* pTransform, ForceComponent* pForce,
					bool isPositionOriented, glm::vec3 orientTo, float time);

	// Called every frame/step:
	// Returns true when command is done
	virtual bool Update(double deltaTime);

	virtual bool IsDone(void);

	// Calculate the velocity it should reach
	virtual bool PreStart(void);

	virtual bool PostEnd(void);
private:
	enum ePhase
	{
		STARTUP,
		EASYIN,
		CONSTANT,
		EASYOUT
	};
	ePhase m_currPhase;

	glm::vec3 m_finalRotation; 

	float m_easyInRatio;
	float m_easyOutRatio;

	glm::vec3 m_easyInPos;   // Orientation it will stop accelerating
	glm::vec3 m_easyOutPos;  // Orientation it will start decelerating
	bool m_stopAtEnd;        // When arrive at orientation should stop rotating?

	float m_time;		  // Max time that should take to move
	float m_easyInTime;   // Time it will accelerate
	float m_easyOutTime;  // Time it will decelerate
	float m_constantTime; // Time the velocity will be constant

	glm::vec3 m_maxVelocity;

	float m_elapsedTime;

	TransformComponent* m_pTransform;
	ForceComponent* m_pForce;

	void m_UpdateRotation(glm::vec3 currPosition, glm::vec3 currVelocity);

	void m_SetRotationMaxVel(glm::vec3 currOrientation, glm::vec3 currCentrVelocity);
};
