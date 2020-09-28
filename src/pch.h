// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


// error C2039: 'wait_for': is not a member of 'winrt::impl'
#include "winrt/base.h"
namespace winrt::impl
{
	template <typename Async>
	auto wait_for(Async const& async, Windows::Foundation::TimeSpan const& timeout);
}


// https://mariusbancila.ro/blog/2020/01/29/using-microsoft-edge-in-a-native-windows-desktop-app-part-2/
// Windows Header Files
#include <windows.h>
#include <objbase.h>
#include <fstream>
#include <iostream>
#include <shellscalingapi.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <filesystem>
#include <map>
#include <functional>


#include <wrl.h>
//#include <wil/com.h>


// ATL Support
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlwin.h>
#include <atltrace.h>

// WTL
#include <atlapp.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlcrack.h>
#include <atlmisc.h>



// Com wrapper
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <wrl.h>

// WebView
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>


 
//json
#include <cpprest/json.h>



extern CAppModule _Module; 


// HRESULT

#define E_AUTH_DONE								_HRESULT_TYPEDEF_(0x90000000L)
// const

static constexpr UINT MSG_NAVIGATE = WM_APP + 123;
static constexpr UINT MSG_RUN_ASYNC_CALLBACK = WM_APP + 124;


namespace		fs = std::filesystem;

#include "framework.h"




#endif //PCH_H
