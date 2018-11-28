#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera
{
public:
	Camera()
	{
		loc_ = { 4,3,3 };
		up_ = { 0,1,0 };
		dir_ = { 0, 0, 0 };
		theta_ = -1;
		phi_ = 1;
	}

	float lastX = 0, lastY = 0;
	void Update(GLFWwindow* pWindow)
	{
		glm::vec3 update(0);
		if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
			update += glm::vec3(1);
		else if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
			update += glm::vec3(-1);

		if (glfwGetKey(pWindow, GLFW_KEY_R) == GLFW_PRESS)
			update += glm::vec3(0, -1, 0);
		else if (glfwGetKey(pWindow, GLFW_KEY_F) == GLFW_PRESS)
			update += glm::vec3(0, 1, 0);

		if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(pWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
			update *= 10;
		loc_ += 0.1f * this->CalcuateLookDirection() * update;

		auto leftMouse = glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT);
		if(leftMouse == GLFW_PRESS)
		{
			double x, y;
			glfwGetCursorPos(pWindow, &x, &y);
			if(lastX > 0 && lastY > 0)
			{
				theta_ += 0.005f * (x - lastX);
				phi_ += 0.005f * (lastY - y);
				if (phi_ <= 0)
					phi_ = 0.001f;
				if (phi_ >= 3.14)
					phi_ = (3.14 - 0.001f);
				if (theta_ <= -6.28)
					theta_ = 0;
				if (theta_ >= 6.28)
					theta_ = 0;
			}

			lastX = x;
			lastY = y;
		}
		else if (leftMouse == GLFW_RELEASE && lastX != 0 || lastY != 0)
		{
			lastX = 0;
			lastY = 0;
		}

		dir_ = loc_+ this->CalcuateLookDirection();
	}

	glm::mat4 View()
	{
		return glm::lookAt(loc_, dir_, up_);
	}

	glm::vec3 CalcuateLookDirection()
	{
		return glm::vec3(
			sin(theta_) * sin(phi_),
			-cos(phi_),
			-cos(theta_) * sin(phi_));
	}
private:
	glm::vec3 up_, dir_, loc_;
	float theta_, phi_;
};

#endif