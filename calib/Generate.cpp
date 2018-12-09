

#include "opencv2/aruco/charuco.hpp"
#include "cv.hpp"

int main()
{
	auto dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_100);
	auto im = cv::aruco::CharucoBoard::create(6, 4, 0.04, 0.02, dict);
	cv::Mat img;
	cv::Size d(800, 600);
	im->draw(d, img);
	cv::imshow("Out", img);
	cv::imwrite("C:\\new\\CV-507\\final\\assets\\out.png", img);
	cvWaitKey(100000);
	return 0;
}
