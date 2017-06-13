/*
 * Simulator.cpp
 * HRVO Library
 *
 * Copyright (c) 2009-2015 University of North Carolina at Chapel Hill.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, non-commercial research, and non-profit
 * purposes, without fee, and without a written agreement is hereby granted,
 * provided that the above copyright notice, this paragraph, and the following
 * four paragraphs appear in all copies.
 *
 * Permission to incorporate this software into commercial products may be
 * obtained by contacting the authors <geom@cs.unc.edu> or the Office of
 * Technology Development at the University of North Carolina at Chapel Hill
 * <otd@unc.edu>.
 *
 * This software program and documentation are copyrighted by the University of
 * North Carolina at Chapel Hill. The software program and documentation are
 * supplied "as is," without any accompanying services from the University of
 * North Carolina at Chapel Hill or the authors. The University of North
 * Carolina at Chapel Hill and the authors do not warrant that the operation of
 * the program will be uninterrupted or error-free. The end-user understands
 * that the program was developed for research purposes and is advised not to
 * rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
 * AUTHORS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
 * SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT
 * CHAPEL HILL OR THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
 * DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY
 * STATUTORY WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON
 * AN "AS IS" BASIS, AND THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE
 * AUTHORS HAVE NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jamie Snape, Jur van den Berg, Stephen J. Guy, and Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <http://gamma.cs.unc.edu/HRVO/>
 */

/**
 * \file   Simulator.cpp
 * \brief  Defines the Simulator class.
 */

#ifndef HRVO_SIMULATOR_H_
#include "Simulator.h"
#endif

#include <stdexcept>

#ifndef HRVO_AGENT_H_
#include "Agent.h"
#endif
#ifndef HRVO_GOAL_H_
#include "Goal.h"
#endif
#ifndef HRVO_KD_TREE_H_
#include "KdTree.h"
#endif


namespace hrvo {
	Simulator::Simulator() : defaults_(NULL), kdTree_(NULL), globalTime_(0.0f), timeStep_(0.0f), reachedGoals_(false)
	{
		kdTree_ = new KdTree(this);
	}

	Simulator::~Simulator()
	{
		delete defaults_;
		defaults_ = NULL;

		delete kdTree_;
		kdTree_ = NULL;

		for (std::vector<Agent *>::iterator iter = agents_.begin(); iter != agents_.end(); ++iter) {
			delete *iter;
			*iter = NULL;
		}

		for (std::vector<Goal *>::iterator iter = goals_.begin(); iter != goals_.end(); ++iter) {
			delete *iter;
			*iter = NULL;
		}
	}

	std::size_t Simulator::addAgent(const Vector2 &position, std::size_t goalNo)
	{
		if (defaults_ == NULL) {
			throw std::runtime_error("Agent defaults not set when adding agent.");
		}

		Agent *const agent = new Agent(this, position, goalNo);
		agents_.push_back(agent);

		return agents_.size() - 1;
	}

	std::size_t Simulator::addAgent(const Vector2 &position, std::size_t goalNo, float neighborDist, std::size_t maxNeighbors, float radius, float goalRadius, float prefSpeed, float maxSpeed,
#if HRVO_DIFFERENTIAL_DRIVE
		float timeToOrientation, float wheelTrack,
#endif /* HRVO_DIFFERENTIAL_DRIVE */
		float uncertaintyOffset, float maxAccel, const Vector2 &velocity, float orientation)
	{
		Agent *const agent = new Agent(this, position, goalNo, neighborDist, maxNeighbors, radius, velocity, maxAccel, goalRadius, prefSpeed, maxSpeed, orientation,
#if HRVO_DIFFERENTIAL_DRIVE
			timeToOrientation, wheelTrack,
#endif /* HRVO_DIFFERENTIAL_DRIVE */
			uncertaintyOffset);
		agents_.push_back(agent);

		return agents_.size() - 1;
	}

	std::size_t Simulator::addGoal(const Vector2 &position)
	{
		Goal *const goal = new Goal(position);
		goals_.push_back(goal);

		return goals_.size() - 1;
	}

