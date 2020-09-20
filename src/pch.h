// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

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

// Boost log

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions/keyword.hpp>

 
//json
#include <cpprest/json.h>

extern CAppModule _Module; 


// HRESULT

#define E_AUTH_DONE								_HRESULT_TYPEDEF_(0x90000000L)
// const

static constexpr UINT MSG_NAVIGATE = WM_APP + 123;
static constexpr UINT MSG_RUN_ASYNC_CALLBACK = WM_APP + 124;


namespace		logging = boost::log;
namespace		src = boost::log::sources;
namespace		keywords = boost::log::keywords;
namespace		attrs = boost::log::attributes;
namespace		sinks = boost::log::sinks;
namespace		expr = boost::log::expressions;
namespace		fs = std::filesystem;

#include "framework.h"




#endif //PCH_H
