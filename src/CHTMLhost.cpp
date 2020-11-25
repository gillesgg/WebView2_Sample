#include "pch.h"
#include "CHTMLhost.h"
#include "CHtmlDialog.h"


INT_PTR CHTMLhost::ShowDialog(std::wstring_view url, std::wstring_view browserDirectory,
	std::wstring_view userDataDirectory)
{
	if (!url.empty())
	{
		CHtmlDialog dialog(url, browserDirectory, userDataDirectory);

		dialog.RegisterCallback(CHtmlDialog::CallbackType::CreationCompleted, [this]()
		{
			ATLTRACE("message=%s\n", "CallbackType::CreationCompleted");
		});

		dialog.RegisterCallback(CHtmlDialog::CallbackType::NavigationCompleted, [this]()
		{
			ATLTRACE("message=%s\n", "CallbackType::NavigationCompleted");
		});

		dialog.RegisterCallback(CHtmlDialog::CallbackType::AuthenticationCompleted, [this]()
		{
			ATLTRACE("message=%s\n", "CallbackType::AuthenticationCompleted");
		});
		return dialog.DoModal();
	}
	return -1;
}