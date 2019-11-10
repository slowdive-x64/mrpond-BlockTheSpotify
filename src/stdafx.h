// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <Windows.h>



// TODO: reference additional headers your program requires here
#include <winsock2.h>
#include <winhttp.h>
#ifdef _DEBUG
#include <fstream>
#endif
#include "HookApi.h"
#include "hosts.h"