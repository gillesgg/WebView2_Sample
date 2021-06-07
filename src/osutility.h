#pragma once
class osutility
{
public:
    static std::wstring GetUserMUI();
    static std::wstring GetBrowserDirectory(std::wstring_view webView2Version, std::wstring_view webView2Channel, std::wstring_view webViewFolder);
    static std::wstring GetUserDataDirectory(std::wstring_view webView2Channel);
private:
    static std::wstring GetProgramFilesx86Directory();
    static std::wstring_view GetRootEdgeDirectory(std::wstring_view webView2Channel);
    static std::wstring GetLangStringFromLangId(DWORD dwLangID_i, bool returnShortCode);

};