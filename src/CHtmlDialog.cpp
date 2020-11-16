#include "pch.h"
#include "CHtmlDialog.h"


#define DOMAIN_PROP L"domain"
#define EXPIRES_PROP L"expires"
#define PATH_PROP L"path"
#define VALUE_PROP L"value"
#define NAME_PROP L"name"

CHtmlDialog::CHtmlDialog(std::wstring_view url, std::wstring_view browserDirectory)
{
	webview2imp_ = std::make_unique<WebView2Impl>();
	m_callbacks[CallbackType::CreationCompleted] = nullptr;
	m_callbacks[CallbackType::NavigationCompleted] = nullptr;
	m_callbacks[CallbackType::AutentCompleted] = nullptr;
	url_ = url;
	browserDirectory_ = browserDirectory;
}

CHtmlDialog::CHtmlDialog() {}

CHtmlDialog::~CHtmlDialog()
{
	CloseWebView();
}
LRESULT	CHtmlDialog::OnCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto* task = reinterpret_cast<CallbackFunc*>(wParam);
	(*task)();
	delete task;

	return bHandled = TRUE;
}



LRESULT CHtmlDialog::OnInitDialog(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	CenterWindow();
	HICON hIcon = AtlLoadIconImage(IDI_WEBVIEW2SAMPLE, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDI_WEBVIEW2SAMPLE, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);
	auto hr = InitWebView();
	bHandled = FALSE;
	return (hr);
}

LRESULT	CHtmlDialog::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ResizeToClientArea();
	return bHandled = FALSE;
}


LRESULT	CHtmlDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	EndDialog(0);	
	return bHandled = FALSE;
}

void CHtmlDialog::CloseWebView()
{
	if (webview2imp_ && webview2imp_->webView_ && webview2imp_->webController_ && webview2imp_->webViewEnvironment_ && webview2imp_->webSettings_)
	{
		webview2imp_->webView_->remove_NavigationCompleted(m_navigationCompletedToken);
		webview2imp_->webView_->remove_NavigationStarting(m_navigationStartingToken);
		webview2imp_->webView_->remove_DocumentTitleChanged(m_documentTitleChangedToken);
		webview2imp_->webView_ = nullptr;
		webview2imp_->webController_->Close();
		webview2imp_->webController_ = nullptr;
		webview2imp_->webSettings_ = nullptr;
		webview2imp_->webViewEnvironment_ = nullptr;
	}
	else
	{
		ATLTRACE("function=%s, message=unable to release webview2imp_\n", __func__ );
	}
}

std::wstring CHtmlDialog::GetAppDataDirectory()
{
	TCHAR path[MAX_PATH];
	std::wstring dataDirectory;
	HRESULT hr = SHGetFolderPath(nullptr, CSIDL_APPDATA, NULL, 0, path);
	if (SUCCEEDED(hr))
	{
		dataDirectory = std::wstring(path);
		dataDirectory.append(L"\\Microsoft\\");
	}
	else
	{
		dataDirectory = std::wstring(L".\\");
	}

	dataDirectory.append(L"WebView_Sample");
	return dataDirectory;
}

HRESULT CHtmlDialog::InitWebView()
{
	ATLTRACE("function=%s\n", __func__);

	std::wstring userDataDirectory = GetAppDataDirectory();
	userDataDirectory.append(L"\\User Data");


	auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
	HRESULT hr = options->put_AllowSingleSignOnUsingOSPrimaryAccount(TRUE);
	if FAILED(hr)
	{
		ATLTRACE("Failed to enable SSO");
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
	}

	hr = options->put_Language(L"en-us");
	if FAILED(hr)
	{
		ATLTRACE("Failed to set language to en-us");
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
	}

	hr = CreateCoreWebView2EnvironmentWithOptions(browserDirectory_.empty() ? nullptr : browserDirectory_.data(), 
		userDataDirectory.data(), options.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &CHtmlDialog::OnCreateEnvironmentCompleted).Get());



	if FAILED(hr)
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
	}

	if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
	{
		MessageBox(L"Unable to find the WebView2 engine", L"WebView_Sample", MB_OK | MB_ICONERROR);
	}

	return (hr);
}


HRESULT CHtmlDialog::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment)
{
	ATLTRACE("function=%s\n", __func__);

	if (result == S_OK)
	{
		if (!webview2imp_)
		{
			ATLTRACE("function=%s message=webview is null\n", __func__);
			result = E_INVALIDARG;
			return result;
		}

		result = environment->QueryInterface(IID_PPV_ARGS(&webview2imp_->webViewEnvironment_));
		if FAILED(result)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(result).data(), result);
			return (result);
		}
		result = webview2imp_->webViewEnvironment_->CreateCoreWebView2Controller(m_hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &CHtmlDialog::OnCreateWebViewControllerCompleted).Get());
		if FAILED(result)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(result).data(), result);
		}
	}
	else
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(result).data(), result);
	}
	return (result);
}


