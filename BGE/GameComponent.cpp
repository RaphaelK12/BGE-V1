#include "GameComponent.h"
#include "Model.h"
#include "Game.h"
#include <iostream>
#include <cmath>
#include <gtx\constants.hpp>
#include <ctime>

using namespace BGE;
using namespace std;

const glm::vec3 GameComponent::basisUp = glm::vec3(0, 1, 0);
const glm::vec3 GameComponent::basisLook = glm::vec3(0, 0, -1);
const glm::vec3 GameComponent::basisRight = glm::vec3(1, 0, 0);

float BGE::RandomFloat()
{	
	return (float)rand()/(float)RAND_MAX;
}

GameComponent::GameComponent(void)
{
	position = glm::vec3(0, 0, 0); 
	look = glm::vec3(0, 0, -1); 
	right = glm::vec3(1, 0, 0); 
	up = glm::vec3(0, 1, 0); 
	velocity = glm::vec3(0, 0, 0); 
	world = glm::mat4(1.0f); // Identity
	orientation = glm::quat(); // Identity
	moved = true;
	speed = 10.0f;
	parent = NULL;
	initialised = false;
	scale = glm::vec3(1.0, 1.0, 1.0);
	attachedToParent = true;
}

GameComponent::~GameComponent(void)
{
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		delete *it;		
	}
}


bool GameComponent::Initialise()
{
	// Initialise all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->initialised = (*it)->Initialise();	
	}
	return true;
}

void GameComponent::Draw()
{
	// Draw all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it)->parent = this;
		(*it ++)->Draw();		
	}
}

void GameComponent::SafeDelete(void * p)
{
	if (p != NULL)
	{
		delete p;
		p = NULL;
	}
}

void GameComponent::Cleanup()
{
	// Cleanup all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{
		(*it ++)->Cleanup();	    	
	}
}


void GameComponent::Update(float timeDelta) {
	world = glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation) *  glm::scale(glm::mat4(1), scale);
	if (parent != NULL && attachedToParent)
	{
		 world = parent->world * world;
	}
	moved = false;

	// Update all the children
	std::list<GameComponent *>::iterator it = children.begin();
	while (it != children.end())
	{	
		(*it ++)->Update(timeDelta);
	}
}

void GameComponent::Walk(float units)
{
	position += look * units;
	moved = true;
}

void GameComponent::Strafe(float units)
{
	position += right * units;
	moved = true;
}

void GameComponent::Fly(float units)
{
	position.y += units;
	moved = true;
}

void GameComponent::Pitch(float angle)
{
	// A pitch is a rotation around the right vector
	glm::quat rot = glm::angleAxis(angle, right);

	orientation = rot * orientation;
	
	look = RotateVector(basisLook, orientation);
	up = RotateVector(basisUp, orientation);

	/*
	glm::mat4 pitch;
	pitch = glm::rotate(pitch, angle, right);

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = pitch * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tup = glm::vec4(up, 0);
	tlook = pitch * tup;
	up = glm::vec3(tup);
	*/
	moved = true;
}

void GameComponent::Yaw(float angle)
{
	// A yaw is a rotation around the global up vector
	glm::quat rot = glm::angleAxis(angle, basisUp);

	orientation = rot * orientation;

	look = RotateVector(basisLook, orientation);
	right = RotateVector(basisRight, orientation);

	//look = basisLook * orientation;
	//right = basisRight * orientation;


	/*
	glm::mat4 yaw;
	yaw = glm::rotate(yaw, angle, up); 

	glm::vec4 tlook = glm::vec4(look, 0);
	tlook = yaw * tlook;
	look = glm::vec3(tlook);

	glm::vec4 tright = glm::vec4(right, 0);
	tright = yaw * tright;
	right = glm::vec3(tright);
	*/
	moved = true;
}

void GameComponent::RotateVectors()
{
	look = orientation * look;
	right = orientation * right;
	up = orientation * up;

}

void GameComponent::Roll(float angle)
{
	
	moved = true;
}

void GameComponent::AddChild(GameComponent * child)
{
	child->parent = this;
	children.push_back(child);
}


std::list<GameComponent *> * GameComponent::GetChildren()
{
	return & children;
}