
#include <tchar.h>
#include <windows.h>
#include <initguid.h>
#include <oleacc.h>
#include "base/at_exit.h"
#include <atlbase.h>
#include "base/process_util.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "ui/base/ui_base_paths.h"
#include "base/i18n/icu_util.h"
#include "base/message_loop.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/widget/widget.h"
#include "ui/base/resource/resource_bundle_win.h"
#include "base/native_library.h"
#include "ui/base/win/scoped_ole_initializer.h"

#include "demo_main.h"


CComModule _Module;


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	ui::ScopedOleInitializer OleIniter;

	 _Module.Init(NULL, hInstance);

	CommandLine::Init(0, NULL);

	base::EnableTerminationOnHeapCorruption();

	base::AtExitManager exit_manager;

	icu_util::Initialize();

	ui::RegisterPathProvider();

	ui::ResourceBundle::InitSharedInstanceWithLocale("zh-CN", NULL);

	MessageLoopForUI main_message_loop;
	
	views::Widget* win = views::Widget::CreateWindowWithBounds(new DemoMain, gfx::Rect(400, 200, 850, 300));
	win->Show();

	MessageLoopForUI::current()->Run();

	ui::ResourceBundle::CleanupSharedInstance();


	return 0;
}



// 提升公共控件样式.
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif