#ifndef _HELPER_H_
#define _HELPER_H_
#include "Buffer/VertexBuffer.h"
#include "CVLogger.h"
#include "Timer.h"
#include "File.h"

void glErrorCheck(int pLine, const char* pFile)
{
	auto err = glGetError();
	if (err != GL_NO_ERROR)
	{
		auto errStr = gluErrorString(err);
		char out[1024];
		sprintf(out, "ERR: %s, %i %s\n", (char*)errStr, pLine, pFile);
		CVLogger::Log(out);
	}
}
std::vector<glm::vec3> ReadInOBJ(const char* pFile)
	{
		FILE* fp = fopen(pFile, "r");

		assert(fp != nullptr);
		{
			std::vector<glm::vec3 > vertices;
			std::vector<unsigned int> vIndicies;
			while(true)
			{
				char lineHdr[255];
				int res = fscanf(fp, "%s", lineHdr);
				if (res == EOF)
					break;

				if(strcmp(lineHdr, "v") == 0)
				{
					float x, y, z;
					fscanf(fp, "%f %f %f\n", &x, &y, &z);
					vertices.push_back(glm::vec3(x, y, z));
				}

				else if (strcmp(lineHdr, "f") == 0)
				{
					unsigned int index[3] = {0};

					if(fgets(lineHdr, sizeof(lineHdr), fp) != nullptr)
					{
						int matches = sscanf(lineHdr, " %d %d %d\n", index, index+1, index+2);
						assert(matches == 3);
					}

					for(int i = 0; i < 3; ++i)
					{
						if(index[i])
						{
							vIndicies.push_back(index[i]);
						}
					}
				}
			}
			std::vector<glm::vec3> v;
			for (unsigned int vindex : vIndicies)
			{
				if(vindex)
					v.push_back(vertices.at(vindex-1));
			}
			return v;
		}
	}

glm::mat4 BuildProjection(float pFOV, float pRatio, float n, float f)
{
	float ang = 1.0f / tan(pFOV * (3.14159 / 360.f));
	auto mat = glm::identity<glm::mat4>();
	mat[0][0] = ang / pRatio;
	mat[1][1] = ang;
	mat[2][2] = (f + n) / (n - f);
	mat[3][2] = (2.f * f * n) / (n - f);
	mat[2][3] = -1.f;
	mat[3][3] = 0.f;
	return mat;
}

glm::mat4 BuildProjectionLegacy(float n, float f, float a, float b, float cx, float cy)
{
	return glm::make_mat4x4(new float[16]{ a / cx, 0, 0, 0,
		0, b / cy, 0, 0,
		0, 0, -(f + n) / (f - n), -(2 * f*n) / (f - n),
		0, 0, -1, 0 });
}

template <typename T>
void PrintMat(cv::Mat* pMat, int pX = 4, int pY = 4)
{
	for (int i = 0; i < pX; ++i)
	{
		printf("| ");
		for (int j = 0; j < pY; ++j)
		{
			printf("%.2f\t\t", pMat->at<T>(i, j));
		}
		printf("|\n");
	}
	printf("\n");
}

void PrintMat(glm::mat4 pMat)
{
	for (int i = 0; i < 4; ++i)
	{
		printf("| ");
		for (int j = 0; j < 4; ++j)
		{
			printf("%.2f\t\t", pMat[i][j]);
		}
		printf("|\n");
	}
	printf("\n");
}

cv::Mat VecToMat(cv::Vec3b pIn)
{
	cv::Mat mat(3, 1, CV_32F);
	mat.at<double>(0, 0) = pIn[0];
	mat.at<double>(1, 0) = pIn[1];
	mat.at<double>(2, 0) = pIn[2];
	return mat;
}

void fromCV2GLM(const cv::Mat& cvmat, glm::mat4* glmmat)
{
	if (cvmat.cols != 4 || cvmat.rows != 4 || cvmat.type() != CV_32F)
	{
		printf("Conversion err\n");
		return;
	}
	memcpy(glm::value_ptr(*glmmat), cvmat.data, 16 * sizeof(float));
}

void fromGLM2CV(const glm::mat4& glmmat, cv::Mat* cvmat)
{
	if (cvmat->cols != 4 || cvmat->rows != 4)
	{
		(*cvmat) = cv::Mat(4, 4, CV_32F);
	}
	memcpy(cvmat->data, glm::value_ptr(glmmat), 16 * sizeof(float));
}

