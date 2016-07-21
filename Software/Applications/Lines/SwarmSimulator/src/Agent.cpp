/*
 * Agent.cpp
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
 * \file   Agent.cpp
 * \brief  Defines the Agent class.
 */

#ifndef HRVO_AGENT_H_
#include "Agent.h"
#endif

#include <algorithm>
#include <cmath>
#include <limits>

#ifndef HRVO_DEFINITIONS_H_
#include "Definitions.h"
#endif
#ifndef HRVO_GOAL_H_
#include "Goal.h"
#endif
#ifndef HRVO_KD_TREE_H_
#include "KdTree.h"
#endif

namespace hrvo {
	const float HRVO_PI = 3.141592653589793f;

	Agent::Agent(Simulator *simulator) : simulator_(simulator), goalNo_(0), maxNeighbors_(0), goalRadius_(0.0f), maxAccel_(0.0f), maxSpeed_(0.0f), neighborDist_(0.0f), orientation_(0.0f), prefSpeed_(0.0f), radius_(0.0f), uncertaintyOffset_(0.0f),
#if HRVO_DIFFERENTIAL_DRIVE
		leftWheelSpeed_(0.0f), rightWheelSpeed_(0.0f), timeToOrientation_(0.0f), wheelTrack_(0.0f),
#endif /* HRVO_DIFFERENTIAL_DRIVE */
		reachedGoal_(false) { }

	Agent::Agent(Simulator *simulator, const Vector2 &position, std::size_t goalNo) : simulator_(simulator), newVelocity_(simulator_->defaults_->velocity_), position_(position), velocity_(simulator_->defaults_->velocity_), goalNo_(goalNo), maxNeighbors_(simulator_->defaults_->maxNeighbors_), goalRadius_(simulator_->defaults_->goalRadius_), maxAccel_(simulator_->defaults_->maxAccel_), maxSpeed_(simulator_->defaults_->maxSpeed_), neighborDist_(simulator_->defaults_->neighborDist_), orientation_(simulator_->defaults_->orientation_), prefSpeed_(simulator_->defaults_->prefSpeed_), radius_(simulator_->defaults_->radius_), uncertaintyOffset_(simulator_->defaults_->uncertaintyOffset_),
#if HRVO_DIFFERENTIAL_DRIVE
		leftWheelSpeed_(0.0f), rightWheelSpeed_(0.0f), timeToOrientation_(simulator_->defaults_->timeToOrientation_), wheelTrack_(simulator_->defaults_->wheelTrack_),
#endif /* HRVO_DIFFERENTIAL_DRIVE */
		reachedGoal_(false)
	{
#if HRVO_DIFFERENTIAL_DRIVE
		computeWheelSpeeds();
#endif /* HRVO_DIFFERENTIAL_DRIVE */
	}

	Agent::Agent(Simulator *simulator, const Vector2 &position, std::size_t goalNo, float neighborDist, std::size_t maxNeighbors, float radius, const Vector2 &velocity, float maxAccel, float goalRadius, float prefSpeed, float maxSpeed, float orientation,
#if HRVO_DIFFERENTIAL_DRIVE
				 float timeToOrientation, float wheelTrack,
#endif /* HRVO_DIFFERENTIAL_DRIVE */
				 float uncertaintyOffset) : simulator_(simulator), newVelocity_(velocity), position_(position), velocity_(velocity), goalNo_(goalNo), maxNeighbors_(maxNeighbors), goalRadius_(goalRadius), maxAccel_(maxAccel), maxSpeed_(maxSpeed), neighborDist_(neighborDist), orientation_(orientation), prefSpeed_(prefSpeed), radius_(radius), uncertaintyOffset_(uncertaintyOffset),
#if HRVO_DIFFERENTIAL_DRIVE
		leftWheelSpeed_(0.0f), rightWheelSpeed_(0.0f), timeToOrientation_(timeToOrientation), wheelTrack_(wheelTrack),
#endif /* HRVO_DIFFERENTIAL_DRIVE */
		reachedGoal_(false)
	{
#if HRVO_DIFFERENTIAL_DRIVE
		computeWheelSpeeds();
#endif /* HRVO_DIFFERENTIAL_DRIVE */
	}

	void Agent::computeNeighbors()
	{
		neighbors_.clear();
		simulator_->kdTree_->query(this, neighborDist_ * neighborDist_);
	}

