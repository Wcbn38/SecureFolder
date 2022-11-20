#include "pch.h"
#include "schedule.h"

struct threadData
{
	unsigned int timer;
	void (*call)(void* prm);
	void* prms;
	bool kill = false;
};

DWORD WINAPI thread(LPVOID  routine)
{
	threadData* data = (threadData*) routine;

	while (!data->kill)
	{
		data->call(data->prms);
		Sleep(data->timer);
	}

	return 0;
}

TSKHNDL startTask(unsigned int timer, void (*call)(void* prm), void* prms)
{
	threadData *myThreadData = new threadData{ timer, call, prms, false };
	CreateThread(NULL, 0, thread, myThreadData, 0, NULL);
	return &myThreadData->kill;
}

void stopTask(TSKHNDL hndl)
{
	*hndl = true;
}