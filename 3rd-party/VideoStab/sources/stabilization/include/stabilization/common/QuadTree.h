/*
 * QuadTree.h
 *
 *  Created on: 21.05.2012
 *      Author: Poruchik-bI
 */

#pragma once

namespace VideoLib {
namespace Common {


class QuadTree {
public:
	class Node{
	public:
		Node * child00;
		Node * child01;
		Node * child10;
		Node * child11;

		cv::Point2d   begin_point;
		cv::Size 	size;
		/* Пересечение */
		bool checkCross(Area const& area);
		/* Вписанная область */
		bool checkInscribedArea(Area const& area);
		/* Описанная область */
		bool checkDelineateArea(Area const& area);

	};
	QuadTree();
	virtual ~QuadTree();
};

} /* namespace Common */
} /* namespace VideoLib */
