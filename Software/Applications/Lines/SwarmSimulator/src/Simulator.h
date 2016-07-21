/*
 * Simulator.h
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
 * \file   Simulator.h
 * \brief  Declares the Simulator class.
 */

#ifndef HRVO_SIMULATOR_H_
#define HRVO_SIMULATOR_H_

#include <limits>
#include <vector>
#include "Definitions.h"
#ifndef HRVO_VECTOR2_H_
#include "Vector2.h"
#endif

/**
 * \brief  Set to 0 for a holonomic agent; set to 1 for a differential-drive agent.
 */
#define HRVO_DIFFERENTIAL_DRIVE 1

namespace hrvo {
	class Agent;
	class Goal;
	class KdTree;

	/**
	 * \class  Simulator
	 * \brief  The simulation.
	 */
	class Simulator {
	public:
		/**
		 * \brief  Constructor.
		 */
		Simulator();

		/**
		 * \brief  Destructor.
		 */
		~Simulator();

		/**
		 * \brief      Adds a new agent with default properties to the simulation.
		 * \param[in]  position  The starting position of this agent.
		 * \param[in]  goalNo    The goal number of this agent.
		 * \return     The number of the agent.
		 */
		std::size_t addAgent(const Vector2 &position, std::size_t goalNo);

		/**
		 * \brief      Adds a new agent to the simulation.
		 * \param[in]  position           The starting position of this agent.
		 * \param[in]  goalNo             The goal number of this agent.
		 * \param[in]  neighborDist       The maximum neighbor distance of this agent.
		 * \param[in]  maxNeighbors       The maximum neighbor count of this agent.
		 * \param[in]  radius             The radius of this agent.
		 * \param[in]  goalRadius         The goal radius of this agent.
		 * \param[in]  prefSpeed          The preferred speed of this agent.
		 * \param[in]  maxSpeed           The maximum speed of this agent.
		 * \param[in]  uncertaintyOffset  The uncertainty offset of this agent.
		 * \param[in]  maxAccel           The maximum acceleration of this agent.
		 * \param[in]  velocity           The initial velocity of this agent.
		 * \param[in]  orientation        The initial orientation (in radians) of this agent.
		 * \return     The number of the agent.
		 */
		std::size_t addAgent(const Vector2 &position, std::size_t goalNo, float neighborDist, std::size_t maxNeighbors, float radius, float goalRadius, float prefSpeed, float maxSpeed,
#if HRVO_DIFFERENTIAL_DRIVE
			float timeToOrientation, float wheelTrack,
#endif /* HRVO_DIFFERENTIAL_DRIVE */
			float uncertaintyOffset = 0.0f, float maxAccel = std::numeric_limits<float>::infinity(), const Vector2 &velocity = Vector2(0.0f, 0.0f), float orientation = 0.0f);

		/**
		 * \brief      Adds a new goal to the simulation.
		 * \param[in]  position  The position of this goal.
		 * \return     The number of the goal.
		 */
		std::size_t addGoal(const Vector2 &position);

		/**
		 * \brief  Performs a simulation step; updates the orientation, position, and velocity of each agent, and the progress of each towards its goal.
		 */
		void doStep();

		/**
		 * \brief      Returns the goal number of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose goal number is to be retrieved.
		 * \return     The present goal number of the agent.
		 */
		std::size_t getAgentGoal(std::size_t agentNo) const;

		/**
		 * \brief      Returns the goal radius of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose goal radius is to be retrieved.
		 * \return     The present goal radius of the agent
		 */
		float getAgentGoalRadius(std::size_t agentNo) const;

#if HRVO_DIFFERENTIAL_DRIVE
		/**
		 * \brief      Returns the left wheel speed of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose left wheel speed is to be retrieved.
		 * \return     The present signed left wheel speed of the agent.
		 */
		float getAgentLeftWheelSpeed(std::size_t agentNo) const;
#endif /* HRVO_DIFFERENTIAL_DRIVE */