HRESULT CHtmlDialog::ResizeToClientArea()
{
	HRESULT hr = S_OK;
	if (webview2imp_ && webview2imp_->webController_)
	{
		RECT bounds;
		GetClientRect(&bounds);
		hr = webview2imp_->webController_->put_Bounds(bounds);
		if (FAILED(hr))
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}
	}
	return (hr);
}

// TODO : remplace E_FAIL with user define error 
HRESULT CHtmlDialog::OnCreateWebViewControllerCompleted(HRESULT result, ICoreWebView2Controller* controller)
{
	ATLTRACE("function=%s\n", __func__);

	HRESULT hr = S_OK;

	if (result != S_OK || controller == nullptr)
	{
		ATLTRACE("function=%s, message=cannot create webview environment\n", __func__);
		return E_INVALIDARG;
	}

	webview2imp_->webController_ = controller;
	if FAILED(hr = controller->get_CoreWebView2(&webview2imp_->webView_))
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		return hr;
	}
	if FAILED(hr = webview2imp_->webView_->get_Settings(&webview2imp_->webSettings_))
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		return hr;
	}
	if FAILED(hr = RegisterEventHandlers())
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		return hr;
	}
	if FAILED(hr = ResizeToClientArea())
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		return hr;
	}
	auto callback = m_callbacks[CallbackType::CreationCompleted];
	if (callback == nullptr)
	{
		ATLTRACE("function=%s, message=unable to create callback", __func__);
		return E_FAIL;
	}
	RunAsync(callback);
	return hr;
}


