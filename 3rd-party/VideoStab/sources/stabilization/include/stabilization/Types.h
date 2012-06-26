/*
 * Types.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

namespace Stabilization{

namespace MotionModes{
enum _model
{
	MOTION_BASE = 0,
	MOTION_STAYED = 1,
	MOTION_MOVED = 2
};
} /*MotionModes*/
typedef MotionModes::_model MotionModel;

namespace ModeTypes{
enum _type
{
	STABILIZE = 0,
	UN_STABILIZE = 1,
	ANALYZE = 2,
	BASE = 3,
	MOTION_DETECTION = 4,
	TRACK = 5,
	MAX_MODE_TYPE = 6
};
} /*ModeTypes*/
typedef ModeTypes::_type ModeType;



} /*Stabilization*/