		/**
		 * \brief      Returns the maximum acceleration of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose maximum acceleration is to be retrieved.
		 * \return     The present maximum acceleration of the agent.
		 */
		float getAgentMaxAccel(std::size_t agentNo) const;

		/**
		 * \brief      Returns the maximum neighbor count of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose maximum neighbor count is to be retrieved.
		 * \return     The present maximum neighbor count of the agent.
		 */
		std::size_t getAgentMaxNeighbors(std::size_t agentNo) const;

		/**
		 * \brief      Returns the maximum speed of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose maximum speed is to be retrieved.
		 * \return     The present maximum speed of the agent.
		 */
		float getAgentMaxSpeed(std::size_t agentNo) const;

		/**
		 * \brief      Returns the maximum neighbor distance of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose maximum neighbor distance is to be retrieved.
		 * \return     The present maximum neighbor distance of the agent.
		 */
		float getAgentNeighborDist(std::size_t agentNo) const;

		/**
		 * \brief      Returns the orientation of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose orientation is to be retrieved.
		 * \return     The present orientation (in radians) of the agent.
		 */
		float getAgentOrientation(std::size_t agentNo) const;

		/**
		 * \brief      Returns the position of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose position is to be retrieved.
		 * \return     The present position of the (center of) the agent.
		 */
		Vector2 getAgentPosition(std::size_t agentNo) const;

		/**
		 * \brief      Returns the preferred speed of a specified agent.
		 * \details    The preferred speed of an agent is the speed it would choose to take if it were not influenced by other agents.
		 * \param[in]  agentNo  The number of the agent whose preferred speed is to be retrieved.
		 * \return     The present preferred speed of the agent.
		 */
		float getAgentPrefSpeed(std::size_t agentNo) const;

		/**
		 * \brief      Returns the radius of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose radius is to be retrieved.
		 * \return     The present radius of the agent.
		 */
		float getAgentRadius(std::size_t agentNo) const;

		/**
		 * \brief      Returns the progress towards its goal of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose progress towards its goal is to be retrieved.
		 * \return     True if the agent has reached its goal; false otherwise.
		 */
		bool getAgentReachedGoal(std::size_t agentNo) const;

#if HRVO_DIFFERENTIAL_DRIVE
		/**
		 * \brief      Returns the right wheel speed of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose right wheel speed is to be retrieved.
		 * \return     The present signed right wheel speed of the agent.
		 */
		float getAgentRightWheelSpeed(std::size_t agentNo) const;

		/**
		 * \brief      Returns the "time to orientation" of a specified agent.
		 * \details    The time to orientation is the time period that a differential-drive agent is given to assume the orientation defined by its new velocity.
		 * \param[in]  agentNo  The number of the agent whose time to orientation is to be retrieved.
		 * \return     The present time to orientation of the agent.
		 */
		float getAgentTimeToOrientation(std::size_t agentNo) const;
#endif /* HRVO_DIFFERENTIAL_DRIVE */

		/**
		 * \brief      Returns the "uncertainty offset" of a specified agent.
		 * \details    The uncertainty offset is the amount velocity obstacles are widened to allow for uncertainty in the position and velocity of a differential drive agent.
		 * \param[in]  agentNo  The number of the agent whose uncertainty offset is to be retrieved.
		 * \return     The present uncertainty offset of the agent.
		 */
		float getAgentUncertaintyOffset(std::size_t agentNo) const;

		/**
		 * \brief      Returns the velocity of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose velocity is to be retrieved.
		 * \return     The present velocity of the agent.
		 */
		Vector2 getAgentVelocity(std::size_t agentNo) const;

#if HRVO_DIFFERENTIAL_DRIVE
		/**
		 * \brief      Returns the wheel track of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose wheel track is to be retrieved.
		 * \return     The present wheel track of the agent.
		 */
		float getAgentWheelTrack(std::size_t agentNo) const;
#endif /* HRVO_DIFFERENTIAL_DRIVE */

