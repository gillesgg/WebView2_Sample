#pragma once

#include "Resource.h"

using namespace Microsoft::WRL;

class WebView2Impl
{
public:
	wil::com_ptr<ICoreWebView2Environment> webViewEnvironment_ = nullptr;
	wil::com_ptr<ICoreWebView2>            webView_ = nullptr;
	wil::com_ptr<ICoreWebView2Controller>  webController_ = nullptr;
	wil::com_ptr<ICoreWebView2Settings>    webSettings_ = nullptr;
};

class CHtmlDialog : public CDialogImpl<CHtmlDialog>
{
public:
	enum { IDD = IDD_MAINDLG };

	enum class CallbackType
	{
		CreationCompleted,
		NavigationCompleted,
		TitleChanged,
		AutentCompleted,
	};
	using CallbackFunc = std::function<void(void)>;



public:
	BEGIN_MSG_MAP(CHtmlDialogBase)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(MSG_RUN_ASYNC_CALLBACK, OnCallBack)
	END_MSG_MAP()
public:
	CHtmlDialog(std::wstring url);
	CHtmlDialog();
	~CHtmlDialog();
	void		RegisterCallback(CallbackType const type, CallbackFunc callback);

	LRESULT		OnCallBack(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT		OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT		OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT		OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

public:
	std::wstring_view GetURL();
	HRESULT NavigateTo(std::wstring_view url);
	HRESULT		DisablePopups();
	HRESULT		Navigate(std::wstring_view url, CallbackFunc onComplete);
	HRESULT		DisableDevelopper();
	HRESULT		GetCookies();
private:
	HRESULT		InitWebView();
	void		CloseWebView();
	HRESULT		OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT		ResizeToClientArea();
	HRESULT		OnCreateWebViewControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
	HRESULT		RegisterEventHandlers();
	void		RunAsync(CallbackFunc callback);


private:
	std::map<CallbackType, CallbackFunc>	m_callbacks;
	std::unique_ptr<WebView2Impl>			webview2imp_;
	bool									m_isNavigating = false;
	std::wstring							url_;
	EventRegistrationToken					m_navigationStartingToken = {};
	EventRegistrationToken					m_navigationCompletedToken = {};
	EventRegistrationToken					m_documentTitleChangedToken = {};
	EventRegistrationToken					webresourcerequestedToken_ = {};

};

