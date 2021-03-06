#pragma once

// Types

typedef unsigned char           byte;
typedef short                   int16;
typedef unsigned short          uint16;
typedef int                     int32;
typedef unsigned int            uint32;
typedef long long               int64;
typedef unsigned long long      uint64;

#ifndef NULL
#define NULL 0
#endif

// ExecutionEngine Callbacks

#if _WINDOWS
#define __stdcall __stdcall
#else
#define __stdcall 
#endif

typedef byte(__stdcall* InvokeInteropCallback)(void* method, byte length);
typedef byte(__stdcall* LoadScriptCallback)(const byte* scriptHash, byte isDynamicInvoke, int32 rvcount);
typedef int32(__stdcall* GetMessageCallback)(uint32 iteration, byte* &message);

typedef void(__stdcall* OnStepIntoCallback)(void* item);