		/**
		 * \brief   Returns the global time of the simulation.
		 * \return  The present global time of the simulation (zero initially).
		 */
		float getGlobalTime() const { return globalTime_; }

		/**
		 * \brief      Returns the position of a specified goal.
		 * \param[in]  goalNo  The number of the goal whose position is to be retrieved.
		 * \return     The position of the goal.
		 */
		Vector2 getGoalPosition(std::size_t goalNo) const;

		/**
		 * \brief   Returns the count of agents in the simulation.
		 * \return  The count of agents in the simulation.
		 */
		std::size_t getNumAgents() const { return agents_.size(); }

		/**
		 * \brief   Returns the count of goals in the simulation.
		 * \return  The count of goals in the simulation.
		 */
		std::size_t getNumGoals() const { return goals_.size(); }

		/**
		 * \brief   Returns the time step of the simulation.
		 * \return  The present time step of the simulation.
		 */
		float getTimeStep() const { return timeStep_; }

		/**
		 * \brief   Returns the progress towards their goals of all agents.
		 * \return  True if all agents have reached their goals; false otherwise.
		 */
		bool haveReachedGoals() const { return reachedGoals_; }

		/**
		 * \brief      Sets the default properties for any new agent that is added.
		 * \param[in]  neighborDist       The default maximum neighbor distance of a new agent.
		 * \param[in]  maxNeighbors       The default maximum neighbor count of a new agent.
		 * \param[in]  radius             The default radius of a new agent.
		 * \param[in]  goalRadius         The default goal radius of a new agent.
		 * \param[in]  prefSpeed          The default preferred speed of a new agent.
		 * \param[in]  maxSpeed           The default maximum speed of a new agent.
		 * \param[in]  uncertaintyOffset  The default uncertainty offset of a new agent.
		 * \param[in]  maxAccel           The default maximum acceleration of a new agent.
		 * \param[in]  velocity           The default initial velocity of a new agent.
		 * \param[in]  orientation        The default initial orientation (in radians) of a new agent.
		 */
		void setAgentDefaults(float neighborDist, std::size_t maxNeighbors, float radius, float goalRadius, float prefSpeed, float maxSpeed,
#if HRVO_DIFFERENTIAL_DRIVE
			float timeToOrientation, float wheelTrack,
#endif /* HRVO_DIFFERENTIAL_DRIVE */
			float uncertaintyOffset = 0.0f, float maxAccel = std::numeric_limits<float>::infinity(), const Vector2 &velocity = Vector2(), float orientation = 0.0f);

		/**
		 * \brief      Sets the goal number of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose goal number is to be modified.
		 * \param[in]  goalNo   The replacement goal number.
		 */
		void setAgentGoal(std::size_t agentNo, std::size_t goalNo);

		/**
		 * \brief      Sets the goal radius of a specified agent.
		 * \param[in]  agentNo     The number of the agent whose goal radius is to be modified.
		 * \param[in]  goalRadius  The replacement goal radius.
		 */
		void setAgentGoalRadius(std::size_t agentNo, float goalRadius);

		/**
		 * \brief      Sets the maximum linear acceleraton of a specified agent.
		 * \param[in]  agentNo   The number of the agent whose maximum acceleration is to be modified.
		 * \param[in]  maxAccel  The replacement maximum acceleration.
		 */
		void setAgentMaxAccel(std::size_t agentNo, float maxAccel);

		/**
		 * \brief      Sets the maximum neighbor count of a specified agent.
		 * \param[in]  agentNo       The number of the agent whose maximum neighbor count is to be modified.
		 * \param[in]  maxNeighbors  The replacement maximum neighbor count.
		 */
		void setAgentMaxNeighbors(std::size_t agentNo, std::size_t maxNeighbors);

		/**
		 * \brief      Sets the maximum speed of a specified agent.
		 * \param[in]  agentNo   The number of the agent whose maximum speed is to be modified.
		 * \param[in]  maxSpeed  The replacement maximum speed.
		 */
		void setAgentMaxSpeed(std::size_t agentNo, float maxSpeed);

