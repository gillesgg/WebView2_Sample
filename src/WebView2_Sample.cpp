// WebView2_Sample.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "WebView2_Sample.h"
#include "CHTMLhost.h" 
#include "osutility.h"


int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow)
{
    // Parse argument. 
    int argc = 0;
    LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

    std::wstring_view webView2Version = L"";
    if (argc > 1)
    {   // Assume first argument is WebView2 version to use, in the format "x.y.z.t".
        webView2Version = argv[1];
        ATLTRACE("User-provided WebView2 version=%ls\n", webView2Version.data());
    }

    std::wstring_view webView2Channel = L"";
    if (argc > 2)
    {   // Assume second argument is WebView2 channel to use: "beta", "dev", "canary" or  "fixed" "" for stable channel.
        webView2Channel = argv[2];
        ATLTRACE("User-provided WebView2 channel=%ls\n", webView2Channel.data());
    }

    std::wstring_view webViewFolder = L"";
    if (argc > 3)
    {   // Assume second argument of WebView2 channel to use: "fixed".
        // we read the folder
        webViewFolder = argv[3];
        ATLTRACE("User-provided WebView2 root folder=%ls\n", webViewFolder.data());
    }



    // Set DPI awareness to PerMonitorV2.
    DPI_AWARENESS_CONTEXT dpiAwarenessContext = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
    // Available since Windows 10 1703.
    if (!::SetProcessDpiAwarenessContext(dpiAwarenessContext))
    {
        auto error = ::GetLastError();
        ATLTRACE("Failed to set DPI context\n");
        ATLTRACE("function=%s, message=%s, error=%lu. Ignoring...\n", __func__, 
            std::system_category().message(error).data(), error);
    }

    HRESULT hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if FAILED(hr)
    {
        ATLTRACE("Failed to initialize COM\n");
        ATLTRACE("function=%s, message=%s, hr=%d\n", __func__,
            std::system_category().message(hr).data(), hr);
        return -1;
    }

    // Verify that the WebView2 runtime is installed.
    PWSTR edgeVersionInfo = nullptr;
    hr = ::GetAvailableCoreWebView2BrowserVersionString(nullptr, &edgeVersionInfo);
    if (FAILED(hr) || (edgeVersionInfo == nullptr))
    {
        ATLTRACE("The WebView2 runtime is not installed\n");
        ::MessageBoxW(nullptr, L"Please install the WebView2 runtime before running this application,"
            L" available on https://go.microsoft.com/fwlink/p/?LinkId=2124703", L"Missing WebView2 runtime", 
            MB_OK | MB_ICONERROR);
        return - 1;
    }

    ATLTRACE("Found installed WebView version %ls\n", edgeVersionInfo);

    if (webView2Version.empty())
    {   // User did not provided specific WebView2 versions and channels.
        // Set WebView2 version and channel to default values. 
        std::wstring_view edgeVersionInfoStr = edgeVersionInfo;
        size_t pos = edgeVersionInfoStr.find(L' ');

        if ((edgeVersionInfoStr.size() > 0) && (pos < edgeVersionInfoStr.size() - 1))
        {   // Assume Edge version with format 'x.y.z.t channel"
            webView2Version = edgeVersionInfoStr.substr(0, pos);
            edgeVersionInfo[pos] = L'\0'; // Ensure webView2Version is null-terminated.
            webView2Channel = edgeVersionInfoStr.substr(pos + 1, edgeVersionInfoStr.size() - pos - 1);
        }
        else
        {   // Assume Edge version with format 'x.y.z.t"
            webView2Version = edgeVersionInfoStr;
        }

        ATLTRACE("Using WebView2 version=%ls\n", webView2Version.data());
        ATLTRACE("Using WebView2 channel=%ls\n", webView2Channel.data());
    }
    CHTMLhost host;
    host.ShowDialog(L"http://msdn.microsoft.com", osutility::GetBrowserDirectory(webView2Version, webView2Channel, webViewFolder),
        osutility::GetUserDataDirectory(webView2Channel));
    
    // Cleanup.
    ::LocalFree(edgeVersionInfo);
    ::CoUninitialize();
    
    return 0;
}