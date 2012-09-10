#pragma once

struct FeatInter;

interface Checker
{
	virtual bool contains(FeatInter* point) const = 0;
	virtual bool containsBase(FeatInter* point) const = 0;
};