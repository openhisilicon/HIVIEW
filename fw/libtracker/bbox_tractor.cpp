#include "bbox_tractor.h"

static vector<BboxID> cur;
static tractor det_tractor;

void tractor::ini(vector<BboxID> first)
{
	pre = first;
	tot_person = 0;
}

void tractor::displacement_matrix(vector<BboxID>  cur)
{
	int r = cur.size();
	int c = pre.size();
	vector<vector<float>> dist(r);
	for (int i = 0; i < r; i++)
		dist[i].resize(c);
	for (int i = 0; i < cur.size(); i++)
	{
		for (int j = 0; j < pre.size(); j++)
		{
			dist[i][j] = pow(cur[i].centerx - pre[j].centerx, 2) + pow(cur[i].centery - pre[j].centery, 2);
			if (dist[i][j] > std::min(cur[i].heigth*cur[i].width, pre[j].heigth*pre[j].heigth)*0.5)
			{
				dist[i][j] = 1000000;
			}
		}
	}
	dist_mat = dist;
}

void tractor::match(vector<BboxID> cur)
{
	for (int i = 0; i < cur.size(); i++)
	{
		vector<float>::iterator smallest;
		smallest = min_element(std::begin(dist_mat[i]), std::end(dist_mat[i]));

		int n = distance(std::begin(dist_mat[i]), smallest);
		if (dist_mat[i][n] == 1000000)
		{
			unmatches.push_back(i);  ////cur[i] 和matches[i]
			continue;														//十分依赖于检测效果！如果出现重叠框误检，容易带跑ID，导致ID上涨
		}
		for (int j = 0; j < cur.size(); j++)
			dist_mat[j][n] = 1000000;
		vector<int> match = { i, n };
		/*if (n > max_matches)
		{
		max_matches = n;
		}*/
		matches.push_back(match);
	}
}

void tractor::track(vector<BboxID> & cur)
{
	displacement_matrix(cur);
	match(cur);
	int k = 0;
	//tot_person = tot_person + cur.size() - matches.size();
	for (int i = 0; i < cur.size(); i++)
	{
		if (i < matches.size())
		{
			cur[matches[i][0]].ID = pre[matches[i][1]].ID;
			//cv::imshow("result frame", frame);
			//int key = waitKey(1);
		}
		else
		{
			cur[unmatches[k]].ID = tot_person + k;
			k++;
		}
	}
	tot_person = tot_person + unmatches.size();
	pre = cur;
	matches.clear();
	unmatches.clear();
	dist_mat.clear();
}

tractor::~tractor()
{
	std::cout << "tracking finished" << std::endl;
}

int tracker_id(int FrameIndex, RESULT_BAG *result_bag , RESULT_BAG *result_out)
{
	if(result_bag->obj_num > 0)
	{
		for (int i = 0; i < result_bag->obj_num; i++)
		{
			BboxID box = { result_bag->obj[i].width,result_bag->obj[i].heigth, result_bag->obj[i].centerx,result_bag->obj[i].centery, i };
			cur.push_back(box);
		}
		if (FrameIndex == 1)
		{
			det_tractor.ini(cur);
			det_tractor.tot_person = det_tractor.tot_person + cur.size();
		}
		else
		{
			det_tractor.track(cur);
			for (int i = 0; i < cur.size(); i++)
			{
				int x = cur[i].centerx - cur[i].width / 2;
				int y = cur[i].centery - cur[i].heigth / 2;
				int w = cur[i].width;
				int h = cur[i].heigth;
				result_out->obj[i].centerx = cur[i].centerx;
				result_out->obj[i].centery = cur[i].centery;
				result_out->obj[i].heigth = cur[i].heigth;
				result_out->obj[i].width = cur[i].width;
				result_out->obj[i].ID = cur[i].ID;
				result_out->obj_num++;
			}
		}
	}

	cur.clear();
}



