#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
public:
	Timer() { Start(); }
	void Start()
	{
		start_ = glfwGetTime();
		elapsed_ = 0;
	}
	double Stop()
	{
		fin_ = glfwGetTime();
		elapsed_ =  fin_ - start_;
		return elapsed_;
	}
	double Restart()
	{
		fin_ = glfwGetTime();
		auto elapsed = fin_ - start_;
		Start();
		return elapsed;
	}

	double Elapsed()
	{
		return glfwGetTime() - start_;
	}
private:
	double start_, fin_;
	double elapsed_;
};

#endif