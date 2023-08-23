//Date.cpp
//Created by Wang Jie (2022010554)
#include <iostream>
#include "Date.h"
#include "Exception.h"

const int Date::days_normal[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const int Date::days_leap[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
bool Date::is_leap(int year)
{
	if (year % 100 != 0)return year % 4 == 0;
	else return year % 400 == 0;
}
Date::Date(int yy, int mm, int dd)
{
	if (mm < 1 || mm > 12)throw DateException(yy, mm, dd);
	if (is_leap(yy))
		if (dd < 1 || dd > days_leap[mm - 1])throw DateException(yy, mm, dd);
		else if (dd < 1 || dd > days_normal[mm - 1]) throw DateException(yy, mm, dd);
	y = yy; m = mm; d = dd;
}
using std::ostream;
using std::istream;
using std::cout;
ostream& operator<<(ostream& os, const Date& D)
{
	os << D.y << "/" << D.m << "/" << D.d;
	return os;
}
istream& operator>>(istream& is, Date& D)
{
	cout << "Enter the date(YYYY MM DD): ";
	int y, m, d;
	is >> y >> m >> d;
	if (m < 1 || m > 12) throw DateException(y, m, d);
	if (Date::is_leap(y))
		if (d < 1 || d > Date::days_leap[m - 1]) throw DateException(y, m, d);
		else if (d < 1 || d > Date::days_normal[m - 1]) throw DateException(y, m, d);
	D.y = y;
	D.m = m;
	D.d = d;
	return is;
}