#pragma once
class CHTMLhost
{
public:
	INT_PTR ShowDialog(std::wstring_view url, std::wstring_view browserDirectory,
		std::wstring_view userDataDirectory);
};

