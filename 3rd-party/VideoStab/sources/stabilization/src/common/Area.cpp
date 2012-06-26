/*
 * Area.cpp
 *
 *  Created on: 21.05.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/common/Area.h>

namespace VideoLib {
namespace Common {

Area::Area(size_t const& n){
	m_points.reserve(n);
	m_unit_vectors.reserve(n);
}
Area::Area(Area::Points const& points, bool const& cals_vecs = true)
:m_points(points)
{
	calcUnitVecs();
}
Area::Area(Area::Points const& points, Area::Points const& unit_vectors)
:m_points(points)
,m_unit_vectors(unit_vectors)
{
}

Area::~Area(){
}

void Area::calcUnitVecs(){

	m_unit_vectors.clear();

	Area::Points::iterator it1 = m_points.begin();
	Area::Points::iterator it2;
	for(Area::Points::iterator it = m_points.begin(); it != m_points.end();it++){
		it2 = it1 + 1;
		if(it2 == m_points.end()){
			it2 = m_points.begin();
		}
		m_unit_vectors.push_back(Lines::unitVec(*it1,*it2));
	}
}

} /* namespace Common */
} /* namespace VideoLib */
