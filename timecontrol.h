#ifndef _TIME_CTRL_H_
#define _TIME_CTRL_H_

class TCtrl
{
	public:
		TCtrl()
		{
			fAllocatedTime=0.0f;
		}
		TCtrl(float fAllocT)
		{
			fAllocatedTime=fAllocT;
		}

		float fAllocatedTime,fElapsedTime;
		clock_t clkStartTime,clkCurrentTime;

		const float GetAllocatedTime() const
		{
			return fAllocatedTime;
		}

		const void SetAllocatedTime(float fAllocT)
		{
			fAllocatedTime=fAllocT;
		}

		const void SetStartTime()
		{
			clkStartTime=clock();
			fElapsedTime=0.0f;
		}

		const bool UpdateTime()
		{
			clkCurrentTime=clock();
			fElapsedTime = ((float)(clkCurrentTime-clkStartTime))/CLOCKS_PER_SEC;

			return (fElapsedTime>fAllocatedTime);
		}

		const bool GetStatus() const
		{
			return (fElapsedTime>fAllocatedTime);
		}

		const float GetElapsedTime() const
		{
			return fElapsedTime;
		}
};

extern TCtrl g_TimeControl;

#endif
