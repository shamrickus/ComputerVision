#ifndef _ARUCO_H_
#define _ARUCO_H_
#include "opencv2/aruco.hpp"
#include "../Helper.h"

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

	glm::mat4 GLTransform()
	{
		cv::Mat rotation;
		cv::Mat view = cv::Mat::zeros(4,4,CV_32F);
		cv::Mat transform = cv::Mat::zeros(4,4,CV_32F);
		cv::Rodrigues(rotationVector_, rotation);
		auto res = calcVec(rotationVector_);
		for(int row = 0; row<3; ++row)
		{
			for(int col = 0; col<3; ++col)
			{
				view.at<float>(row, col) = (float)rotation.at<double>(row, col);
			}
			view.at<float>(row, 3) = (float)translationVector_[row];
		}
		//z translation
		view.at<float>(3, 3) = 1.0f;
		transform.at<float>(0, 0) = 1.0f;
		transform.at<float>(1, 1) = -1.0f;
		transform.at<float>(2, 2) = -1.0f;
		transform.at<float>(3, 3) = 1.0f;
		/*
		for(int i = 0; i < 4; ++i)
		{
			transform.at<float>(1, i) = -1.0f;
			transform.at<float>(2, i) = -1.0f;
		}
		view = view * transform;
		*/
		view = transform * view;
		cv::transpose(view, view);
		glm::mat4 ret = glm::identity<glm::mat4>();
		fromCV2GLM(view, &ret);
		transform_= ret;
		return transform_;
		/*
		auto trans = glm::translate(glm::identity<glm::mat4>(), CVtoGLM(translationVector_));
		return RotateByVec(trans, CVtoGLM(rotationVector_));
	*/
	}

private:
	glm::mat4 transform_;
	std::vector<cv::Point2f> corner_;
	cv::Vec3d rotationVector_, translationVector_;
	int id_;
};

class ArucoFrame
{
public:
	ArucoFrame() { }
	ArucoFrame(cv::Mat pMat) : image_(pMat) { }

	void Add(int pIndex, ArucoTag* pTag)
	{
		tags_[pIndex] = pTag;
	}

	ArucoTag* operator[](int pIndex)
	{
		return tags_[pIndex];
	}

	GLuint ToTexture()
	{
		cv::flip(image_, image_, 0);
		glEnable(GL_TEXTURE_2D);
		GLuint tex_;
		glGenTextures(1, &tex_);
		glBindTexture(GL_TEXTURE_2D, tex_);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glPixelStorei(GL_UNPACK_ALIGNMENT, (image_.step & 3) ? 1 : 4);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, image_.step/image_.elemSize());
		glTexImage2D(GL_TEXTURE_2D,0,3,image_.cols, image_.rows,
			0,GL_BGR,GL_UNSIGNED_BYTE, image_.data);
		return tex_;
	}
	float HWRatio()
	{
		return (float)image_.cols / image_.rows;
	}
private:
	cv::Mat image_;
	std::map<int, ArucoTag*> tags_;
};

class Aruco
{
public:
	Aruco()
	{
		tagType_ = cv::aruco::DICT_4X4_1000;
		//tagType_ = cv::aruco::DICT_4X4_250;
		markerLength_ = 0.05f;
		//markerLength_ = 0.785;
		dict_ = cv::aruco::getPredefinedDictionary(tagType_);
		detector_ = cv::aruco::DetectorParameters::create();

		/*double K_[3][3] =
		{ { 675, 0, 320 },
		{ 0, 675, 240 },
		{ 0, 0, 1 } };*/
		double K_[3][3] =
		{ { 32, 0, 1920 },
		{ 0, 32, 1080 },
		{ 0, 0, 1 } };
		k_ = cv::Mat(3, 3, CV_64F, K_).clone();
		double dist_[] = { 0, 0, 0, 0, 0 };
		//double dist_[] = { -0.003364, -0.00001321, -0.003368, 0.0005888, 0.00000001276 };
		distCoeffs_ = cv::Mat(5, 1, CV_64F, dist_).clone();
	}

	ArucoFrame* ProcessFrame(cv::Mat pImage, int pFrameNumber)
	{
		std::vector<std::vector<cv::Point2f> > corners, rejects;
		std::vector<int> markerIds;
		cv::aruco::detectMarkers(pImage, dict_, corners,
			markerIds, detector_, rejects);
		if(markerIds.empty())
		{
			CVLogger::Log("Unable to find tags on frame " + pFrameNumber);
			return new ArucoFrame();
		}
		std::vector< cv::Vec3d> rVecs, tVecs;
		cv::aruco::estimatePoseSingleMarkers(corners, markerLength_, k_,
			distCoeffs_, rVecs, tVecs);
		cv::aruco::drawDetectedMarkers(pImage, corners, markerIds);

		ArucoFrame* tags = new ArucoFrame(pImage);
		for(int i = 0; i < markerIds.size(); ++i)
		{
			auto rvec = rVecs[i];
			auto tvec = tVecs[i];
			auto tag = new ArucoTag(corners[i], markerIds[i], rvec, tvec);
			tags->Add(markerIds[i], tag);
			//cv::aruco::drawAxis(pImage, k_, distCoeffs_, rvec, tvec, 1);
		}
		tags_.push_back(tags);
		return tags;
	}

private:
	unsigned int tagType_;
	float markerLength_;
	std::vector<ArucoFrame* > tags_;
	cv::Ptr<cv::aruco::Dictionary> dict_;
	cv::Ptr<cv::aruco::DetectorParameters> detector_;
	cv::Mat k_, distCoeffs_;
};

#endif