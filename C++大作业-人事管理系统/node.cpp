//node.cpp
//Created by Wang Jie (2022010554)
//node类实现
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <set>
#include <memory>
#include "base.h"
#include "Exception.h"

unsigned int node::max_uid = 0;
const char node::default_name[node::NAME_MAX_LENGTH] = "default node name";

using std::string;
using std::ifstream;
using std::ofstream;
using std::shared_ptr;
using std::weak_ptr;

node::node(const string& n_name, NodeType n_node_type) : node_type(n_node_type), father()
{
	//设置名称
	strcpy_s(name, n_name.c_str());

	//设置uid并更新max_uid
	uid = max_uid;
	max_uid++;
}

node::node(ifstream& ifs, NodeType n_node_type) :node_type(n_node_type)
{
	//读取名称
	ifs.read(name, NAME_MAX_LENGTH);

	//读取uid
	ifs.read((char*)&uid, sizeof(int));

	//读取父节点uid
	ifs.read((char*)&father_uid, sizeof(int));

	//暂时将父节点设为空指针
	father = shared_ptr<department>();
}

bool node::setfather(const shared_ptr<department>& n_father)
{
	//设置父节点uid并更新父亲节点中孩子节点的信息，若父节点为空，则设uid为-1
	if (n_father)
	{
		if (n_father->child.insert(shared_from_this()).second)//若插入父节点成功
		{
			//更新原父节点中孩子节点的信息
			if (father.lock())father.lock()->child.erase(shared_from_this());

			father_uid = n_father->getuid();
			father = n_father;
			return true;
		}
		else return false;
	}
	else
	{
		father_uid = -1;
		if (father.lock())
		{
			father.lock()->child.erase(shared_from_this());
			father = shared_ptr<department>();
		}
		return true;
	}
}

void node::fwrite(ofstream& ofs) const
{
	ofs.write(name, NAME_MAX_LENGTH);
	ofs.write((char*)&uid, sizeof(int));
	ofs.write((char*)&father_uid, sizeof(int));
}

bool defaultNodeCmp::operator()(const std::shared_ptr<node>& left, const std::shared_ptr<node>& right) const
{
	if (left->getnodetype() == node::NodeType::Department && right->getnodetype() == node::NodeType::Department)
		return left->getname() < right->getname();
	else if (left->getnodetype() == node::NodeType::Department && (int)(right->getnodetype()) > 1)
		return true;
	else if (right->getnodetype() == node::NodeType::Department && (int)(left->getnodetype()) > 1)
		return false;
	else return left->getuid() < right->getuid();//由于uid和people_uid严格正相关，所以这里可换成按uid排序

}
