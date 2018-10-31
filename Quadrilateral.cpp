#include "Quadrilateral.h"

using namespace std;
using namespace cv;

std::ostream& operator<<(std::ostream& os, const Quadrilateral& quad) {
	os << quad.pt1.x << ", " << quad.pt1.y << ", ";
	os << quad.pt2.x << ", " << quad.pt2.y << ", ";
	os << quad.pt3.x << ", " << quad.pt3.y << ", ";
	os << quad.pt4.x << ", " << quad.pt4.y;
	return os;
}

double Quadrilateral::perimeter() const
{
	Line line_a(pt1, pt2);
	Line line_b(pt2, pt3);
	Line line_c(pt3, pt4);
	Line line_d(pt4, pt1);
	return line_a.length() + line_b.length() + line_c.length() + line_d.length();
}


int orientation3Points(CvPoint pt1, CvPoint pt2, CvPoint pt3) {
	int sign = ((pt2.x - pt1.x) * (pt3.y - pt1.y) - (pt2.y - pt1.y) * (pt3.x - pt1.x));
	return sign > 0 ? 1 : (sign < 0 ? -1 : 0);
}


bool operator==(Point a, Point b) {
	if (a.x == b.x && a.y == b.y) return true;
	return false;
}

bool Quadrilateral::isConvex()
{
	// if any of the corner is identical
	if (pt1 == pt2 || pt2 == pt3 || pt3 == pt4 || pt4 == pt1) return false;

	int orientation1 = orientation3Points(pt1, pt2, pt3);

	if (orientation1 != 0) {
		if (orientation1 == orientation3Points(pt2, pt3, pt4)) {
			if (orientation1 == orientation3Points(pt3, pt4, pt1)) {
				if (orientation1 == orientation3Points(pt4, pt1, pt2)) {
					// modify the points so they are rotated clockwise
					if (orientation1 == -1) {
						Point t = pt4;
						pt4 = pt2;
						pt2 = t;
					}
					return true;
				}
			}
		}
	}
	return false;
}


//void pixel_under_line_segment(CvPoint pt1, CvPoint pt2, int& pixel_num, double& pixel_sum, const Mat& img) {
//	int x1 = pt1.x;
//	int	y1 = pt1.y;
//	int	x2 = pt2.x;
//	int	y2 = pt2.y;
//
//	pixel_sum = 0;
//	pixel_num = 0;
//
//	vector<CvPoint> pixels;
//	bool steep = abs(y2 - y1) > abs(x2 - x1);
//
//	if (steep) {
//		int t = x1;
//		x1 = y1;
//		y1 = t;
//
//		t = x2;
//		x2 = y2;
//		y2 = t;
//	}
//
//
//	if (x1 > x2) {
//		int t = x1;
//		x1 = x2;
//		x2 = t;
//
//		t = y1;
//		y1 = y2;
//		y2 = t;
//	}
//
//
//	int dx = x2 - x1;
//	int dy = y2 - y1;
//
//	//if (dx == 0) return;
//
//	double gradient = dy / dx;
//
//	
//
//	double x_end = round(x1);
//	double y_end = y1 + (gradient * (x_end - x1));
//	double xpxl0 = x_end;
//	double ypxl0 = round(y_end);
//	if (steep) {
//		pixels.push_back(CvPoint(ypxl0, xpxl0));
//		pixels.push_back(CvPoint(ypxl0 + 1, xpxl0));
//	}
//	else {
//		pixels.push_back(CvPoint(xpxl0, ypxl0));
//		pixels.push_back(CvPoint(xpxl0, ypxl0 + 1));
//	}
//
//	double interpolated_y = y_end + gradient;
//
//	x_end = round(x2);
//	y_end = y2 + (gradient * (x_end - x2));
//	double xpxl1 = x_end;
//	double ypxl1 = round(y_end);
//
//	for (int x = xpxl0 + 1; x < xpxl1; x++) {
//		if (steep) {
//			pixels.push_back(CvPoint(floor(interpolated_y), x));
//			pixels.push_back(CvPoint(floor(interpolated_y) + 1, x));
//		}
//		else {
//			pixels.push_back(CvPoint(x, floor(interpolated_y)));
//			pixels.push_back(CvPoint(x, floor(interpolated_y) + 1));
//		}
//		interpolated_y += gradient;
//	}
//
//	if (steep) {
//		pixels.push_back(CvPoint(ypxl1, xpxl1));
//		pixels.push_back(CvPoint(ypxl1 + 1, xpxl1));
//	}
//	else {
//		pixels.push_back(CvPoint(xpxl1, ypxl1));
//		pixels.push_back(CvPoint(xpxl1, ypxl1 + 1));
//	}
//
//	for (vector<CvPoint>::iterator it = pixels.begin(); it != pixels.end(); it++) {
//		try {
//			if ((*it).x < img.cols && (*it).x >= 0 &&
//				(*it).y < img.rows && (*it).y >= 0) {
//				++pixel_num;
//				pixel_sum += img.at<float>((*it).y, (*it).x);
//			}
//		}
//		catch (exception e) {
//			cout << (*it).y << ", " << (*it).x << endl;
//		}
//	}
//
//	return;
//
//}

// this one using line iteratior
void pixel_under_line_segment2(CvPoint pt1, CvPoint pt2, int& pixel_num, double& pixel_sum, const Mat& img) {
	LineIterator it(img, pt1, pt2, 8);
	pixel_num = it.count;
	pixel_sum = 0;
	Vec3b t;
	for (int i = 0; i < it.count; i++, it++) {
		t = Vec3b(*it);
		pixel_sum += t[0];
	}
	
}


double Quadrilateral::score(const cv::Mat& img) const
{	
	Quadrilateral quad = *this;

	int pixel_num = 0;
	double pixel_sum = 0;

	int pixel_num_all = 0;
	double pixel_sum_all = 0;

	pixel_under_line_segment2(quad.pt1, quad.pt2, pixel_num, pixel_sum, img);
	pixel_num_all += pixel_num;
	pixel_sum_all += pixel_sum;

	pixel_under_line_segment2(quad.pt2, quad.pt3, pixel_num, pixel_sum, img);
	pixel_num_all += pixel_num;
	pixel_sum_all += pixel_sum;

	pixel_under_line_segment2(quad.pt3, quad.pt4, pixel_num, pixel_sum, img);
	pixel_num_all += pixel_num;
	pixel_sum_all += pixel_sum;

	pixel_under_line_segment2(quad.pt4, quad.pt1, pixel_num, pixel_sum, img);
	pixel_num_all += pixel_num;
	pixel_sum_all += pixel_sum;

	double score = 0;

	if (pixel_num_all == 0) return 0;

	// using img
	score += pixel_sum_all / pixel_num_all;

	// chenck if the inside angles are close to pi/2
	//double angle1 = line1->angle_between(line2);
	//double angle2 = line1->angle_between(line3);
	//double angle3 = line1->angle_between(line4);
	//double angle4 = line1->angle_between(line1);

	//double right_angle = CV_PI / 2.0;

	//double error = abs(right_angle - angle1) + abs(right_angle - angle2) + 
	//	abs(right_angle - angle3) + abs(right_angle - angle4);

	//score = score - error ;

	return score;
}
