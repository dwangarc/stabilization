#pragma once

#include <WTypes.h>
#include <tchar.h>
#include <string>

class RegistryParamsSaver
{
	double uintToDouble(int val, double median, double koeff) const;
	int doubleToUint(double val, double median, double koeff) const;
public:
	double loadDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double defaultVal) const;
	void saveDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double val) const;

	int loadInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int defaultVal) const;
	void saveInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int val) const;

	std::string loadString(LPCTSTR lpszSection, LPCTSTR lpszEntry, std::string const& defaultVal) const;
	void saveString(LPCTSTR lpszSection, LPCTSTR lpszEntry, std::string const&val) const;
};