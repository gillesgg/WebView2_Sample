// WebView2_Sample.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "WebView2_Sample.h"
#include "CHTMLhost.h"


// Global Variables:
HINSTANCE hInst;                                // current instance


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    CoInitialize(0L);

    CHTMLhost host;

    

    host.ShowDialog(L"http://msdn.microsoft.com");

    

}
