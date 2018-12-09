#ifndef _ARUCO_FRAME_H_
#define _ARUCO_FRAME_H_
#include "opencv2/core/mat.hpp"
#include "Aruco.h"
#include <map>
#include "ArucoTag.h"

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
		if(!glIsTexture(tex_))
		{
			glEnable(GL_TEXTURE_2D);
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
		}
		return tex_;
	}
	float HWRatio()
	{
		return (float)image_.cols / image_.rows;
	}
	cv::Mat Image() { return image_; }
private:
	cv::Mat image_;
	std::map<int, ArucoTag*> tags_;
	GLuint tex_;
};


#endif