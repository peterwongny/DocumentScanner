#include "Detector.h"
#include <map>


void sort_points(Point& a, Point& b, Point& c, Point& d);

void Detector::merge_lines(vector<Vec2f>& hough_lines, vector<Line>& lines) {
	for (size_t i = 0; i < hough_lines.size(); i++)
	{
		// limit the number of lines
		// this can be done because lines are sorted in descending confidence
		if (lines.size() > max_lines) break;

		bool diff = true;
		float rho = hough_lines[i][0], theta = hough_lines[i][1];

		for (vector<Line>::iterator it = lines.begin(); it != lines.end(); it++) {
			if (abs(rho - (*it).rho) < 20 && abs(theta - (*it).theta) < CV_PI / 10.0) {
				diff = false; break;
			}
			if (abs(-rho - (*it).rho) < 20 && abs(theta - (*it).theta - CV_PI) < CV_PI / 10.0) {
				diff = false; break;
			}
			if (abs(-rho - (*it).rho) < 20 && abs(theta - (*it).theta + CV_PI) < CV_PI / 10.0) {
				diff = false; break;
			}
		}

		if (!diff) continue;

		Point pt1, pt2;


		if (theta < CV_PI / 4. || theta > 3. * CV_PI / 4.) {
			pt1 = Point(cvRound(rho / cos(theta)), 0);
			pt2 = Point(cvRound((rho - image_h * sin(theta))) / cos(theta), image_h);
		}
		else {
			pt1 = Point(0, cvRound(rho / sin(theta)));
			pt2 = Point(image_w, cvRound((rho - image_w * cos(theta)) / sin(theta)));
		}

		//draw
		//cv::line(image, pt1, pt2, Scalar(0, 255, 0));

		lines.push_back(Line(pt1, pt2, rho, theta));
	}

}

void Detector::find_intersections(vector<Line>& lines, map<pair<Line*, Line*>, Point>& intersections)
{
	for (auto it1 = lines.begin(); it1 != lines.end(); it1++) {
		for (auto it2 = lines.begin(); it2 != lines.end(); it2++) {

			double angle_between = abs(it1->theta - it2->theta);
			if (angle_between < CV_PI / min_angle_diff ||
				angle_between >(min_angle_diff - 1) * CV_PI / min_angle_diff) {
				continue;
			}
			Point in_p = intersection((*it1), (*it2));
			//// order 
			//if ((*it1) <= (*it2)) {
			//	intersections.insert(pair<pair<Line*, Line*>, Point>(pair<Line*, Line*>(&(*it1), &(*it2)), in_p));
			//}
			//else {
			//	intersections.insert(pair<pair<Line*, Line*>, Point>(pair<Line*, Line*>(&(*it2), &(*it1)), in_p));
			//}
			intersections.insert(pair<pair<Line*, Line*>, Point>(pair<Line*, Line*>(&(*it1), &(*it2)), in_p));

			// try to draw the intersections
			//circle(image, in_p, 2, Scalar(0, 0, 255));

		}
	}

}

void Detector::find_quads(vector<Line>& lines, map<pair<Line*, Line*>, Point>& intersections, vector<Quadrilateral>& quads)
{
	for (auto ita = lines.begin(); ita != lines.end() - 3; ita++) {
		for (auto itb = lines.begin() + 1; itb != lines.end() - 2; itb++) {
			for (auto itc = lines.begin() + 2; itc != lines.end() - 1; itc++) {
				for (auto itd = lines.begin() + 3; itd != lines.end(); itd++) {
					// we have a the combinations now

					// limit the number of quads
					// this can be done because lines are sorted in descending confidence
					if (quads.size() > max_quads) break;

					// consider different order
					for (int i = 0; i != 3; i++) {
						vector<Line>::iterator it1, it2, it3, it4;
						if (i == 0) {
							it1 = ita;
							it2 = itb;
							it3 = itc;
							it4 = itd;
						}
						else if (i == 1) {
							it1 = ita;
							it2 = itb;
							it3 = itd;
							it4 = itc;
						}
						else if (i == 2) {
							it1 = ita;
							it2 = itc;
							it3 = itb;
							it4 = itd;
						}

						map<pair<Line*, Line*>, Point>::iterator it_c1, it_c2, it_c3, it_c4;
						//if ((*it1) <= (*it2)) it_c1 = intersections.find(pair<Line*, Line*>(&(*it1), &(*it2)));
						//else it_c1 = intersections.find(pair<Line*, Line*>(&(*it2), &(*it1)));

						//if ((*it2) <= (*it3)) it_c2 = intersections.find(pair<Line*, Line*>(&(*it2), &(*it3)));
						//else it_c2 = intersections.find(pair<Line*, Line*>(&(*it3), &(*it2)));

						//if ((*it3) <= (*it4)) it_c3 = intersections.find(pair<Line*, Line*>(&(*it3), &(*it4)));
						//else it_c3 = intersections.find(pair<Line*, Line*>(&(*it4), &(*it3)));

						//if ((*it4) <= (*it1)) it_c4 = intersections.find(pair<Line*, Line*>(&(*it4), &(*it1)));
						//else it_c4 = intersections.find(pair<Line*, Line*>(&(*it1), &(*it4)));


						it_c1 = intersections.find(pair<Line*, Line*>(&(*it1), &(*it2)));
						if (it_c1 == intersections.end()) it_c1 = intersections.find(pair<Line*, Line*>(&(*it2), &(*it1)));

						it_c2 = intersections.find(pair<Line*, Line*>(&(*it2), &(*it3)));
						if (it_c2 == intersections.end()) it_c2 = intersections.find(pair<Line*, Line*>(&(*it3), &(*it2)));

						it_c3 = intersections.find(pair<Line*, Line*>(&(*it3), &(*it4)));
						if (it_c3 == intersections.end()) it_c3 = intersections.find(pair<Line*, Line*>(&(*it4), &(*it3)));

						it_c4 = intersections.find(pair<Line*, Line*>(&(*it4), &(*it1)));
						if (it_c4 == intersections.end()) it_c4 = intersections.find(pair<Line*, Line*>(&(*it1), &(*it4)));


						if (it_c1 == intersections.end() ||
							it_c2 == intersections.end() ||
							it_c3 == intersections.end() ||
							it_c4 == intersections.end()) continue;

						Quadrilateral a_quad(it_c1->second, it_c2->second,
							it_c3->second, it_c4->second,
							&(*it1), &(*it2), &(*it3), &(*it4));

						if (a_quad.isConvex()) { // if it is a proper convex quadrilateral
							if (a_quad.perimeter() > image_w * 0.5) { // if it is big enough
								double angle_between = max(abs(it1->theta - it3->theta),
									abs(it2->theta - it4->theta)); // if the opposite size is parallel enough
								if (angle_between < CV_PI / min_angle_diff ||
									angle_between >(min_angle_diff - 1) * CV_PI / min_angle_diff) {
									quads.push_back(a_quad);

									// draw the quadrilateral

									//cv::line(image, it_c1->second, it_c2->second, Scalar(255, 0, 0));
									//cv::line(image, it_c2->second, it_c3->second, Scalar(255, 0, 0));
									//cv::line(image, it_c3->second, it_c4->second, Scalar(255, 0, 0));
									//cv::line(image, it_c4->second, it_c1->second, Scalar(255, 0, 0));

								}
							}

							break; // only one possible quadrilateral for every combination

						}
					}
				}
			}
		}
	}
}