HRESULT CHtmlDialog::RegisterEventHandlers()
{
	ATLTRACE("function=%s\n", __func__);

	HRESULT hr;

	// NavigationCompleted handler
	hr = webview2imp_->webView_->add_NavigationCompleted(Callback<ICoreWebView2NavigationCompletedEventHandler>(
		[this](
			ICoreWebView2*,
			ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
		{
			m_isNavigating = false;
			HRESULT hr;
			BOOL success;
			hr = args->get_IsSuccess(&success);

			if (!success)
			{
				COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus{};
				hr = args->get_WebErrorStatus(&webErrorStatus);
				if (webErrorStatus == COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED)
				{
					ATLTRACE("function=%s message=COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED\n", __func__);
				}
			}

			wil::unique_cotaskmem_string uri;
			webview2imp_->webView_->get_Source(&uri);

			if (wcscmp(uri.get(), L"about:blank") == 0)
			{
				uri = wil::make_cotaskmem_string(L"");
			}

			auto callback = m_callbacks[CallbackType::NavigationCompleted];
			if (callback != nullptr)
				RunAsync(callback);

			return S_OK;
		})
		.Get(), &m_navigationCompletedToken);
	if FAILED(hr)
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
	}
	// NavigationStarting handler
	hr = webview2imp_->webView_->add_NavigationStarting(
		Callback<ICoreWebView2NavigationStartingEventHandler>(
			[this](
				ICoreWebView2*,
				ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
			{
				wil::unique_cotaskmem_string uri;
				args->get_Uri(&uri);

				m_isNavigating = true;

				return S_OK;
			}).Get(), &m_navigationStartingToken);


	hr = webview2imp_->webView_->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
	if FAILED(hr)
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
	}
	hr = webview2imp_->webView_->add_WebResourceRequested(
		Callback<ICoreWebView2WebResourceRequestedEventHandler>(
			[this](
				ICoreWebView2*,
				ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT
			{
				wil::com_ptr <ICoreWebView2WebResourceRequest> request = nullptr;
				wil::com_ptr <ICoreWebView2HttpRequestHeaders> headers = nullptr;

				auto hr = args->get_Request(&request);
				if FAILED(hr)
				{
					ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
					return (hr);
				}
				hr = request->get_Headers(&headers);
				if FAILED(hr)
				{
					ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
					return (hr);
				}
				BOOL hasheader = FALSE;
				hr = headers->Contains(L"Authorization", &hasheader);
				if (hasheader == TRUE && hr == S_OK)
				{
					LPTSTR authV = new TCHAR[1000];
					hr = headers->GetHeader(L"Authorization", &authV);
					if (hr == S_OK)
					{
						ATLTRACE(L"name=%s value=%s\n", L"Authorization", authV);
						auto callback = m_callbacks[CallbackType::AutentCompleted];
						if (callback == nullptr)
						{
							ATLTRACE("function=%s message=unable to create callback\n", __func__);
							return E_FAIL;
						}
						RunAsync(callback);
						return S_OK;
					}
				}
				return hr;

			}).Get(), &webresourcerequestedToken_);


	if FAILED(hr)
	{
		ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
	}
	if (!url_.empty())
		hr = webview2imp_->webView_->Navigate(url_.c_str());
	else
		hr = webview2imp_->webView_->Navigate(L"about:blank");
	return (hr);
}

void CHtmlDialog::RegisterCallback(CallbackType const type, CallbackFunc callback)
{
	m_callbacks[type] = callback;
}

void CHtmlDialog::RunAsync(CallbackFunc callback)
{
	auto* task = new CallbackFunc(callback);
	PostMessage(MSG_RUN_ASYNC_CALLBACK, reinterpret_cast<WPARAM>(task), 0);
}


std::wstring_view CHtmlDialog::GetURL()
{
	std::wstring url;
	if (webview2imp_->webView_)
	{
		wil::unique_cotaskmem_string uri;
		webview2imp_->webView_->get_Source(&uri);

		if (wcscmp(uri.get(), L"about:blank") == 0)
		{
			uri = wil::make_cotaskmem_string(L"");
		}

		url = uri.get();
	}

	return url;
}


HRESULT CHtmlDialog::NavigateTo(std::wstring_view url)
{
	if (!url.empty())
	{
		std::wstring surl(url);

		if (surl.find(L"://") < 0)
		{
			if (surl.length() > 1 && surl[1] == ':')
				surl = L"file://" + surl;
			else
				surl = L"http://" + surl;
		}
		return(webview2imp_->webView_->Navigate(surl.c_str()));
	}
	else
	{
		return E_INVALIDARG;
	}
}

HRESULT CHtmlDialog::Navigate(std::wstring_view url, CallbackFunc onComplete)
{
	if (webview2imp_->webView_)
	{
		m_callbacks[CallbackType::NavigationCompleted] = onComplete;
		return (NavigateTo(url));
	}
	else
	{
		return E_FAIL;
	}
}
/// <summary>
/// Disable browser popup menu
/// </summary>
/// <returns></returns>
HRESULT CHtmlDialog::DisablePopups()
{
	if (webview2imp_->webSettings_)
	{
		return (webview2imp_->webSettings_->put_AreDefaultScriptDialogsEnabled(FALSE));
	}
	else
	{
		return E_FAIL;
	}
}
/// <summary>
/// Disable developper tooling
/// </summary>
/// <returns></returns>
HRESULT CHtmlDialog::DisableDevelopper()
{
	if (webview2imp_->webSettings_)
	{
		return (webview2imp_->webSettings_->put_AreDevToolsEnabled(FALSE));
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT CHtmlDialog::GetCookies()
{
	HRESULT hr = S_OK;

	if (webview2imp_->webSettings_)
	{
		wil::com_ptr<ICoreWebView2DevToolsProtocolEventReceiver> receiver;
		hr = webview2imp_->webView_->GetDevToolsProtocolEventReceiver(L"Network.getAllCookies", &receiver);
		if FAILED(hr)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
			return (hr);
		}
		hr = webview2imp_->webView_->CallDevToolsProtocolMethod(
			L"Network.getAllCookies",
			L"{}",
			Callback<ICoreWebView2CallDevToolsProtocolMethodCompletedHandler>(
				[](HRESULT error, PCWSTR resultJson) -> HRESULT
				{
					if (SUCCEEDED(error))
					{
						utility::stringstream_t jsonCookieArray;
						jsonCookieArray << resultJson;
						web::json::value jsonCookieArrayValue = web::json::value::parse(jsonCookieArray);
						if (jsonCookieArrayValue.is_object() == true)
						{
							for (auto iter = std::begin(jsonCookieArrayValue.as_object()); iter != std::end(jsonCookieArrayValue.as_object()); ++iter)
							{
								std::wstring name = iter->first;
								web::json::value cookiecoll = iter->second;

								if (cookiecoll.is_array() == true)
								{
									for (auto iter1 = std::begin(cookiecoll.as_array()); iter1 != std::end(cookiecoll.as_array()); ++iter1)
									{
										auto& cookie = *iter1;
										if (cookie.is_object() == true)
										{
											auto vdomain = cookie[DOMAIN_PROP].serialize();
											auto vexpire = cookie[EXPIRES_PROP].serialize();

											// TODO : convert the date
											auto dt = utility::datetime::from_string(vexpire, utility::datetime::RFC_1123);
											uint64_t interval = dt.to_interval();

											auto vpath = cookie[PATH_PROP].serialize();
											auto vvalue = cookie[VALUE_PROP].serialize();
											auto vname = cookie[NAME_PROP].serialize();

										}
									}
								}
							}
						}
					}
					return S_OK;
				}).Get());
	}
	else
	{
		return E_FAIL;
	}
	return (hr);
}