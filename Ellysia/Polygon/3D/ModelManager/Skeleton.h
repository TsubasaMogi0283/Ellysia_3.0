#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <Joint.h>
#include <Node.h>

struct Skeleton {
	//RootJointのIndex
	int32_t root;
	//Joint名とIndexとの辞書
	std::map<std::string, int32_t>jointMap;
	//所属しているジョイント
	std::vector<Joint> joints;

};

//Nodeの階層構造からSkeletonを作る
Skeleton CreateSkeleton(const Node& rootNode);


//Skeletonの更新
void SkeletonUpdate(Skeleton& skeleton);