	void Agent::computeNewVelocity()
	{
		velocityObstacles_.clear();
		velocityObstacles_.reserve(neighbors_.size());

		VelocityObstacle velocityObstacle;

		for (std::set<std::pair<float, std::size_t > >::const_iterator iter = neighbors_.begin(); iter != neighbors_.end(); ++iter) {
			const Agent *const other = simulator_->agents_[iter->second];

			if (absSq(other->position_ - position_) > sqr(other->radius_ + radius_)) {
				const float angle = atan(other->position_ - position_);
				const float openingAngle = std::asin((other->radius_ + radius_) / abs(other->position_ - position_));

				velocityObstacle.side1_ = Vector2(std::cos(angle - openingAngle), std::sin(angle - openingAngle));
				velocityObstacle.side2_ = Vector2(std::cos(angle + openingAngle), std::sin(angle + openingAngle));

				const float d = 2.0f * std::sin(openingAngle) * std::cos(openingAngle);

				if (det(other->position_ - position_, prefVelocity_ - other->prefVelocity_) > 0.0f) {
					const float s = 0.5f * det(velocity_ - other->velocity_, velocityObstacle.side2_) / d;

					velocityObstacle.apex_ = other->velocity_ + s * velocityObstacle.side1_ - (uncertaintyOffset_ * abs(other->position_ - position_) / (other->radius_ + radius_)) * normalize(other->position_ - position_);
				}
				else {
					const float s = 0.5f * det(velocity_ - other->velocity_, velocityObstacle.side1_) / d;

					velocityObstacle.apex_ = other->velocity_ + s * velocityObstacle.side2_ - (uncertaintyOffset_ * abs(other->position_ - position_) / (other->radius_ + radius_)) * normalize(other->position_ - position_);
				}

				velocityObstacles_.push_back(velocityObstacle);
			}
			else {
				velocityObstacle.apex_ = 0.5f * (other->velocity_ + velocity_) - (uncertaintyOffset_ + 0.5f * (other->radius_ + radius_ - abs(other->position_ - position_)) / simulator_->timeStep_) * normalize(other->position_ - position_);
				velocityObstacle.side1_ = normal(position_, other->position_);
				velocityObstacle.side2_ = -velocityObstacle.side1_;
				velocityObstacles_.push_back(velocityObstacle);
			}
		}

		candidates_.clear();

		Candidate candidate;

		candidate.velocityObstacle1_ = std::numeric_limits<int>::max();
		candidate.velocityObstacle2_ = std::numeric_limits<int>::max();

		if (absSq(prefVelocity_) < maxSpeed_ * maxSpeed_) {
			candidate.position_ = prefVelocity_;
		}
		else {
			candidate.position_ = maxSpeed_ * normalize(prefVelocity_);
		}

		candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));

		for (int i = 0; i < static_cast<int>(velocityObstacles_.size()); ++i) {
			candidate.velocityObstacle1_ = i;
			candidate.velocityObstacle2_ = i;

			const float dotProduct1 = (prefVelocity_ - velocityObstacles_[i].apex_) * velocityObstacles_[i].side1_;
			const float dotProduct2 = (prefVelocity_ - velocityObstacles_[i].apex_) * velocityObstacles_[i].side2_;

			if (dotProduct1 > 0.0f && det(velocityObstacles_[i].side1_, prefVelocity_ - velocityObstacles_[i].apex_) > 0.0f) {
				candidate.position_ = velocityObstacles_[i].apex_ + dotProduct1 * velocityObstacles_[i].side1_;

				if (absSq(candidate.position_) < maxSpeed_ * maxSpeed_) {
					candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
				}
			}

			if (dotProduct2 > 0.0f && det(velocityObstacles_[i].side2_, prefVelocity_ - velocityObstacles_[i].apex_) < 0.0f) {
				candidate.position_ = velocityObstacles_[i].apex_ + dotProduct2 * velocityObstacles_[i].side2_;

				if (absSq(candidate.position_) < maxSpeed_ * maxSpeed_) {
					candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
				}
			}
		}

		for (int j = 0; j < static_cast<int>(velocityObstacles_.size()); ++j) {
			candidate.velocityObstacle1_ = std::numeric_limits<int>::max();
			candidate.velocityObstacle2_ = j;

			float discriminant = maxSpeed_ * maxSpeed_ - sqr(det(velocityObstacles_[j].apex_, velocityObstacles_[j].side1_));

			if (discriminant > 0.0f) {

				const float t1 = -(velocityObstacles_[j].apex_ * velocityObstacles_[j].side1_) + std::sqrt(discriminant);
				const float t2 = -(velocityObstacles_[j].apex_ * velocityObstacles_[j].side1_) - std::sqrt(discriminant);

				if (t1 >= 0.0f) {
					candidate.position_ = velocityObstacles_[j].apex_ + t1 * velocityObstacles_[j].side1_;
					candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
				}

				if (t2 >= 0.0f) {
					candidate.position_ = velocityObstacles_[j].apex_ + t2 * velocityObstacles_[j].side1_;
					candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
				}
			}

			discriminant = maxSpeed_ * maxSpeed_ - sqr(det(velocityObstacles_[j].apex_, velocityObstacles_[j].side2_));

			if (discriminant > 0.0f) {
				const float t1 = -(velocityObstacles_[j].apex_ * velocityObstacles_[j].side2_) + std::sqrt(discriminant);
				const float t2 = -(velocityObstacles_[j].apex_ * velocityObstacles_[j].side2_) - std::sqrt(discriminant);

				if (t1 >= 0.0f) {
					candidate.position_ = velocityObstacles_[j].apex_ + t1 * velocityObstacles_[j].side2_;
					candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
				}

				if (t2 >= 0.0f) {
					candidate.position_ = velocityObstacles_[j].apex_ + t2 * velocityObstacles_[j].side2_;
					candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
				}
			}
		}

		for (int i = 0; i < static_cast<int>(velocityObstacles_.size()) - 1; ++i) {
			for (int j = i + 1; j < static_cast<int>(velocityObstacles_.size()); ++j) {
				candidate.velocityObstacle1_ = i;
				candidate.velocityObstacle2_ = j;

				float d = det(velocityObstacles_[i].side1_, velocityObstacles_[j].side1_);

				if (d != 0.0f) {
					const float s = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[j].side1_) / d;
					const float t = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[i].side1_) / d;

					if (s >= 0.0f && t >= 0.0f) {
						candidate.position_ = velocityObstacles_[i].apex_ + s * velocityObstacles_[i].side1_;

						if (absSq(candidate.position_) < maxSpeed_ * maxSpeed_) {
							candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
						}
					}
				}

				d = det(velocityObstacles_[i].side2_, velocityObstacles_[j].side1_);

				if (d != 0.0f) {
					const float s = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[j].side1_) / d;
					const float t = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[i].side2_) / d;

					if (s >= 0.0f && t >= 0.0f) {
						candidate.position_ = velocityObstacles_[i].apex_ + s * velocityObstacles_[i].side2_;

						if (absSq(candidate.position_) < maxSpeed_ * maxSpeed_) {
							candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
						}
					}
				}

				d = det(velocityObstacles_[i].side1_, velocityObstacles_[j].side2_);

				if (d != 0.0f) {
					const float s = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[j].side2_) / d;
					const float t = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[i].side1_) / d;

					if (s >= 0.0f && t >= 0.0f) {
						candidate.position_ = velocityObstacles_[i].apex_ + s * velocityObstacles_[i].side1_;

						if (absSq(candidate.position_) < maxSpeed_ * maxSpeed_) {
							candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
						}
					}
				}

				d = det(velocityObstacles_[i].side2_, velocityObstacles_[j].side2_);

				if (d != 0.0f) {
					const float s = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[j].side2_) / d;
					const float t = det(velocityObstacles_[j].apex_ - velocityObstacles_[i].apex_, velocityObstacles_[i].side2_) / d;

					if (s >= 0.0f && t >= 0.0f) {
						candidate.position_ = velocityObstacles_[i].apex_ + s * velocityObstacles_[i].side2_;

						if (absSq(candidate.position_) < maxSpeed_ * maxSpeed_) {
							candidates_.insert(std::make_pair(absSq(prefVelocity_ - candidate.position_), candidate));
						}
					}
				}
			}
		}

		int optimal = -1;

		for (std::multimap<float, Candidate>::const_iterator iter = candidates_.begin(); iter != candidates_.end(); ++iter) {
			candidate = iter->second;
			bool valid = true;

			for (int j = 0; j < static_cast<int>(velocityObstacles_.size()); ++j) {
				if (j != candidate.velocityObstacle1_ && j != candidate.velocityObstacle2_ && det(velocityObstacles_[j].side2_, candidate.position_ - velocityObstacles_[j].apex_) < 0.0f && det(velocityObstacles_[j].side1_, candidate.position_ - velocityObstacles_[j].apex_) > 0.0f) {
					valid = false;

					if (j > optimal) {
						optimal = j;
						newVelocity_ = candidate.position_;
					}

					break;
				}
			}

			if (valid) {
				newVelocity_ = candidate.position_;
				break;
			}
		}
	}

	void Agent::computePreferredVelocity()
	{
		const Vector2 goalPosition = simulator_->goals_[goalNo_]->position_;
		const float distSqToGoal = absSq(goalPosition - position_);
        if (sqr(prefSpeed_ * simulator_->timeStep_) > distSqToGoal) {
              prefVelocity_ = (goalPosition - position_) / simulator_->timeStep_;
        }
        else {
            prefVelocity_ = prefSpeed_ * (goalPosition - position_) / std::sqrt(distSqToGoal);
        }
	}

