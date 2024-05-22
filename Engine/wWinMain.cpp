#include "Win32_/FatWin32_.hpp"

import DX11Framework;

#if _MSVC_LANG > 202002L
import std;
#else
#if _MSVC_LANG == 202002L
import std.core;
#else
#error C++20 or greater version required
#endif // _MSVC_LANG == 202002L
#endif // _MSVC_LANG > 202002L

int APIENTRY wWinMain(
	/* [[maybe_unused]] */ _In_     HINSTANCE hInst,
	   [[maybe_unused]]    _In_opt_ HINSTANCE hPrevInst,
	   [[maybe_unused]]    _In_     LPWSTR lpCmdLine,
	   [[maybe_unused]]    _In_     int nShowCmd)
{
	try
	{
		fatpound::win32::dx11::Framework{ hInst }.Run();

		return 0;
	}
	catch (const std::exception& ex)
	{
		const std::string str = ex.what();
		const std::wstring wstr{ str.cbegin(), str.cend() };

		MessageBox(nullptr, wstr.c_str(), L"Error", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, L"Non-STD Exception was thrown...", L"Error", MB_OK | MB_ICONERROR);
	}

	return -1;
}