glm::vec3 CVtoGLM(cv::Vec3d pVector)
{
	return glm::vec3(pVector[0], pVector[1], pVector[2]);
}

glm::mat4 RotateByVec(glm::mat4 pMat, glm::vec3 pRot)
{
	glm::mat4 trans = pMat;
	for (int i = 1; i <= 3; ++i)
	{
		int x = i == 1 ? 1 : 0, y = i == 2 ? 1 : 0, z = i == 3 ? 1 : 0;
		trans = glm::rotate(trans, (float)pRot[i - 1], glm::vec3(x, y, z));
	}
	return trans;
}

void ShowImageInWindow(cv::Mat pImage)
{
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create a window for display.
	cv::imshow("Display window", pImage); // Show our image inside it.
}

float* ODEtoOGL(const dReal* p, const dReal* R)
{
	float* M = new float[16];
	M[0] = R[0];	M[1] = R[4];	M[2] = R[8];	M[3] = 0;
	M[4] = R[1];	M[5] = R[5];	M[6] = R[9];	M[7] = 0;
	M[8] = R[2];	M[9] = R[6];	M[10] = R[10];	M[11] = 0;
	M[12] = p[0];	M[13] = p[1];	M[14] = p[2];	M[15] = 1;
	return M;
}

void diagnostics()
{
	char* out = new char[100];
	sprintf(out, "Renderer: %s\n", glGetString(GL_RENDERER));
	CVLogger::Log(out);
	sprintf(out, "OpenGL version: %s\n", glGetString(GL_VERSION));
	CVLogger::Log(out);
	sprintf(out, "Vendor: %s\n", glGetString(GL_VENDOR));
	CVLogger::Log(out);
	sprintf(out, "GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	CVLogger::Log(out);
}

std:: vector < double > calcVec(cv:: Vec3d rvec) {
	cv::Mat R;
	cv::Rodrigues( rvec , R);
	double ax, ay, az;
	ay = atan2(-R.at<double>(2, 0),
			pow(pow(R.at< double >(0, 0), 2) + pow(R.at< double >(1, 0), 2), 0.5));
	double cy = cos(ay);
	if (abs(cy) < 1e-9) {
		az = 0.0;
		ax = atan2(R.at< double >(0, 1), R.at< double >(1, 1));
		if (ay < 0)
			ax = -ax;
	}
	else
	{
		az = atan2(R.at< double >(1, 0) / cy, R.at< double >(0, 0) / cy);
		ax = atan2(R.at< double >(2, 1) / cy, R.at< double >(2, 2) / cy);
	}
	std:: vector < double > result;
	result.push_back(ax);
	result.push_back(ay);
	result.push_back(az);
	return
		result;
}
std::vector<std::string> Split(char* pInput, char pDelim)
	{
		auto result = std::vector<std::string>();
		std::stringstream ss(pInput);
		std::string item;
		while (getline(ss, item, pDelim))
			if (!item.empty())
				result.push_back(item);
		return result;
	};

void Dump(std::vector< std::map<std::string, double> > pStats)
{

	std::string output;
	bool first = true;
	for(auto& datapoint: pStats)
	{
			for(auto& point: datapoint)
			{
				if(first)
				{
					output += point.first + ",";
				}
				else
				{
					output += std::to_string(point.second) + ",";
				}
			}
			first = false;
			output += "\n";
	}
	File::WriteToFile("assets/dump.csv", output);
}

void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "glfw error (%i): %s\n", error, description);
	CVLogger::Log(description);
}

void drawStatistics(GLFWwindow* window, int frameCount)
{
	static Timer timer;
	static int framesSinceLastUpdate = 0;
	double elapsed = timer.Elapsed();
	if (elapsed > (double)1 / 15)
	{
		char outputText[256];
		sprintf(outputText, "FPS: %.0f, FT: %.4fms, F: %i",
		        ((float)framesSinceLastUpdate / elapsed),
		        elapsed / framesSinceLastUpdate, frameCount);
		glfwSetWindowTitle(window, outputText);
		framesSinceLastUpdate = 0;
		timer.Restart();
	}
	framesSinceLastUpdate++;
}


#endif
