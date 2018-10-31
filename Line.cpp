#include "Line.h"


double Line::length() const{
	double dx = pt2.x - pt1.x;
	double dy = pt2.y - pt1.y;
	return sqrt(dx*dx + dy*dy);
}

bool Line::operator<=(const Line & another) const
{
	// just compare form pt1.x to pt1.y to... to pt2.y
	if (pt1.x > another.pt1.x)
		return true;
	else if (pt1.x < another.pt1.x)
		return false;
	else if (pt1.y > another.pt1.y)
		return true;
	else if (pt1.y < another.pt1.y)
		return false;
	else if (pt2.x > another.pt2.x)
		return true;
	else if (pt2.x < another.pt2.x)
		return false;
	else if (pt2.y > another.pt2.y)
		return true;
	else if (pt2.y < another.pt1.y)
		return false;
	else return true;
}

double Line::angle_between(Line * other) const
{
	double result = abs(theta - other->theta);

	if (result > CV_PI / 2.) result = CV_PI - result;

	return result;
}


std::ostream& operator<<(std::ostream& os, const Line& line) {
	os << line.pt1.x << ", " << line.pt1.y << ", " << line.pt2.x << ", " << line.pt2.y;
	os << ", " << line.rho << ", " << line.theta;
	return os;
}

// give intersection of two lines
// here we don't consider if two lines are parallel
// let the user to handle it
CvPoint intersection(const Line& line1, const Line& line2) {
	int x1 = line1.pt1.x;
	int y1 = line1.pt1.y;
	int x2 = line1.pt2.x;
	int y2 = line1.pt2.y;
	int x3 = line2.pt1.x;
	int y3 = line2.pt1.y;
	int x4 = line2.pt2.x;
	int y4 = line2.pt2.y;


	double u = double((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / double((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));

	double x = x1 + u * (x2 - x1);
	double y = y1 + u * (y2 - y1);

	x = round(x);
	y = round(y);

	return CvPoint(x, y);
}