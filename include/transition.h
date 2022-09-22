#ifndef TRANSITION_H
#define TRANSITION_H

#include <wedge2/main.h>
#include <wedge2/systems.h>

EXPORT_CLASS_ALIGN(Transition_Step, 16) : public wedge::Step
{
public:
	static const int LENGTH = 1500;

	Transition_Step(bool out, wedge::Task *task); // in or out
	virtual ~Transition_Step();
	
	bool run();
	void draw_back();
	void draw_fore();
	void start();
	
	// For 16 byte alignment to make glm::mat4 able to use SIMD
#ifdef _WIN32
	void *operator new(size_t i);
	void operator delete(void* p);
#endif

protected:
	bool out;
	Uint32 start_time;
	int count;
	glm::mat4 mv_save, proj_save;
	int length;
};

#endif // TRANSITION_H