	void Simulator::doStep()
	{
		if (kdTree_ == NULL) {
			throw std::runtime_error("Simulation not initialized when attempting to do step.");
		}

		if (timeStep_ == 0.0f) {
			throw std::runtime_error("Time step not set when attempting to do step.");
		}

		reachedGoals_ = true;

		kdTree_->build();

		for (std::vector<Agent *>::iterator iter = agents_.begin(); iter != agents_.end(); ++iter) {
			(*iter)->computePreferredVelocity();
			(*iter)->computeNeighbors();
			(*iter)->computeNewVelocity();
#if HRVO_DIFFERENTIAL_DRIVE
			(*iter)->computeWheelSpeeds();
#endif /* HRVO_DIFFERENTIAL_DRIVE */
		}

		for (std::vector<Agent *>::iterator iter = agents_.begin(); iter != agents_.end(); ++iter) {
			(*iter)->update();
		}

		globalTime_ += timeStep_;
	}

	std::size_t Simulator::getAgentGoal(std::size_t agentNo) const
	{
		return agents_[agentNo]->goalNo_;
	}

	float Simulator::getAgentGoalRadius(std::size_t agentNo) const
	{
		return agents_[agentNo]->goalRadius_;
	}

#if HRVO_DIFFERENTIAL_DRIVE
	float Simulator::getAgentLeftWheelSpeed(std::size_t agentNo) const
	{
		return agents_[agentNo]->leftWheelSpeed_;
	}
#endif /* HRVO_DIFFERENTIAL_DRIVE */

	float Simulator::getAgentMaxAccel(std::size_t agentNo) const
	{
		return agents_[agentNo]->maxAccel_;
	}

	std::size_t Simulator::getAgentMaxNeighbors(std::size_t agentNo) const
	{
		return agents_[agentNo]->maxNeighbors_;
	}

	float Simulator::getAgentMaxSpeed(std::size_t agentNo) const
	{
		return agents_[agentNo]->maxSpeed_;
	}

	float Simulator::getAgentNeighborDist(std::size_t agentNo) const
	{
		return agents_[agentNo]->neighborDist_;
	}

	float Simulator::getAgentOrientation(std::size_t agentNo) const
	{
		return agents_[agentNo]->orientation_;
	}

	Vector2 Simulator::getAgentPosition(std::size_t agentNo) const
	{
		return agents_[agentNo]->position_;
	}

	float Simulator::getAgentPrefSpeed(std::size_t agentNo) const
	{
		return agents_[agentNo]->prefSpeed_;
	}

	float Simulator::getAgentRadius(std::size_t agentNo) const
	{
		return agents_[agentNo]->radius_;
	}

	bool Simulator::getAgentReachedGoal(std::size_t agentNo) const
	{
		return agents_[agentNo]->reachedGoal_;
	}

#if HRVO_DIFFERENTIAL_DRIVE
	float Simulator::getAgentRightWheelSpeed(std::size_t agentNo) const
	{
		return agents_[agentNo]->rightWheelSpeed_;
	}

	float Simulator::getAgentTimeToOrientation(std::size_t agentNo) const
	{
		return agents_[agentNo]->timeToOrientation_;
	}
#endif /* HRVO_DIFFERENTIAL_DRIVE */

	float Simulator::getAgentUncertaintyOffset(std::size_t agentNo) const
	{
		return agents_[agentNo]->uncertaintyOffset_;
	}

	Vector2 Simulator::getAgentVelocity(std::size_t agentNo) const
	{
		return agents_[agentNo]->velocity_;
	}

#if HRVO_DIFFERENTIAL_DRIVE
	float Simulator::getAgentWheelTrack(std::size_t agentNo) const
	{
		return agents_[agentNo]->wheelTrack_;
	}
#endif /* HRVO_DIFFERENTIAL_DRIVE */

	Vector2 Simulator::getGoalPosition(std::size_t goalNo) const
	{
		return goals_[goalNo]->position_;
	}

