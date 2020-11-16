#include "pch.h"
#include "CHTMLhost.h"
#include "CHtmlDialog.h"


INT_PTR CHTMLhost::ShowDialog(std::wstring_view url, std::wstring_view browserDirectory)
{
	if (!url.empty())
	{
		CHtmlDialog dialog(url, browserDirectory);

		dialog.RegisterCallback(CHtmlDialog::CallbackType::CreationCompleted, [this]()
		{
			ATLTRACE("message=%s\n", "CallbackType::CreationCompleted");
		});

		dialog.RegisterCallback(CHtmlDialog::CallbackType::NavigationCompleted, [this]()
		{
			ATLTRACE("message=%s\n", "CallbackType::AutentCompleted");
		});

		dialog.RegisterCallback(CHtmlDialog::CallbackType::AutentCompleted, [this]()
		{
			ATLTRACE("message=%s\n", "CallbackType::AutentCompleted");
		});
		return dialog.DoModal();
	}
	return -1;
}