#if HRVO_DIFFERENTIAL_DRIVE
	void Agent::computeWheelSpeeds()
	{
		float targetOrientation;

        if (reachedGoal_) {
			targetOrientation = orientation_;
		}
		else {
			targetOrientation = atan(newVelocity_);
		}

		float orientationDiff = std::fmod(targetOrientation - orientation_, 2.0f * HRVO_PI);

		if (orientationDiff < -HRVO_PI) {
			orientationDiff += 2.0f * HRVO_PI;
		}

		if (orientationDiff > HRVO_PI) {
			orientationDiff -= 2.0f * HRVO_PI;
		}

		float speedDiff = (orientationDiff * wheelTrack_) / timeToOrientation_;

		if (speedDiff > 2.0f * maxSpeed_) {
			speedDiff = 2.0f * maxSpeed_;
		}
		else if (speedDiff < -2.0f * maxSpeed_) {
			speedDiff = -2.0f * maxSpeed_;
		}

		float targetSpeed = abs(newVelocity_);

		if (targetSpeed + 0.5f * std::fabs(speedDiff) > maxSpeed_) {
			if (speedDiff >= 0.0f) {
				rightWheelSpeed_ = maxSpeed_;
				leftWheelSpeed_ = maxSpeed_ - speedDiff;
			}
			else {
				leftWheelSpeed_ = maxSpeed_;
				rightWheelSpeed_ = maxSpeed_ + speedDiff;
			}
		}
		else if (targetSpeed - 0.5f * std::fabs(speedDiff) < -maxSpeed_) {
			if (speedDiff >= 0.0f) {
				leftWheelSpeed_ = -maxSpeed_;
				rightWheelSpeed_ = speedDiff - maxSpeed_;
			}
			else {
				rightWheelSpeed_ = -maxSpeed_;
				leftWheelSpeed_ = -maxSpeed_ - speedDiff;
			}
		}
		else {
			rightWheelSpeed_ = targetSpeed + 0.5f * speedDiff;
			leftWheelSpeed_ = targetSpeed - 0.5f * speedDiff;
		}
	}
