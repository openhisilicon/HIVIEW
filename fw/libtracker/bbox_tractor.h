#ifndef __BBOX_TRACTOR_H__
#define __BBOX_TRACTOR_H__

#include<iostream>
#include<vector>
#include <algorithm>
#include <math.h>
#include "basetype.h"

using namespace std;
#ifdef __cplusplus       // 新加上的
extern "C" {
#endif
typedef struct BboxID
{
	int width;
	int heigth;
	int centerx;
	int centery;
	int ID;
}BboxID;


//typedef struct Tractor
//{
//	int num_Bbox;
//	vector<BboxID> Bbox;
//}Tractor;

class tractor
{
public:
	int trac_num;
	void ini(vector<BboxID> first); //第一帧初始化
	//dist_mat(vector<BboxID> cur);
	void track(vector<BboxID> &  cur);
	int tot_person;
	~tractor();

private:
	void displacement_matrix(vector<BboxID> cur);
	void match(vector<BboxID> cur);
	vector<BboxID> pre;
	vector<vector<float>> dist_mat;
	vector <vector<int>> matches;
	vector <int> unmatches;
};


int tracker_id(int FrameIndex, RESULT_BAG *result_bag, RESULT_BAG *result_out);

#ifdef __cplusplus     // 新加上的
}
#endif
#endif
