/*
 * Area.h
 *
 *  Created on: 21.05.2012
 *      Author: Poruchik-bI
 */

#pragma once

namespace VideoLib {
namespace Common {

class Area {
public:
	typedef cv::Vector<cv::Point2d> Points;
	typedef cv::Vector<cv::Point2d> UnitVecs;

	Area(size_t const& n);
	Area(Points const& points, bool const& cals_vecs = true);
	Area(Points const& points, Points const& unit_vectors);

	virtual ~Area();

	void calcUnitVecs();

	inline Points & points() {return m_points;}
	inline Points const& points()const {return m_points;}

	inline UnitVecs & unitVectors() {return m_unit_vectors;}
	inline UnitVecs const& unitVectors()const {return m_unit_vectors;}

	inline size_t numPoints(){ return m_points.size(); }
private:
	Points m_points;
	UnitVecs m_unit_vectors;
};

typedef std::vector<Area> Areas;

} /* namespace Common */
} /* namespace VideoLib */
