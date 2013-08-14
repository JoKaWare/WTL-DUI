
#include "demo_main.h"

#include "base/i18n/icu_util.h"
#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/path_service.h"
#include "ui/base/ui_base_paths.h"
#include "base/process_util.h"
#include "base/stringprintf.h"
#include "ui/gfx/image/image.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane.h"
#include "ui/views/focus/accelerator_handler.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/widget/widget.h"


DemoMain::DemoMain()  :contents_(NULL), status_label_(NULL) {}

DemoMain::~DemoMain() {}

bool DemoMain::CanResize() const
{
    return true;
}

std::wstring DemoMain::GetWindowTitle() const
{
    return std::wstring(L"Base Widget Demo");
}

views::View* DemoMain::GetContentsView()
{
    return contents_;
}

void DemoMain::WindowClosing()
{
    MessageLoopForUI::current()->Quit();
}

views::Widget* DemoMain::GetWidget()
{
    return contents_->GetWidget();
}

const views::Widget* DemoMain::GetWidget() const
{
    return contents_->GetWidget();
}

void DemoMain::TabSelectedAt(int index)
{
    SetStatus(base::StringPrintf(L"Select tab: %d", index));
}

void DemoMain::SetStatus(const std::wstring& status)
{
    status_label_->SetText(status);
}

void DemoMain::Run()
{
    base::EnableTerminationOnHeapCorruption();

    base::AtExitManager exit_manager;

	icu_util::Initialize();
	
	ui::RegisterPathProvider();
	ui::ResourceBundle::InitSharedInstanceWithLocale("en-US", NULL);

	

    MessageLoopForUI main_message_loop;

   DCHECK(contents_ == NULL) << "Run called more than once.";
    contents_ = new views::View();
    contents_->set_background(views::Background::CreateStandardPanelBackground());
    views::GridLayout* layout = new views::GridLayout(contents_);
    contents_->SetLayoutManager(layout);
 
    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 1,
        views::GridLayout::USE_PREF, 0, 0);

    views::TabbedPane* tabbed_pane = new views::TabbedPane();
    tabbed_pane->set_listener(this);
    status_label_ = new views::Label();

    layout->StartRow(1, 0);
    layout->AddView(tabbed_pane);
    layout->StartRow(0, 0);
    layout->AddView(status_label_);

    views::Widget* window = views::Widget::CreateWindowWithBounds(this,
        gfx::Rect(200, 200, 800, 400));

  //  DemoDownload demo_download(this);
    tabbed_pane->AddTab( L"eee", new views::Label( L"weo") );

    window->Show();

    MessageLoopForUI::current()->Run();

    ui::ResourceBundle::CleanupSharedInstance();
}