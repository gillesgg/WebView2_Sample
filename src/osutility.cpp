#include "pch.h"
#include "osutility.h"

// Returns Program Files directory x86.
std::wstring osutility::GetProgramFilesx86Directory()
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
std::wstring_view osutility::GetRootEdgeDirectory(std::wstring_view webView2Channel)
{
    static std::map<std::wstring_view, std::wstring_view, WStringIgnoreCaseLess> channels =
    {   // Name of Edge directory based on release channel.
        { L"",          L"EdgeWebView" },
        { L"beta",      L"Edge Beta" },
        { L"dev",       L"Edge Dev" },
        { L"canary",    L"Edge Canary" },
        { L"Fixed",     L"Fixed" },
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
std::wstring osutility::GetBrowserDirectory(std::wstring_view webView2Version, std::wstring_view webView2Channel, std::wstring_view webViewFolder)
{
    std::wstring browserDirectory = L"";
    std::wstring_view rootEdgeDirectory = GetRootEdgeDirectory(webView2Channel);

    if (!webView2Version.empty() && !rootEdgeDirectory.empty() && webViewFolder.empty())
    {   // Build directory from env var and channel.
        std::wstring programFilesx86Directory = GetProgramFilesx86Directory();
        std::wstring_view format = LR"(%s\Microsoft\%s\Application\%s)";

        size_t length = swprintf(nullptr, 0, format.data(),programFilesx86Directory.data(), rootEdgeDirectory.data(), webView2Version.data());
        browserDirectory.resize(length + 1);

        swprintf(browserDirectory.data(), browserDirectory.size(), format.data(),programFilesx86Directory.data(), rootEdgeDirectory.data(), webView2Version.data());
        browserDirectory.resize(length); // Remove trailing L'\0'.

        fs::path browserPath(browserDirectory);
        browserPath /= L"msedge.exe";
        if (!fs::exists(browserPath))
        {   // Return empty string if browser executable is not found.
            ATLTRACE("Incorrect browser path. File '%ls' not found\n", browserPath.c_str());
            return std::wstring();
        }
    }
    if (!webViewFolder.empty())
    {
        fs::path browserPath(webViewFolder);
        browserPath /= L"msedgewebview2.exe";
        if (!fs::exists(browserPath))
        {   // Return empty string if browser executable is not found.
            ATLTRACE("Incorrect browser path. File '%ls' not found\n", browserPath.c_str());
            return std::wstring();
        }
        browserDirectory = webViewFolder;
    }
    return browserDirectory;
}

// Get directory containing the user data based on webView2Version and webView2Channel.
// Returns empty string if webView2Version is empty.
std::wstring osutility::GetUserDataDirectory(std::wstring_view webView2Channel)
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



std::wstring osutility::GetLangStringFromLangId(DWORD dwLangID_i, bool returnShortCode)
{
    const int MAX_LANG_LEN = 81;

    // Prepare LCID
    const LCID lcidLang = MAKELCID(dwLangID_i, SORT_DEFAULT);

    // Will hold language
    TCHAR szLangBuffer[MAX_LANG_LEN] = { 0 };

    LCTYPE resultFormat = (returnShortCode ? LOCALE_SISO639LANGNAME : LOCALE_SENGLANGUAGE);
    DWORD dwCount = GetLocaleInfo(lcidLang, resultFormat, szLangBuffer, MAX_LANG_LEN);
    if (!dwCount)
    {
        ATLTRACE(_T("Failed to get locale language information"));
        return _T("");
    }

    // Will hold country
    TCHAR szCountryBuffer[MAX_LANG_LEN] = { 0 };
    resultFormat = (returnShortCode ? LOCALE_SISO3166CTRYNAME : LOCALE_SENGCOUNTRY);

    // Get country
    dwCount = GetLocaleInfo(lcidLang, resultFormat, szCountryBuffer, MAX_LANG_LEN);

    if (!dwCount)
    {
        ATLTRACE(_T("Failed to get locale country information"));
        return szLangBuffer;
    }// End if

    std::wstring combinedResult(szLangBuffer);
    combinedResult += _T("-");
    combinedResult += szCountryBuffer;
    return combinedResult;
}

std::wstring osutility::GetUserMUI()
{
    auto lang_id = GetUserDefaultUILanguage();
    return GetLangStringFromLangId(lang_id, true);
}