#pragma once

#include "ImageFeatures/inc/kdTreePoints.h"

interface KdTreeInter
{
	virtual void add(KdPointsContainer points) = 0;
	virtual size_t getSize() const = 0;
	virtual void match(KdPointsContainer kdPoints) const = 0;
	virtual void markNonBase(int time, int max_non_base_time) = 0;
	virtual bool contains(FeatInter* point) const = 0;
	virtual bool containsBase(FeatInter* point) const = 0;

	virtual ~KdTreeInter()
	{}
};
