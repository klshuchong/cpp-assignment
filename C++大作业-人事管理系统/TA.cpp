//TA.cpp
//Created by Wang Jie (2022010554)
//TA类实现
#include <iostream>
#include <fstream>
#include "base.h"
#include "Derived_people.h"

const char TA::default_name[node::NAME_MAX_LENGTH] = "default TA name";

using std::ostream;
using std::endl;
using std::ofstream;

void TA::print(ostream& os) const
{
	graduate::print(os);
	os << "月薪：" << getpay() << endl;
}

void TA::fwrite(ofstream& ofs) const
{
	graduate::fwrite(ofs);
	unsigned int npay = getpay();
	ofs.write((char*)&npay, sizeof(unsigned int));
}