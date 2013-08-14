
#ifndef __demo_animate_h__
#define __demo_animate_h__

#pragma once

#include "ui/views/controls/button/button.h"
#include "ui/views/view.h"

namespace views
{
    class NativeTextButton;
}

class DemoAnimate : public views::View, public views::ButtonListener
{
public:
    explicit DemoAnimate();
    virtual ~DemoAnimate();
	virtual void ViewHierarchyChanged(bool is_add,
		View* parent,
		View* child) OVERRIDE 
	{
		View::ViewHierarchyChanged(is_add, parent, child);
		// We're not using child == this because a Widget may not be
		// available when this is added to the hierarchy.
		if (is_add && GetWidget() && !view_created_) {
			view_created_ = true;
			InitView(this);
		}
	}
	bool view_created_;
    // Overridden from DemoBase:
    virtual std::wstring GetTitle();
    virtual void InitView(views::View* container);
    // Overridden from view::ButtonListener:
    virtual void ButtonPressed(views::Button* sender,
        const views::Event& event);

private:
    views::View* apps_container_;
    views::NativeTextButton* button_animate_;

    DISALLOW_COPY_AND_ASSIGN(DemoAnimate);
};

#endif //__demo_animate_h__