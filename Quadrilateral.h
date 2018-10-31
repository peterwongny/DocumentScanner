#ifndef QUADRILATERAL_H__
#define QUADRILATERAL_H__



#include <opencv2\opencv.hpp>
#include <iostream>
#include "Line.h"


class Quadrilateral
{
	friend std::ostream& operator<<(std::ostream& os, const Quadrilateral& quad);

public:
	Quadrilateral(CvPoint pt1, CvPoint pt2, CvPoint pt3, CvPoint pt4, Line* line1, Line* line2,
		Line* line3, Line* line4) : pt1(pt1), pt2(pt2), pt3(pt3), pt4(pt4),
		line1(line1), line2(line2), line3(line3), line4(line4) {}

	double perimeter() const; 

	bool isConvex();

	double score(const cv::Mat& img) const;

public:
	CvPoint pt1;
	CvPoint pt2;
	CvPoint pt3;
	CvPoint pt4;

	Line* line1;
	Line* line2;
	Line* line3;
	Line* line4;
};

std::ostream& operator<<(std::ostream& os, const Quadrilateral& quad);




#endif // !QUADRILATERAL_H__