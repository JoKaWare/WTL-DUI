
#include "demo_label.h"

#include "ui/base/resource/resource_bundle.h"

#include "ui/views/controls/link.h"
#include "ui/views/controls/separator.h"
#include "ui/views/layout/box_layout.h"

#include "demo_main.h"

DemoLabel::DemoLabel()  { view_created_ = false; }

DemoLabel::~DemoLabel() {}

std::wstring DemoLabel::GetTitle()
{
	
    return std::wstring(L"Label & Link");
}

void DemoLabel::InitView(views::View* container)
{
    link_ = new views::Link(L"Click me!");
    link_->set_listener(this);
    link_custom_ = new views::Link(L"Click me!");
    link_custom_->set_listener(this);
    link_custom_->SetFont(ui::ResourceBundle::GetSharedInstance().GetFont(
        ui::ResourceBundle::BoldFont));
    link_custom_->SetEnabledColor(SK_ColorMAGENTA);
    link_custom_->SetPressedColor(SK_ColorGREEN);
    link_disable_ = new views::Link(L"Click me! Oops, I'm disabled!");
    link_disable_->SetEnabled(false);
    link_disable_->set_listener(this);
    link_disable_custom_ = new views::Link(L"Click me! Oops, I'm disabled!");
    link_disable_custom_->SetEnabled(false);
    link_disable_custom_->SetDisabledColor(SK_ColorGRAY);
    link_disable_custom_->set_listener(this);

    separator_ = new views::Separator();

    label_ = new views::Label(L"I'm a Label!");
    label_->SetBackgroundColor(SkColorSetRGB(142, 233, 233));
    label_->SetFont(ui::ResourceBundle::GetSharedInstance().GetFont(
        ui::ResourceBundle::LargeFont));
    label_align_left_ = new views::Label(L"I'm a Label!");
    label_align_left_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
    label_align_right_ = new views::Label(L"I'm a Label!");
    label_align_right_->SetHorizontalAlignment(views::Label::ALIGN_RIGHT);
    label_multi_line_ = new views::Label(L"I'm a Label! \nTihs's the second line.");
    label_multi_line_->SetMultiLine(true);
    label_multi_line_->SizeToFit(label_multi_line_->width());
    label_multi_line_->SetTooltipText(L"¶àÐÐ±êÇ©");

    container->SetLayoutManager(new views::BoxLayout(
        views::BoxLayout::kVertical, 5, 5, 5));
    container->AddChildView(link_);
    container->AddChildView(link_custom_);
    container->AddChildView(link_disable_);
    container->AddChildView(link_disable_custom_);

    container->AddChildView(separator_);

    container->AddChildView(label_);
    container->AddChildView(label_align_left_);
    container->AddChildView(label_align_right_);
    container->AddChildView(label_multi_line_);
}

void DemoLabel::LinkClicked(views::Link* source, int event_flags)
{
   

  
}