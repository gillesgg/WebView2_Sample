#include "pch.h"
#include "CHTMLhost.h"
#include "CHtmlDialog.h"
#include "Utility/Logger/Logger.h"



INT_PTR CHTMLhost::ShowDialog(std::wstring url)
{

	if (!url.empty())
	{
		CHtmlDialog dialog(url);

		dialog.RegisterCallback(CHtmlDialog::CallbackType::CreationCompleted, [this]()
		{
			LOG_INFO << __FUNCTION__ << " CallbackType::CreationCompleted";

		});

		dialog.RegisterCallback(CHtmlDialog::CallbackType::NavigationCompleted, [this]()
		{
			LOG_INFO << __FUNCTION__ << " CallbackType::NavigationCompleted";
		});

		dialog.RegisterCallback(CHtmlDialog::CallbackType::AutentCompleted, [this]()
		{
			LOG_INFO << __FUNCTION__ << " CallbackType::AutentCompleted";
		});
		return dialog.DoModal();
	}
	return -1;
}