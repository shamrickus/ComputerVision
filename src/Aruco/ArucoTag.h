#ifndef _ARUCO_TAG_H_
#define _ARUCO_TAG_H_
#include <vector>
#include "opencv2/core/mat.hpp"
#include "glm/mat4x2.hpp"
#include "opencv2/ximgproc/disparity_filter.hpp"
#include "../Helper.h"
#include "glm/ext/matrix_transform.hpp"


class ArucoTag
{
public:
	ArucoTag() {}
	ArucoTag(std::vector<cv::Point2f> pCorner, int pId, cv::Vec3d pRVec, cv::Vec3d pTVec)
	{
		corner_ = pCorner;
		id_ = pId;
		rotationVector_ = pRVec;
		translationVector_ = pTVec;
	}

	std::vector<double> Rotation()
	{
		return calcVec(rotationVector_);
	}
	cv::Vec3d RotationRaw()
	{
		return rotationVector_;
	}

	glm::mat4 GLTransform(float pScale)
	{
		cv::Mat rotation, jacobian;
		cv::Mat view = cv::Mat::zeros(4,4,CV_32F);
		cv::Mat transform = cv::Mat::zeros(4,4,CV_32F);
		cv::Rodrigues(rotationVector_, rotation, jacobian);
		for(int row = 0; row<3; ++row)
		{
			for(int col = 0; col<3; ++col)
			{
				view.at<float>(row, col) = (float)rotation.at<double>(row, col);
			}
			view.at<float>(row, 3) = (float)translationVector_[row] * 0.1f;
		}
		view.at<float>(3, 3) = 1.0f;


		//z translation
		transform.at<float>(0, 0) = 1.0f;
		transform.at<float>(1, 1) = -1.0f;
		transform.at<float>(2, 2) = -1.0f;
		transform.at<float>(3, 3) = 1.0f;
		view = transform * view;
		cv::transpose(view, view);
		//PrintMat<float>(&view);
		auto r = rotationVector_;
		auto t = translationVector_;
		//std::cout << "rVec " << r[0] << ", " << r[1] << ", " << r[2] << std::endl;
		//std::cout << "tVec " << t[0] << ", " << t[1] << ", " << t[2] << std::endl;
		glm::mat4 ret = glm::identity<glm::mat4>();
		fromCV2GLM(view, &ret);
		transform_= ret;
		return transform_;
		/*
		auto trans = glm::translate(glm::identity<glm::mat4>(), CVtoGLM(translationVector_));
		return RotateByVec(trans, CVtoGLM(rotationVector_));
	*/
	}

	std::vector<cv::Point2f> Corner() { return corner_;  }

	cv::Vec3d Translation() { return translationVector_; }

private:
	glm::mat4 transform_;
	std::vector<cv::Point2f> corner_;
	cv::Vec3d rotationVector_, translationVector_;
	int id_;
};

#endif