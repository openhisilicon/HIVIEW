//
// Created by dingjian on 18-2-3.
//

#ifndef POLYIOU_POLYIOU_H
#define POLYIOU_POLYIOU_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

//#include <vector>
//double iou_poly(std::vector<double> p, std::vector<double> q);

double poly_iou(double p[10*2], int pn, double q[10*2], int qn);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //POLYIOU_POLYIOU_H