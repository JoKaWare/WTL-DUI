
#include "demo_html.h"

#include "base/sys_string_conversions.h"
#include "ui/views/controls/activex/html_view.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/grid_layout.h"

#include "demo_main.h"

DemoHtml::DemoHtml( ) :  
html_view_(NULL), html_(NULL), button_(NULL),view_created_(false)
{
	
}

DemoHtml::~DemoHtml() {}

std::wstring DemoHtml::GetTitle()
{
    return std::wstring(L"Html Document");
}

void DemoHtml::InitView(views::View* container)
{
    views::GridLayout* layout = new views::GridLayout(container);
    container->SetLayoutManager(layout);

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);

    views::ColumnSet* column_set1 = layout->AddColumnSet(1);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        0.0f, views::GridLayout::USE_PREF, 0, 0);

    layout->StartRow(1.0f, 0);
    html_view_ = new views::HtmlView();
    layout->AddView(html_view_);

    layout->StartRow(0.0f, 1);
    html_ = new views::Textfield();
    html_->SetText(L"<hr><p>I'm ÍòĞÇĞÇ</p><input/>");
    layout->AddView(html_);
    button_ = new views::NativeTextButton(this, L"äÖÈ¾");
    layout->AddView(button_);
}

void DemoHtml::ButtonPressed(views::Button* sender,
                             const views::Event& event)
{
    if(button_ == sender)
    {
        html_view_->SetHtml(base::SysWideToNativeMB(html_->text()));
    }
}