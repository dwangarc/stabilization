#include "stdafx.h"
#include <afxwin.h>
#include "../RegistryParamsSaver.h"
#include "Include/different_libs.h"


double RegistryParamsSaver::uintToDouble(int val, double median, double koeff) const
{
	return ((double)val) / koeff - median;
}

int RegistryParamsSaver::doubleToUint(double val, double median, double koeff) const
{
	return (int)((val + median) * koeff);
}

double RegistryParamsSaver::loadDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double defaultVal) const
{
	int val = AfxGetApp()->GetProfileInt(lpszSection,	lpszEntry, doubleToUint(defaultVal, 10000, 10000));
	return uintToDouble(val, 10000, 10000);
}

void RegistryParamsSaver::saveDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double val) const
{
	AfxGetApp()->WriteProfileInt(lpszSection,	lpszEntry, doubleToUint(val, 10000, 10000));
}

int RegistryParamsSaver::loadInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int defaultVal) const
{
	return AfxGetApp()->GetProfileInt(lpszSection,	lpszEntry, defaultVal);
}

void RegistryParamsSaver::saveInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int val) const
{
	AfxGetApp()->WriteProfileInt(lpszSection,	lpszEntry, val);
}

std::string RegistryParamsSaver::loadString(LPCTSTR lpszSection, LPCTSTR lpszEntry, std::string const& defaultVal) const
{
	return dif_lib::CStrToStr(AfxGetApp()->GetProfileString(lpszSection,	lpszEntry, dif_lib::s2ws(defaultVal).c_str()));
}

void RegistryParamsSaver::saveString(LPCTSTR lpszSection, LPCTSTR lpszEntry, std::string const& val) const
{
	AfxGetApp()->WriteProfileString(lpszSection,	lpszEntry, dif_lib::s2ws(val).c_str());
}
