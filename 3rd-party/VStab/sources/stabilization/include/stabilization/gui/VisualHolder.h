/*
 * VisalHolder.h
 *
 *  Created on: 28.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stabilization/gui/Types.h>
#include <stabilization/Stabilizer.h>

#include <map>

namespace GUI {

class VisualHolder {
public:

	VisualHolder(VideoLib::StabilizerPtr const& stabiliaer);
	virtual ~VisualHolder();

	inline VideoLib::StabilizerPtr const& getStabilizer()const{return m_stailizer; }
	inline void setStabilizer(VideoLib::StabilizerPtr stabilizer){ m_stailizer = stabilizer; }

	inline VisualFlags const& getFlags()const{return m_visual_flags;}
	inline ShowFlag const& getFlags(DisplayFlag flag)const{return m_visual_flags.at(flag);}

	inline void setFlag(DisplayFlag const& ds_flag, ShowFlag const& sh_flag){m_visual_flags[ds_flag] = sh_flag;}
	inline void addFlag(DisplayFlag const& ds_flag, ShowFlag const& sh_flag){m_visual_flags[ds_flag] |= sh_flag;}
private:
	VideoLib::StabilizerPtr m_stailizer;
	VideoLib::FrameCapturePtr m_capture;

	VisualFlags m_visual_flags;

};

} /* namespace GUI */
