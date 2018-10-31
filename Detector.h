

// Author Wong Ngo Yin
// 2018-06-21
/*
using machine learning algorithm for edge detection
than detect the straight lines and score the quadrilaterals
we use the implementation from opencv2\ximgproc in opencv_contrib

For more:
Structured Forests for Fast Edge Detection
http://openaccess.thecvf.com/content_iccv_2013/papers/Dollar_Structured_Forests_for_2013_ICCV_paper.pdf
Fast Edge Detection Using Structured Forests
https://arxiv.org/abs/1406.5549
*/

#ifndef DETECTOR_H__

# define DETECTOR_H__


# include <string>
# include <opencv2\opencv.hpp>
# include <opencv2\ximgproc\structured_edge_detection.hpp>

# include "Line.h"
# include "Quadrilateral.h"

using namespace std;
using namespace cv;
using namespace cv::ximgproc;

class Detector
{
public:
	Detector(string model_filename) {
		pDollar = createStructuredEdgeDetection(model_filename);
		edge_binary_thresh = 0.3;
		hough_thresh = 20;
		min_angle_diff = 3.5;
		max_lines = 10;
		max_quads = 120;
		image_h = 200;
		image_w = 200;
	}

	// the first point is the upper left one, in clockwise direction
	void detect_doc(Mat& img, Point& a, Point& b, Point& c, Point& d);
	//void detect_doc(Mat& image, Mat& edged, Mat& orig_edged);


private:
	Ptr<StructuredEdgeDetection> pDollar;

	// control how many positive pixels for hough transform
	double edge_binary_thresh;
	// control how many pixels are considered straight line in hough transform
	int hough_thresh;
	// minimum angle of the corner of the target
	double min_angle_diff;
	// maximum lines considered, due to compute limit
	int max_lines;
	// maximum lines considered, due to compute limit
	int max_quads;

	// image height will be compressed to
	int image_h;
	// image width will be compressed to
	int image_w;


// helpers 
private:
	void merge_lines(vector<Vec2f>& hough_lines, vector<Line>& lines);
	void find_intersections(vector<Line>& lines, map<pair<Line*, Line*>, Point>& intersections);
	void find_quads(vector<Line>& lines, map<pair<Line*, Line*>, Point>& intersections, vector<Quadrilateral>& quads);
};


#endif // !DETECTOR_H__