void Detector::detect_doc(Mat& img, Point& a, Point& b, Point& c, Point& d)
{
	Mat image, orig_edged, edged;

	img.copyTo(image);

	// resize input to image
	resize(image, image, Size(image_w, image_h));

	// edge detection
	image.convertTo(image, DataType<float>::type, 1 / 255.0);

	pDollar->detectEdges(image, orig_edged);

	// not maximun suppression
	Mat orient, suppressed_edged;
	pDollar->computeOrientation(orig_edged, orient);
	pDollar->edgesNms(orig_edged, orient, suppressed_edged);

	// binarize
	threshold(suppressed_edged, edged, edge_binary_thresh, 255, THRESH_BINARY);

	edged.convertTo(edged, CV_8U);

	vector<Vec2f> hough_lines;
	HoughLines(edged, hough_lines, 1, CV_PI / 180, hough_thresh, 0, 0);


	// clustering
	vector<Line> lines;
	merge_lines(hough_lines, lines);

	// no quad possible, if not return, error in the following steps
	// can be inserted into find_quads, but put it here can also jump through find_intersections
	if (lines.size() < 4)
		return;

	// find intersections
	map<pair<Line*, Line*>, Point> intersections;
	find_intersections(lines, intersections);

	// finding strictly convex quadrilaterals
	vector<Quadrilateral> quads;
	find_quads(lines, intersections, quads);

	//cout << quads.size() << endl;

	// find the quad with best score
	if (quads.empty()) return;

	Quadrilateral best_quad = quads.front();
	double max_score = 0.0;

	for (auto it = quads.begin(); it != quads.end(); it++) {
		double q_score = it->score(orig_edged*255);
		if (q_score > max_score) {
			max_score = q_score;
			best_quad = *it;
		}
	}

	// draw the best quad


	//cv::line(image, best_quad.pt1, best_quad.pt2, Scalar(0, 255, 255));
	//cv::line(image, best_quad.pt2, best_quad.pt3, Scalar(0, 255, 255));
	//cv::line(image, best_quad.pt3, best_quad.pt4, Scalar(0, 255, 255));
	//cv::line(image, best_quad.pt4, best_quad.pt1, Scalar(0, 255, 255));

	// scale it back
	a.x = best_quad.pt1.x * float(img.cols) / float(image_w);
	a.y = best_quad.pt1.y * float(img.rows) / float(image_h);
	b.x = best_quad.pt2.x * float(img.cols) / float(image_w);
	b.y = best_quad.pt2.y * float(img.rows) / float(image_h);
	c.x = best_quad.pt3.x * float(img.cols) / float(image_w);
	c.y = best_quad.pt3.y * float(img.rows) / float(image_h);
	d.x = best_quad.pt4.x * float(img.cols) / float(image_w);
	d.y = best_quad.pt4.y * float(img.rows) / float(image_h);

	sort_points(a, b, c, d);

	cout << a << endl;

	return;

}

// rotate the points in clockwise direction
void rotate(Point& a, Point& b, Point& c, Point& d){
	Point t = d;
	d = c;
	c = b;
	b = a;
	a = t;
}

// sort the points so that the first point is the upper left one, in clockwise direction
void sort_points(Point& a, Point& b, Point& c, Point& d){
	// all points are in clockwise direction after the isConvex() in Class quadrilateral
	for (int i = 0; i < 4; i++){
		int x_count = 0;
		if (b.x < a.x) ++x_count;
		if (c.x < a.x) ++x_count;
		if (d.x < a.x) ++x_count;
		if(x_count > 1){
			rotate(a, b, c, d);
			continue;
		}
	
		int y_count = 0;
		if (b.y < a.y) ++y_count;
		if (c.y < a.y) ++y_count;
		if (d.y < a.y) ++y_count;
		if(y_count > 1){
			rotate(a, b, c, d);
			continue;
		}
		break;
	}

}