/*
 * KdTree.h
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
 * \file   KdTree.h
 * \brief  Declares the KdTree class.
 */

#ifndef HRVO_KD_TREE_H_
#define HRVO_KD_TREE_H_

#include <cstddef>
#include <vector>

#ifndef HRVO_VECTOR2_H_
#include "Vector2.h"
#endif

namespace hrvo {
	class Agent;
	class Simulator;

	/**
	 * \class  KdTree
	 * \brief  k-D trees for agents in the simulation.
	 */
	class KdTree {
	private:
		/**
		 * \class  Node
		 * \brief  Defines a k-D tree node.
		 */
		class Node {
		public:
			/**
			 * \brief  Constructor.
			 */
			Node() : begin_(0), end_(0), left_(0), right_(0), maxX_(0.0f), maxY_(0.0f), minX_(0.0f), minY_(0.0f) { }

			/**
			 * \brief  The beginning node number.
			 */
			std::size_t begin_;

			/**
			 * \brief  The ending node number.
			 */
			std::size_t end_;

			/**
			 * \brief  The left node number.
			 */
			std::size_t left_;

			/**
			 * \brief  The right node number.
			 */
			std::size_t right_;

			/**
			 * \brief  The maximum x-coordinate.
			 */
			float maxX_;

			/**
			 * \brief  The maximum y-coordinate.
			 */
			float maxY_;

			/**
			 * \brief  The minimum x-coordinate.
			 */
			float minX_;

			/**
			 * \brief  The minimum y-coordinate.
			 */
			float minY_;
		};

		/**
		 * \brief  The maximum leaf size of a k-D tree.
		 */
		static const std::size_t HRVO_MAX_LEAF_SIZE = 10;

		/**
		 * \brief      Constructor.
		 * \param[in]  simulator  The simulation.
		 */
		explicit KdTree(Simulator *simulator);

		/**
		 * \brief  Builds an agent k-D tree.
		 */
		void build();

		/**
		 * \brief  Recursive function to build a k-D tree.
		 * \param  begin  The beginning k-D tree node.
		 * \param  end    The ending k-D tree node.
		 * \param  node   The current k-D tree node.
		 */
		void buildRecursive(std::size_t begin, std::size_t end, std::size_t node);

		/**
		 * \brief      Computes the neighbors of the specified agent.
		 * \param[in]  agent    A pointer to the agent for which neighbors are to be computed.
		 * \param[in]  rangeSq  The squared range around the agent.
		 */
		void query(Agent *agent, float rangeSq) const
		{
			queryRecursive(agent, rangeSq, 0);
		}

		/**
		 * \brief          Recursive function to compute the neighbors of the specified agent.
		 * \param[in]      agent    A pointer to the agent for which neighbors are to be computed.
		 * \param[in,out]  rangeSq  The squared range around the agent.
		 * \param[in]      node     The current k-D tree node.
		 */
		void queryRecursive(Agent *agent, float &rangeSq, std::size_t node) const;

		Simulator *const simulator_;
		std::vector<std::size_t> agents_;
		std::vector<Node> nodes_;

		friend class Agent;
		friend class Simulator;
	};
}

#endif /* HRVO_KD_TREE_H_ */