	void Simulator::setAgentDefaults(float neighborDist, std::size_t maxNeighbors, float radius, float goalRadius, float prefSpeed, float maxSpeed,
#if HRVO_DIFFERENTIAL_DRIVE
		float timeToOrientation, float wheelTrack,
#endif /* HRVO_DIFFERENTIAL_DRIVE */
		float uncertaintyOffset, float maxAccel, const Vector2 &velocity, float orientation)
	{
		if (defaults_ == NULL) {
			defaults_ = new Agent(this);
		}

		defaults_->goalRadius_ = goalRadius;
		defaults_->maxAccel_ = maxAccel;
		defaults_->maxNeighbors_ = maxNeighbors;
		defaults_->maxSpeed_ = maxSpeed;
		defaults_->neighborDist_ = neighborDist;
		defaults_->newVelocity_ = velocity;
		defaults_->uncertaintyOffset_ = uncertaintyOffset;
		defaults_->orientation_ = orientation;
		defaults_->prefSpeed_ = prefSpeed;
		defaults_->radius_ = radius;
		defaults_->velocity_ = velocity;

#if HRVO_DIFFERENTIAL_DRIVE
		defaults_->timeToOrientation_ = timeToOrientation;
		defaults_->wheelTrack_ = wheelTrack;

		defaults_->computeWheelSpeeds();
#endif /* HRVO_DIFFERENTIAL_DRIVE */
	}

	void Simulator::setAgentGoal(std::size_t agentNo, std::size_t goalNo)
	{
		agents_[agentNo]->goalNo_ = goalNo;
	}

	void Simulator::setAgentGoalRadius(std::size_t agentNo, float goalRadius)
	{
		agents_[agentNo]->goalRadius_ = goalRadius;
	}

	void Simulator::setAgentMaxAccel(std::size_t agentNo, float maxAccel)
	{
		agents_[agentNo]->maxAccel_ = maxAccel;
	}

	void Simulator::setAgentMaxNeighbors(std::size_t agentNo, std::size_t maxNeighbors)
	{
		agents_[agentNo]->maxNeighbors_ = maxNeighbors;
	}

	void Simulator::setAgentMaxSpeed(std::size_t agentNo, float maxSpeed)
	{
		agents_[agentNo]->maxSpeed_ = maxSpeed;
	}

	void Simulator::setAgentNeighborDist(std::size_t agentNo, float neighborDist)
	{
		agents_[agentNo]->neighborDist_ = neighborDist;
	}

	void Simulator::setAgentOrientation(std::size_t agentNo, float orientation)
	{
		agents_[agentNo]->orientation_ = orientation;
	}

	void Simulator::setAgentPosition(std::size_t agentNo, const Vector2 &position)
	{
		agents_[agentNo]->position_ = position;
	}

	void Simulator::setAgentPrefSpeed(std::size_t agentNo, float prefSpeed)
	{
		agents_[agentNo]->prefSpeed_ = prefSpeed;
	}

	void Simulator::setAgentRadius(std::size_t agentNo, float radius)
	{
		agents_[agentNo]->radius_ = radius;
	}
    
    //////////////////////////////////////////////////////////////////////////////////
    //                                                                              //
    //      ADDED BY MATHIEU                                                        //
    //                                                                              //
    //////////////////////////////////////////////////////////////////////////////////

    void Simulator::flushGoals()
    {
        goals_.clear();
    }
    
    void Simulator::setAgentGoal(std::size_t goalNo, const Vector2 &position)
    {
        goals_[goalNo]->position_ = position;
    }
    
    
#if HRVO_DIFFERENTIAL_DRIVE
	void Simulator::setAgentTimeToOrientation(std::size_t agentNo, float timeToOrientation)
	{
		agents_[agentNo]->timeToOrientation_ = timeToOrientation;
	}
#endif /* HRVO_DIFFERENTIAL_DRIVE */

	void Simulator::setAgentUncertaintyOffset(std::size_t agentNo, float uncertaintyOffset)
	{
		agents_[agentNo]->uncertaintyOffset_ = uncertaintyOffset;
	}

	void Simulator::setAgentVelocity(std::size_t agentNo, const Vector2 &velocity)
	{
		agents_[agentNo]->velocity_ = velocity;
	}

#if HRVO_DIFFERENTIAL_DRIVE
	void Simulator::setAgentWheelTrack(std::size_t agentNo, float wheelTrack)
	{
		agents_[agentNo]->wheelTrack_ = wheelTrack;
	}
#endif /* HRVO_DIFFERENTIAL_DRIVE */
}
