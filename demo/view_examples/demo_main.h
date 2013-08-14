
#ifndef __demo_main_h__
#define __demo_main_h__

#pragma once

#include "ui/views/controls/tabbed_pane/tabbed_pane_listener.h"
#include "ui/views/widget/widget_delegate.h"

namespace views
{
    class Label;
    class View;
}

class DemoMain : public views::WidgetDelegate, views::TabbedPaneListener
{
public:
    DemoMain();
    virtual ~DemoMain();

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
    void Run();

private:
    views::View* contents_;

    views::Label* status_label_;

    DISALLOW_COPY_AND_ASSIGN(DemoMain);
};

#endif //__demo_main_h__