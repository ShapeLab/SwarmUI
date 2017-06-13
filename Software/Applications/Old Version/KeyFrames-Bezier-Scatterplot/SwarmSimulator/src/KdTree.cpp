/*
 * KdTree.cpp
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
 * \file   KdTree.cpp
 * \brief  Defines the KdTree class.
 */

#ifndef HRVO_KD_TREE_H_
#include "KdTree.h"
#endif

#include <algorithm>
#include <limits>

#ifndef HRVO_AGENT_H_
#include "Agent.h"
#endif
#ifndef HRVO_DEFINITIONS_H_
#include "Definitions.h"
#endif
#ifndef HRVO_SIMULATOR_H_
#include "Simulator.h"
#endif

namespace hrvo {
	KdTree::KdTree(Simulator *simulator) : simulator_(simulator) { }

	void KdTree::build()
	{
		agents_.reserve(simulator_->agents_.size());

		for (std::size_t i = agents_.size(); i < simulator_->agents_.size(); ++i) {
			agents_.push_back(i);
		}

		nodes_.resize(2 * simulator_->agents_.size() - 1);

		if (!agents_.empty()) {
			buildRecursive(0, agents_.size(), 0);
		}
	}

	void KdTree::buildRecursive(std::size_t begin, std::size_t end, std::size_t node)
	{
		nodes_[node].begin_ = begin;
		nodes_[node].end_ = end;
		nodes_[node].minX_ = nodes_[node].maxX_ = simulator_->agents_[agents_[begin]]->position_.getX();
		nodes_[node].minY_ = nodes_[node].maxY_ = simulator_->agents_[agents_[begin]]->position_.getY();

		for (std::size_t i = begin + 1; i < end; ++i) {
			if (simulator_->agents_[agents_[i]]->position_.getX() > nodes_[node].maxX_) {
				nodes_[node].maxX_ = simulator_->agents_[agents_[i]]->position_.getX();
			}
			else if (simulator_->agents_[agents_[i]]->position_.getX() < nodes_[node].minX_) {
				nodes_[node].minX_ = simulator_->agents_[agents_[i]]->position_.getX();
			}

			if (simulator_->agents_[agents_[i]]->position_.getY() > nodes_[node].maxY_) {
				nodes_[node].maxY_ = simulator_->agents_[agents_[i]]->position_.getY();
			}
			else if (simulator_->agents_[agents_[i]]->position_.getY() < nodes_[node].minY_) {
				nodes_[node].minY_ = simulator_->agents_[agents_[i]]->position_.getY();
			}
		}

		if (end - begin > HRVO_MAX_LEAF_SIZE) {
			const bool vertical = nodes_[node].maxX_ - nodes_[node].minX_ > nodes_[node].maxY_ - nodes_[node].minY_;
			const float split = 0.5f * (vertical ?  nodes_[node].maxX_ + nodes_[node].minX_ : nodes_[node].maxY_ + nodes_[node].minY_);

			std::size_t left = begin;
			std::size_t right = end - 1;

			while (true) {
				while (left <= right && (vertical ? simulator_->agents_[agents_[left]]->position_.getX()
										 : simulator_->agents_[agents_[left]]->position_.getY()) < split) {
					++left;
				}

				while (right >= left && (vertical ? simulator_->agents_[agents_[right]]->position_.getX()
										 : simulator_->agents_[agents_[right]]->position_.getY()) >= split) {
					--right;
				}

				if (left > right) {
					break;
				}
				else {
					std::swap(agents_[left], agents_[right]);
					++left;
					--right;
				}
			}

			if (left == begin) {
				++left;
				++right;
			}

			nodes_[node].left_ = node + 1;
			nodes_[node].right_ = 2 * (left - begin) + node;

			buildRecursive(begin, left, nodes_[node].left_);
			buildRecursive(left, end, nodes_[node].right_);
		}
	}

	void KdTree::queryRecursive(Agent *agent, float &rangeSq, std::size_t node) const
	{
		if (nodes_[node].end_ - nodes_[node].begin_ <= HRVO_MAX_LEAF_SIZE) {
			for (std::size_t i = nodes_[node].begin_; i < nodes_[node].end_; ++i) {
				agent->insertNeighbor(agents_[i], rangeSq);
			}
		}
		else {
			float distSqLeft = 0.0f;
			float distSqRight = 0.0f;

			if (agent->position_.getX() < nodes_[nodes_[node].left_].minX_) {
				distSqLeft += sqr(nodes_[nodes_[node].left_].minX_ - agent->position_.getX());
			}
			else if (agent->position_.getX() > nodes_[nodes_[node].left_].maxX_) {
				distSqLeft += sqr(agent->position_.getX() - nodes_[nodes_[node].left_].maxX_);
			}

			if (agent->position_.getY() < nodes_[nodes_[node].left_].minY_) {
				distSqLeft += sqr(nodes_[nodes_[node].left_].minY_ - agent->position_.getY());
			}
			else if (agent->position_.getY() > nodes_[nodes_[node].left_].maxY_) {
				distSqLeft += sqr(agent->position_.getY() - nodes_[nodes_[node].left_].maxY_);
			}

			if (agent->position_.getX() < nodes_[nodes_[node].right_].minX_) {
				distSqRight += sqr(nodes_[nodes_[node].right_].minX_ - agent->position_.getX());
			}
			else if (agent->position_.getX() > nodes_[nodes_[node].right_].maxX_) {
				distSqRight += sqr(agent->position_.getX() - nodes_[nodes_[node].right_].maxX_);
			}

			if (agent->position_.getY() < nodes_[nodes_[node].right_].minY_) {
				distSqRight += sqr(nodes_[nodes_[node].right_].minY_ - agent->position_.getY());
			}
			else if (agent->position_.getY() > nodes_[nodes_[node].right_].maxY_) {
				distSqRight += sqr(agent->position_.getY() - nodes_[nodes_[node].right_].maxY_);
			}

			if (distSqLeft < distSqRight) {
				if (distSqLeft < rangeSq) {
					queryRecursive(agent, rangeSq, nodes_[node].left_);

					if (distSqRight < rangeSq) {
						queryRecursive(agent, rangeSq, nodes_[node].right_);
					}
				}
			}
			else {
				if (distSqRight < rangeSq) {
					queryRecursive(agent, rangeSq, nodes_[node].right_);

					if (distSqLeft < rangeSq) {
						queryRecursive(agent, rangeSq, nodes_[node].left_);
					}
				}
			}
		}
	}
}
