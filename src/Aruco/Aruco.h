#ifndef _ARUCO_H_
#define _ARUCO_H_
#include "opencv2/aruco.hpp"
#include "../Helper.h"
#include "ArucoFrame.h"
#include "ArucoTag.h"
#include "../File.h"

class Aruco
{
public:
	Aruco()
	{
		tagType_ = cv::aruco::DICT_4X4_1000;
		//tagType_ = cv::aruco::DICT_4X4_250;
		markerLength_ = 1.75f;
		//markerLength_ = 0.054;
		dict_ = cv::aruco::getPredefinedDictionary(tagType_);
		detector_ = cv::aruco::DetectorParameters::create();

		float K_[3][3] =
		{ { 675, 0, 320 },
		{ 0, 675, 240 },
		{ 0, 0, 1 } };
		k_ = cv::Mat(3, 3, CV_32F, K_).clone();
		float dist_[] = { 0,0,0,0,0 };
		distCoeffs_ = cv::Mat(5, 1, CV_32F, dist_).clone();
		//ReadInCamera("assets/pixel/calib.txt");
	}

	void ReadInCamera(std::string pPath)
	{
		auto file = File::ReadFile(pPath.c_str());
		assert(file[0].find("cam:") == 0);
		file[0] = file[0].substr(4, file[0].size());
		assert(file[1].find("dist:") == 0);
		file[1] = file[1].substr(5, file[1].size());
		auto intrinParams = Split((char*)file[0].c_str(), ',');
		assert(intrinParams.size() == 9);
		auto distCoef = Split((char*)file[1].c_str(), ',');
		assert(distCoef.size() == 5);
		double K_[3][3] = { {stod(intrinParams[0]), stod(intrinParams[1]), stod(intrinParams[2])},
		{stod(intrinParams[3]), stod(intrinParams[4]), stod(intrinParams[5])},
		{stod(intrinParams[6]), stod(intrinParams[7]), stod(intrinParams[8])} };
		k_ = cv::Mat(3, 3, CV_32F, K_).clone();

		double dist[5] = { stod(distCoef[0]),stod(distCoef[1]),stod(distCoef[2]),stod(distCoef[3]),stod(distCoef[4]) };

		distCoeffs_ = cv::Mat(5, 1, CV_32F, dist).clone();
	}

	ArucoFrame* ProcessFrame(cv::Mat &pImage, int pFrameNumber)
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
		PrintMat<float>(&distCoeffs_, 5, 1);
		PrintMat<float>(&k_, 3, 3);

		ArucoFrame* tags = new ArucoFrame(pImage);
		for(int i = 0; i < markerIds.size(); ++i)
		{
			auto rvec = rVecs[i];
			auto tvec = tVecs[i];
			auto tag = new ArucoTag(corners[i], markerIds[i], rvec, tvec);
			tags->Add(markerIds[i], tag);
			//cv::aruco::drawAxis(pImage, k_, distCoeffs_, rvec, tvec, markerLength_*.4f);
		}
		tags_.push_back(tags);
		return tags;
	}

	cv::Vec3d Project(int pIndex, float* pPoint)
	{
		cv::Vec3d points;
		auto frame = tags_[pIndex];
		auto tag = (*frame)[1];
		cv::Vec3d pnt(pPoint[0], pPoint[1], pPoint[2]);

		cv::projectPoints(pnt, tag->Rotation(), tag->Translation(), k_, distCoeffs_, points);
		return points;
	}

	cv::Mat CameraIntrin()
	{
		return k_;
	}

	float* Projection(float pNear, float pFar,  float pOriginX, float pOriginY, int pWidth, int pHeight)
	{
		float x1 = (float)((float)pWidth - 2.f*(float)k_.at<double>(0, 2) + 2.f*pOriginX) / (float)pWidth;
		float* vals = new float[16]{
			2.f*(float)k_.at<double>(0,0)/(float)pWidth, x1, 0,
		0,-2.f*(float)k_.at<double>(1,1)/pHeight, (float)(pHeight - 2 * k_.at<double>(1,2) + 2 * pOriginY), 0,
		0,0,(-pFar-pNear)/(pFar-pNear),-2*pFar*pNear/(pFar-pNear),
		0,0,-1,0};
		return vals;
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