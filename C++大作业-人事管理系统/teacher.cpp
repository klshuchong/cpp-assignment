//teacher.cpp
//Created by Wang Jie (2022010554)
//teacher类实现
#include <iostream>
#include <fstream>
#include "base.h"
#include "Derived_people.h"

const char teacher::default_name[node::NAME_MAX_LENGTH] = "default teacher name";

using std::ostream;
using std::endl;
using std::ifstream;
using std::ofstream;

teacher::teacher(ifstream& ifs) : People(ifs, node::NodeType::Teacher)
{
	ifs.read((char*)&pay, sizeof(unsigned int));
}

void teacher::print(ostream& os) const
{
	People::print(os);
	os << "月薪：" << pay << endl;
}

void teacher::fwrite(ofstream& ofs) const
{
	People::fwrite(ofs);
	ofs.write((char*)&pay, sizeof(unsigned int));
}

