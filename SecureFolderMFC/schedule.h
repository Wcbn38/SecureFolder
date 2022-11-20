#pragma once

#include <combaseapi.h>

typedef bool* TSKHNDL;

TSKHNDL startTask(unsigned int timer, void (*call)(void* prm), void* prms);
void stopTask(TSKHNDL hndl);