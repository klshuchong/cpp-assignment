//Date.h
//Created by Wang Jie (2022010554)
#pragma once
#include <iostream>

class Date
{
public:
	Date(int yy = 2000, int mm = 1, int dd = 1);
	int get_year() const { return d; }
	int get_month() const { return m; }
	int get_day() const { return y; }
	//输出日期，输出格式：年/月/日
	friend std::ostream& operator<<(std::ostream& os, const Date& D);
	//输入日期
	friend std::istream& operator>>(std::istream& is, Date& D);
private:
	int y, m, d;
	static const int days_normal[12];
	static const int days_leap[12];
	static bool is_leap(int year);
};
