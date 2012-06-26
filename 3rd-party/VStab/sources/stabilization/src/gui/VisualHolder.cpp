/*
 * VisalHolder.cpp
 *
 *  Created on: 28.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/gui/VisualHolder.h>

namespace GUI {

VisualHolder::VisualHolder(VideoLib::StabilizerPtr const& stabiliaer)
:m_stailizer(stabiliaer)
,m_capture(stabiliaer->getCapture())
{
	m_visual_flags.clear();

	m_visual_flags[DisplayFlags::ORIGINAL_WND] = ShowFlags::FRAME;
	m_visual_flags[DisplayFlags::GRAY_WND] = ShowFlags::NONE;
	m_visual_flags[DisplayFlags::STABILIZED_WND] = ShowFlags::NONE;

}

VisualHolder::~VisualHolder() {
	m_visual_flags.clear();
}

} /* namespace GUI */
