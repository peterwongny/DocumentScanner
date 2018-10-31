# ifndef LINE_H__
# define LINE_H__

#include <opencv2\opencv.hpp>
#include <iostream>

class Line
{
	friend std::ostream& operator<<(std::ostream& os, const Line& line);
	friend CvPoint intersection(const Line& line1, const Line& line2);

public:
	Line(CvPoint pt1, CvPoint pt2, double rho = 0, double theta = 0) : pt1(pt1), pt2(pt2),
		rho(rho), theta(theta) {}

	double length() const;

	bool operator<=(const Line& another) const;

	double angle_between(Line* other) const;

public:
	CvPoint pt1;
	CvPoint pt2;

	double rho;
	double theta;
};

std::ostream& operator<<(std::ostream& os, const Line& line);
CvPoint intersection(const Line& line1, const Line& line2);


#endif