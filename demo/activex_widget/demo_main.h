
#ifndef __demo_main_h__
#define __demo_main_h__

#pragma once

#include "ui/views/controls/tabbed_pane/tabbed_pane_listener.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/controls/button/text_button.h"

namespace views
{
    class Label;
    class View;
}

class DemoMain : public views::WidgetDelegate, views::TabbedPaneListener, views::ButtonListener
{
public:
    DemoMain();
    virtual ~DemoMain();
	static DemoMain* instance() { return instance_; }
    // Overridden from views::WidgetDelegate:
    virtual bool CanResize() const;
    virtual std::wstring GetWindowTitle() const;
    virtual views::View* GetContentsView();
    virtual void WindowClosing();
    virtual views::Widget* GetWidget();
    virtual const views::Widget* GetWidget() const;

    // Overridden from views::TabbedPaneListener:
    virtual void TabSelectedAt(int index);

    void SetStatus(const std::wstring& status);
    void InitWidget();
	void DisplayBalloon()
	{

	}
	virtual void ButtonPressed( views::Button* sender, const views::Event& event)
	{

	}

private:
    views::View* contents_;

    views::Label* status_label_;

	static DemoMain* instance_;

    DISALLOW_COPY_AND_ASSIGN(DemoMain);
};

#endif //__demo_main_h__