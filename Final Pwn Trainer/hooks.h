#include <windows.h>

bool Hook(void* toHook, void* ourFunct, int len);

void funcHook(const char* hookName);