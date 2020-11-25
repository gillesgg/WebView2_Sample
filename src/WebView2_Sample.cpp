// WebView2_Sample.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "WebView2_Sample.h"
#include "CHTMLhost.h" 

// Returns Program Files directory x86.
std::wstring GetProgramFilesx86Directory()
{
    constexpr std::wstring_view programFilesVarName = L"ProgramFiles";
    constexpr std::wstring_view programFilesx86VarName = L"ProgramFiles(x86)";

    std::wstring programFilesx86Directory;
    size_t size = 0;

    if ((_wgetenv_s(&size, nullptr, 0, programFilesx86VarName.data()) == 0) && (size > 0))
    {   // Found env var %ProgramFiles(x86)%. Get its value.
        programFilesx86Directory.resize(size);
        _wgetenv_s(&size, programFilesx86Directory.data(), programFilesx86Directory.size(), 
            programFilesx86VarName.data());
    }
    else
    {   // Assume x86 system. Try env var %ProgramFiles%.
        if ((_wgetenv_s(&size, nullptr, 0, programFilesVarName.data()) == 0) && (size > 0))
        {   // Found env var %ProgramFiles%. Get its value.
            programFilesx86Directory.resize(size);
            _wgetenv_s(&size, programFilesx86Directory.data(), programFilesx86Directory.size(), 
                programFilesVarName.data());
        }
        else
        {
            ATLTRACE("Failed to retrieve %%%ls%% and %%%ls%% environment variables\n",
                programFilesx86VarName.data(), programFilesVarName.data());
        }
    }
    
    return programFilesx86Directory;
}

// Use for case-insensitive wstring keys in std::map.
struct WStringIgnoreCaseLess
{
    bool operator()(std::wstring_view s1, std::wstring_view s2) const
    {

        return _wcsicmp(s1.data(), s2.data()) < 0;
    }
};

// Get root Edge directory based on the channel.
// Returns empty string if unknown channel.
std::wstring_view GetRootEdgeDirectory(std::wstring_view webView2Channel)
{
    static std::map<std::wstring_view, std::wstring_view, WStringIgnoreCaseLess> channels =
    {   // Name of Edge directory based on release channel.
        { L"",          L"Edge" },
        { L"beta",      L"Edge Beta" },
        { L"dev",       L"Edge Dev" },
        { L"canary",    L"Edge Canary" },
    };

    auto pos = channels.find(webView2Channel);

    if (pos == channels.end())
    {   // Invalid channel value.
        ATLTRACE("Incorrect channel value: \"%ls\". Allowed values:\n", webView2Channel.data());
        for (const auto& channelEntry : channels)
        {
            ATLTRACE("\"%ls\"\n", channelEntry.first.data());
        }

        return L"";
    }

    return pos->second;
}

// Get directory containing the browser based on webView2Version and webView2Channel.
// Returns empty string if webView2Version is empty.
std::wstring GetBrowserDirectory(std::wstring_view webView2Version, std::wstring_view webView2Channel)
{
    std::wstring browserDirectory = L"";
    std::wstring_view rootEdgeDirectory = GetRootEdgeDirectory(webView2Channel);

    if (!webView2Version.empty() && !rootEdgeDirectory.empty())
    {   // Build directory from env var and channel.
        std::wstring programFilesx86Directory = GetProgramFilesx86Directory();
        std::wstring_view format = LR"(%s\Microsoft\%s\Application\%s)";
        
        size_t length = swprintf(nullptr, 0, format.data(),
            programFilesx86Directory.data(), rootEdgeDirectory.data(), webView2Version.data());
        browserDirectory.resize(length + 1);
        
        swprintf(browserDirectory.data(), browserDirectory.size(), format.data(), 
            programFilesx86Directory.data(), rootEdgeDirectory.data(), webView2Version.data());
        browserDirectory.resize(length); // Remove trailing L'\0'.

        fs::path browserPath(browserDirectory);
        browserPath /= L"msedge.exe";
        if (!fs::exists(browserPath))
        {   // Return empty string if browser executable is not found.
            ATLTRACE("Incorrect browser path. File '%ls' not found\n", browserPath.c_str());
            return std::wstring();
        }
    }

    return browserDirectory;
}

// Get directory containing the user data based on webView2Version and webView2Channel.
// Returns empty string if webView2Version is empty.
std::wstring GetUserDataDirectory(std::wstring_view webView2Channel)
{
    std::wstring userDirectory(MAX_PATH, L'\0');

    if FAILED(::SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, NULL, 0, userDirectory.data()))
    {   // Use current directory as default.
        userDirectory = L".";
    }

    std::wstring_view rootEdgeDirectory = GetRootEdgeDirectory(webView2Channel);

    if (rootEdgeDirectory.empty())
    {   // Use stable channel as default. 
        rootEdgeDirectory = GetRootEdgeDirectory(L"");
    }

    std::wstring_view format = LR"(%s\Microsoft\%s\User Data)";
    size_t length = swprintf(nullptr, 0, format.data(), 
        userDirectory.data(), rootEdgeDirectory.data());
    std::wstring userDataDirectory(length + 1, L'\0');

    swprintf(userDataDirectory.data(), userDataDirectory.size(), format.data(), 
        userDirectory.data(), rootEdgeDirectory.data());
    userDataDirectory.resize(length);

    return userDataDirectory;
}


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
    {   // Assume second argument is WebView2 channel to use: "beta", "dev", "canary" or "" for stable channel.
        webView2Channel = argv[2];
        ATLTRACE("User-provided WebView2 channel=%ls\n", webView2Channel.data());
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
    host.ShowDialog(L"http://msdn.microsoft.com", GetBrowserDirectory(webView2Version, webView2Channel),
        GetUserDataDirectory(webView2Channel));
    
    // Cleanup.
    ::LocalFree(edgeVersionInfo);
    ::CoUninitialize();
    
    return 0;
}