		/**
		 * \brief      Sets the maximum neighbor distance of a specified agent.
		 * \param[in]  agentNo       The number of the agent whose maximum neighbor distance is to be modified.
		 * \param[in]  neighborDist  The replacement maximum neighbor distance.
		 */
		void setAgentNeighborDist(std::size_t agentNo, float neighborDist);

		/**
		 * \brief      Sets the orientation of a specified agent.
		 * \param[in]  agentNo      The number of the agent whose orientation is to be modified.
		 * \param[in]  orientation  The replacement orientation (in radians).
		 */
		void setAgentOrientation(std::size_t agentNo, float orientation);

		/**
		 * \brief      Sets the position of a specified agent.
		 * \param[in]  agentNo   The number of the agent whose position is to be modified.
		 * \param[in]  position  The replacement position.
		 */
		void setAgentPosition(std::size_t agentNo, const Vector2 &position);

		/**
		 * \brief      Sets the preferred speed of a specified agent.
		 * \details    The preferred speed of an agent is the speed it would choose to take if it were not influenced by other agents.
		 * \param[in]  agentNo    The number of the agent whose preferred speed is to be modified.
		 * \param[in]  prefSpeed  The replacement preferred speed.
		 */
		void setAgentPrefSpeed(std::size_t agentNo, float prefSpeed);

		/**
		 * \brief      Sets the radius of a specified agent.
		 * \param[in]  agentNo  The number of the agent whose radius is to be modified.
		 * \param[in]  radius   The replacement radius.
		 */
		void setAgentRadius(std::size_t agentNo, float radius);

#if HRVO_DIFFERENTIAL_DRIVE
		/**
		 * \brief      Sets the "time to orientation" of a specified agent.
		 * \details    The time to orientation is the time period that a differential-drive agent is given to assume the orientation defined by its new velocity.
		 * \param[in]  agentNo            The number of the agent whose time to orientation is to be modified.
		 * \param[in]  timeToOrientation  The replacement time to orientation.
		 */
		void setAgentTimeToOrientation(std::size_t agentNo, float timeToOrientation);

		/**
		 * \brief      Sets the wheel track of a specified agent.
		 * \param[in]  agentNo     The number of the agent whose wheel track is to be modified.
		 * \param[in]  wheelTrack  The replacement wheel track.
		 */
		void setAgentWheelTrack(std::size_t agentNo, float wheelTrack);
#endif /* HRVO_DIFFERENTIAL_DRIVE */

		/**
		 * \brief      Sets the "uncertainty offset" of a specified agent.
		 * \details    The uncertainty offset is the amount velocity obstacles are widened to allow for uncertainty in the position and velocity of a differential drive agent.
		 * \param[in]  agentNo            The number of the agent whose uncertainty offset is to be modified.
		 * \param[in]  uncertaintyOffset  The replacement uncertainty offset.
		 */
		void setAgentUncertaintyOffset(std::size_t agentNo, float uncertaintyOffset);

		/**
		 * \brief      Sets the velocity of a specified agent.
		 * \param[in]  agentNo   The number of the agent whose velocity is to be modified.
		 * \param[in]  velocity  The replacement velocity.
		 */
		void setAgentVelocity(std::size_t agentNo, const Vector2 &velocity);

		/**
		 * \brief      Sets the time step of the simulation.
		 * \param[in]  timeStep  The replacement time step of the simulation.
		 */
		void setTimeStep(float timeStep) { timeStep_ = timeStep; }

        
        void setAgentGoal(std::size_t goalNo, const Vector2 &position);
        
        void flushGoals();
        
        
	private:
		Simulator(const Simulator &other);
		Simulator &operator=(const Simulator &other);

		Agent *defaults_;
		KdTree *kdTree_;
		float globalTime_;
		float timeStep_;
		bool reachedGoals_;
		std::vector<Agent *> agents_;
		std::vector<Goal *> goals_;

		friend class Agent;
		friend class Goal;
		friend class KdTree;
	};
}

#endif /* HRVO_SIMULATOR_H_ */