#endif /* HRVO_DIFFERENTIAL_DRIVE */

	void Agent::insertNeighbor(std::size_t agentNo, float &rangeSq)
	{
		const Agent *const other = simulator_->agents_[agentNo];

		if (this != other) {
			const float distSq = absSq(position_ - other->position_);

			if (distSq < sqr(radius_ + other->radius_) && distSq < rangeSq) {
				neighbors_.clear();

				if (neighbors_.size() == maxNeighbors_) {
					neighbors_.erase(--neighbors_.end());
				}

				neighbors_.insert(std::make_pair(distSq, agentNo));

				if (neighbors_.size() == maxNeighbors_) {
					rangeSq = (--neighbors_.end())->first;
				}
			}
			else if (distSq < rangeSq) {
				if (neighbors_.size() == maxNeighbors_) {
					neighbors_.erase(--neighbors_.end());
				}

				neighbors_.insert(std::make_pair(distSq, agentNo));

				if (neighbors_.size() == maxNeighbors_) {
					rangeSq = (--neighbors_.end())->first;
				}
			}
		}
	}

	void Agent::update()
	{
#if HRVO_DIFFERENTIAL_DRIVE
		const float averageWheelSpeed = 0.5f * (rightWheelSpeed_ + leftWheelSpeed_);
		const float wheelSpeedDifference = rightWheelSpeed_ - leftWheelSpeed_;

		position_ += simulator_->timeStep_ * averageWheelSpeed * Vector2(std::cos(orientation_), std::sin(orientation_));
		orientation_ += wheelSpeedDifference * simulator_->timeStep_ / wheelTrack_;
		velocity_ = averageWheelSpeed * Vector2(std::cos(orientation_), std::sin(orientation_));
#else /* HRVO_DIFFERENTIAL_DRIVE */

		const float dv = abs(newVelocity_ - velocity_);

		if (dv < maxAccel_ * simulator_->timeStep_) {
			velocity_ = newVelocity_;
		}
		else {
			velocity_ = (1.0f - (maxAccel_ * simulator_->timeStep_ / dv)) * velocity_ + (maxAccel_ * simulator_->timeStep_ / dv) * newVelocity_;
		}

		position_ += velocity_ * simulator_->timeStep_;
#endif /* HRVO_DIFFERENTIAL_DRIVE */


		if (absSq(simulator_->goals_[goalNo_]->position_ - position_) < goalRadius_ * goalRadius_) {
              reachedGoal_ = true;
		}
		else {
			reachedGoal_ = false;
			simulator_->reachedGoals_ = false;
		}

#if !HRVO_DIFFERENTIAL_DRIVE

		if (!reachedGoal_) {
			orientation_ = atan(prefVelocity_);
		}

#endif /* !HRVO_DIFFERENTIAL_DRIVE */
	}
}
