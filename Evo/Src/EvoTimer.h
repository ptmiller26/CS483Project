#ifndef _EVOTIMER_H_
#define _EVOTIMER_H_

class EvoTimer
{
   public:
	EvoTimer();
	~EvoTimer();

	// Start the timer
	void Start();

	// Get seconds passed since last call to start or this function
	float GetSecondsPassed();

   protected:
	LARGE_INTEGER	m_llCounter;
	LONGLONG		m_llFrequency;
};



// Implemenation

inline void EvoTimer::Start()
{
	QueryPerformanceCounter(&m_llCounter);
}

inline float EvoTimer::GetSecondsPassed()
{
	LARGE_INTEGER    liNow;
    QueryPerformanceCounter(&liNow);
    double dDuration = (double)(liNow.QuadPart - m_llCounter.QuadPart)/m_llFrequency;

	m_llCounter = liNow;

	return (float)(dDuration);
}

#endif