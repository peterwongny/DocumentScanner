// main.cpp : Defines the entry point for the application.
//
// to show the usage of the document detector

#include <iostream>
#include <string>
#include <ctime>
#include "Detector.h"

using namespace cv;
using namespace std;


void draw_quad(Mat& img, Point a, Point b, Point c, Point d) {
	cv::line(img, a, b, Scalar(0, 255, 255));
	cv::line(img, b, c, Scalar(0, 255, 255));
	cv::line(img, c, d, Scalar(0, 255, 255));
	cv::line(img, d, a, Scalar(0, 255, 255));

}

int main(int argc, char **argv)
{

	string model_filename = "../../model/model.yml";

	// create a detector object
	// loading machine learning model, takes around 3 seconds on my laptop
	Detector detector(model_filename);

	VideoCapture cap(1);
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	//Mat orig_edged, edged;
	namedWindow("edges", 1);
	for (;;)
	{
		std::clock_t    start;

		start = std::clock();

		Mat frame;
		cap >> frame; // get a new frame from camera
		
		Point a, b, c, d;


		// detect here, return four points of the document by reference
		detector.detect_doc(frame, a, b, c, d);

		draw_quad(frame, a, b, c, d);

		imshow("image", frame);

		std::cout << "Time: " << (double)(CLOCKS_PER_SEC) / (std::clock() - start) << std::endl;


		if (waitKey(30) >= 0) break;
	}
	return 0;
}
