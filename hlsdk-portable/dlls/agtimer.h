//++ BulliT

#if !defined(_AG_TIMER_H_)
#define _AG_TIMER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AgTimer
{
	cvar_t* m_pmp_timelimit;
	float m_fNextTimerUpdate;
	float m_fLastTimeCheck;
	float m_fEffectiveTime;

public:
	AgTimer();
	virtual ~AgTimer();

	void Print(char *sText, int fHoldTime, float x, float y, int iChannel);
	void Think();
};



#endif // !defined(_AG_TIMER_H_)

//-- Martin Webrant
