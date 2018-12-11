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
	Aruco() {
		ReadInCamera("assets/pixel/test2.txt");
		tagType_ = cv::aruco::DICT_4X4_250;
		dict_ = cv::aruco::getPredefinedDictionary(tagType_);
		detector_ = cv::aruco::DetectorParameters::create();
	}
	Aruco(std::string pPath) : Aruco()
	{
		path_ = pPath;
		ReadInCamera(pPath + ".txt");
		vid_ = cv::VideoCapture(pPath + ".mp4");
	}
	Aruco(int pCapturePort) : Aruco() {
		vid_ = cv::VideoCapture(pCapturePort);
		assert(vid_.isOpened());
	}
	void ReadInCamera(std::string pPath)
	{
		auto file = File::ReadFile(pPath.c_str());
		assert(file[0].find("cam:") == 0);
		file[0] = file[0].substr(4, file[0].size());
		assert(file[1].find("dist:") == 0);
		file[1] = file[1].substr(5, file[1].size());
		assert(file[2].find("len:") == 0);
		file[2] = file[2].substr(4, file[2].size());
		auto intrinParams = Split((char*)file[0].c_str(), ',');
		assert(intrinParams.size() == 9);
		auto distCoef = Split((char*)file[1].c_str(), ',');
		assert(distCoef.size() == 5);
		auto len = Split((char*)file[2].c_str(), ',');
		assert(len.size() == 1);

		float K_[3][3] = { {stof(intrinParams[0]), stof(intrinParams[1]), stof(intrinParams[2])},
		{stof(intrinParams[3]), stof(intrinParams[4]), stof(intrinParams[5])},
		{stof(intrinParams[6]), stof(intrinParams[7]), stof(intrinParams[8])} };
		k_ = cv::Mat(3, 3, CV_32F, K_).clone();

		float dist[5] = { stof(distCoef[0]),stof(distCoef[1]),stof(distCoef[2]),stof(distCoef[3]),stof(distCoef[4]) };

		distCoeffs_ = cv::Mat(5, 1, CV_32F, dist).clone();
		markerLength_ = stof(len[0]);
	}

	ArucoFrame* ProcessFrame()
	{
		cv::Mat lastImage_;
		if (vid_.grab())
			vid_.read(lastImage_);
		else
			return  tags_.back();
		if (lastImage_.empty())
			lastImage_ = this->lastImage_;
		std::vector<std::vector<cv::Point2f> > corners, rejects;
		std::vector<int> markerIds;
		
		cv::aruco::detectMarkers(lastImage_, dict_, corners,
			markerIds, detector_, rejects);
		if (markerIds.empty())
			return nullptr;

		std::vector< cv::Vec3d> rVecs, tVecs;
		cv::aruco::estimatePoseSingleMarkers(corners, markerLength_, k_,
			distCoeffs_, rVecs, tVecs);
		cv::aruco::drawDetectedMarkers(lastImage_, corners, markerIds);

		ArucoFrame* tags = new ArucoFrame(lastImage_);
		for(int i = 0; i < markerIds.size(); ++i)
		{
			auto rvec = rVecs[i];
			auto tvec = tVecs[i];
			auto tag = new ArucoTag(corners[i], markerIds[i], rvec, tvec);
			tags->Add(markerIds[i], tag);
			//cv::aruco::drawAxis(lastImage_, k_, distCoeffs_, rvec, tvec, markerLength_*.4f);
		}
		tags_.push_back(tags);
		this->lastImage_ = lastImage_;
		return tags;
	}

	float GetFOV()
	{
		double fovX, fovY, a, b, c;
		cv::Point2d pnt;
		cv::calibrationMatrixValues(k_, cv::Size(1920, 1080), 32, 32, fovX, fovY, a, pnt, c);
		return 0;
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
	std::string path_;
	cv::VideoCapture vid_;
	cv::Mat lastImage_;
};

#endif