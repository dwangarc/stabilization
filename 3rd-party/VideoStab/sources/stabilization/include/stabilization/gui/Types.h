/*
 * VisalHolder.h
 *
 *  Created on: 28.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stdint.h>
#include <map>

namespace GUI {

namespace ShowFlags{

	typedef uint_fast32_t show_flag;

	static const show_flag	NONE 					= 0x00000000;
	static const show_flag	FRAME					= 0x00000001;
	static const show_flag	PREV_FEATURES			= 0x00000002;
	static const show_flag	CURR_FEATURES			= 0x00000004;
	static const show_flag	OPTICAL_FLOW			= 0x00000008;
	static const show_flag	PROJ_TRANSFORM			= 0x00000010;
	static const show_flag	BUFFER_MAP				= 0x00000020;

	static const show_flag	SHIFT_TRACK				= 0x00000040;
	static const show_flag	SCALE_TRACK				= 0x00000080;
	static const show_flag	ROTATION_TRACK			= 0x00000100;
	static const show_flag	KALMAN_SHIFT_TRACK		= 0x00000200;
	static const show_flag	KALMAN_SCALE_TRACK		= 0x00000400;
	static const show_flag	KALMAN_ROTATION_TRACK	= 0x00000800;

} /* namespace ShowFlags */
typedef ShowFlags::show_flag ShowFlag;

namespace DisplayFlags{

	typedef uint_fast32_t display_flag;

	static const display_flag	NONE 				= 0x00000001;
	static const display_flag	ORIGINAL_WND 		= 0x00000002;
	static const display_flag	GRAY_WND			= 0x00000004;
	static const display_flag	STABILIZED_WND		= 0x00000008;

} /*namespace DisplayFlags*/
typedef DisplayFlags::display_flag DisplayFlag;

typedef std::map<DisplayFlag,ShowFlag> VisualFlags;

} /* namespace GUI */
