// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "discord_rpc.h"

#define LAUNCHER_CLASS L"EME.LauncherWnd"
#define LAUNCHER_WINDOW LAUNCHER_CLASS

wchar_t TlExePath[MAX_PATH];
char TicketStr[1024], LastSvrStr[1024], CharCntStr[1024];
LPCSTR SlsUrl, GameString;
bool Hello, Sls, GameStr, Ticket, LastSvr, CharCnt, ClientTerminated;
COPYDATASTRUCT cds;
HWND hWndProc;
UINT uMsgProc;
WPARAM wParamProc;
LPARAM lParamProc;
void (*MessageListener)(LPCSTR, int) = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COPYDATA:
		{
			COPYDATASTRUCT* pCds = (COPYDATASTRUCT*)lParam;
			LPCSTR message = (LPCSTR)pCds->lpData;
			cds = *pCds;
			hWndProc = hWnd;
			wParamProc = wParam;
			if (!lstrcmpA(message, "Hello!!"))
			{
				Hello = true;
				return 1;
			}
			if (!lstrcmpA(message, "slsurl"))
			{
				Sls = true;
				return 1;
			}
			if (!lstrcmpA(message, "gamestr"))
			{
				GameStr = true;
				return 1;
			}
			if (!strncmp(message, "endPopup", 8))
			{
				unsigned int EndPopup;
				sscanf_s(message, "endPopup(%d)", &EndPopup);
				if(MessageListener != nullptr)
					MessageListener("endPopup", EndPopup);
				PostQuitMessage(0);
				return 1;
			}
			if (!strncmp(message, "gameEvent", 9))
			{
				unsigned int GameEvent;
				sscanf_s(message, "gameEvent(%d)", &GameEvent);
				if (MessageListener != nullptr)
					MessageListener("gameEvent", GameEvent);
				return 1;
			}
			if (MessageListener != nullptr)
				MessageListener(message, 0);
			return 1;
		}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI MessageHandler(LPVOID nothing)
{
	while (!ClientTerminated)
	{
		if (Hello)
		{
			LPCSTR out = "Hello!!";
			COPYDATASTRUCT pCds;
			pCds.dwData = cds.dwData;
			pCds.cbData = lstrlenA(out) + 1;
			pCds.lpData = (LPVOID)out;
			DWORD_PTR lpdwResult;
			SendMessageTimeout((HWND)wParamProc, WM_COPYDATA, (WPARAM)hWndProc, (LPARAM)& pCds, SMTO_NORMAL, 0, &lpdwResult);
			Hello = false;
		}
		if (Sls)
		{
			COPYDATASTRUCT pCds;
			pCds.dwData = cds.dwData;
			pCds.cbData = lstrlenA(SlsUrl) + 1;
			pCds.lpData = (LPVOID)SlsUrl;
			DWORD_PTR lpdwResult;
			SendMessageTimeout((HWND)wParamProc, WM_COPYDATA, (WPARAM)hWndProc, (LPARAM)& pCds, SMTO_NORMAL, 0, &lpdwResult);
			Sls = false;
		}
		if (GameStr)
		{
			COPYDATASTRUCT pCds;
			pCds.dwData = cds.dwData;
			pCds.cbData = lstrlenA(GameString) + 1;
			pCds.lpData = (LPVOID)GameString;
			DWORD_PTR lpdwResult;
			SendMessageTimeout((HWND)wParamProc, WM_COPYDATA, (WPARAM)hWndProc, (LPARAM)& pCds, SMTO_NORMAL, 0, &lpdwResult);
			GameStr = false;
		}
		if (Ticket)
		{
			COPYDATASTRUCT pCds;
			pCds.dwData = cds.dwData;
			pCds.cbData = lstrlenA(TicketStr) + 1;
			pCds.lpData = (LPVOID)TicketStr;
			DWORD_PTR lpdwResult;
			SendMessageTimeout((HWND)wParamProc, WM_COPYDATA, (WPARAM)hWndProc, (LPARAM)& pCds, SMTO_NORMAL, 0, &lpdwResult);
			Ticket = false;
		}
		if (LastSvr)
		{
			COPYDATASTRUCT pCds;
			pCds.dwData = cds.dwData;
			pCds.cbData = lstrlenA(LastSvrStr) + 1;
			pCds.lpData = (LPVOID)LastSvrStr;
			DWORD_PTR lpdwResult;
			SendMessageTimeout((HWND)wParamProc, WM_COPYDATA, (WPARAM)hWndProc, (LPARAM)& pCds, SMTO_NORMAL, 0, &lpdwResult);
			LastSvr = false;
		}
		if (CharCnt)
		{
			COPYDATASTRUCT pCds;
			pCds.dwData = cds.dwData;
			pCds.cbData = lstrlenA(CharCntStr) + 1;
			pCds.lpData = (LPVOID)CharCntStr;
			DWORD_PTR lpdwResult;
			SendMessageTimeout((HWND)wParamProc, WM_COPYDATA, (WPARAM)hWndProc, (LPARAM)& pCds, SMTO_NORMAL, 0, &lpdwResult);
			CharCnt = false;
		}
		Sleep(1000);
	}
	return 1;
}

void InitDiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));

	Discord_Initialize("880294531667476500", &handlers, 1, "");
}

void UpdatePresence()
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.largeImageKey = "tera_default";
	discordPresence.startTimestamp = time(0);
	Discord_UpdatePresence(&discordPresence);
}

extern "C" __declspec(dllexport) void LaunchGame(LPCSTR lpSlsUrl, LPCSTR lpGameStr)
{
	SlsUrl = lpSlsUrl;
	GameString = lpGameStr;
	Hello = false;
	Sls = false;
	GameStr = false;
	ClientTerminated = false;

	// Get Running Application's Module Handle
	HMODULE hModule = GetModuleHandle(NULL);

	// Create and Register a Window Class
	WNDCLASSEX windowClassEx;
	windowClassEx.cbSize = sizeof(WNDCLASSEX);
	windowClassEx.lpszClassName = LAUNCHER_CLASS;
	windowClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
	windowClassEx.hbrBackground = NULL;
	windowClassEx.lpfnWndProc = WndProc;
	windowClassEx.hInstance = hModule;
	windowClassEx.cbClsExtra = 0;
	windowClassEx.cbWndExtra = 0;
	windowClassEx.lpszMenuName = NULL;
	RegisterClassEx(&windowClassEx);

	// Create The Window
	CreateWindowEx(WS_EX_APPWINDOW, windowClassEx.lpszClassName, LAUNCHER_WINDOW, WS_OVERLAPPEDWINDOW, INT_MIN, INT_MIN, INT_MIN, INT_MIN, NULL, NULL, hModule, NULL);

	// Create Process TL.exe suspended
	STARTUPINFO lpStartUpInfo = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION lpProcessInformation;
	if (!CreateProcess(TlExePath, NULL, NULL, NULL, true, CREATE_SUSPENDED, NULL, NULL, &lpStartUpInfo, &lpProcessInformation))
	{
		char message[48];
		sprintf_s(message, 48, "Unable to start TL.exe. Error Code: %d", GetLastError());
		MessageBoxA(NULL, message, "Teralauncher.dll Error", MB_ICONERROR | MB_OK);
		return;
	}

	// Resume process thread and create a new thread to handle messages from TL.exe
	ResumeThread(lpProcessInformation.hThread);
	CreateThread(0, 0, MessageHandler, 0, 0, 0);

	InitDiscord();
	UpdatePresence();

	// Listen to incoming messages, translate and dispatch them to WndProc for processing.
	// This procedure will block the Thread it's running on until PostQuitMessage is received.
	MSG msg;
	while (GetMessage(&msg, NULL, 0U, 0U) == TRUE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// If we get out of the loop that means we received PostQuitMessage, so assume client termination and exit.
	ClientTerminated = true;
	Discord_Shutdown();
	CloseHandle(lpProcessInformation.hProcess);
}

extern "C" __declspec(dllexport) void RegisterMessageListener(void (*f)(LPCSTR, int))
{
	MessageListener = f;
}

extern "C" __declspec(dllexport) void SendMessageToClient(LPCSTR responseTo, LPCSTR message)
{
	if (!lstrcmpA(responseTo, "ticket"))
	{
		strcpy_s(TicketStr, message);
		Ticket = true;
	}
	if (!lstrcmpA(responseTo, "last_svr"))
	{
		strcpy_s(LastSvrStr, message);
		LastSvr = true;
	}
	if (!lstrcmpA(responseTo, "char_cnt"))
	{
		strcpy_s(CharCntStr, message);
		CharCnt = true;
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	GetPrivateProfileString(L"MTL", L"TlExePath", L"Client\\TL.exe", TlExePath, MAX_PATH, L".\\mtl.ini");
    return TRUE;
}

