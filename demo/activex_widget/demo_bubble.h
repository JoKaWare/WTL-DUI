
#ifndef __demo_bubble_h__
#define __demo_bubble_h__

#pragma once

#include "ui/views/controls/button/button.h"
#include "ui/views/view.h"
#include "ui/views/bubble/bubble_delegate.h"
#include "ui/views/bubble/bubble_border.h"

namespace views
{
	class BubbleBorder;
    class TextButton;
	class View;
}

class Bubble;

class DemoBubble : public views::View, public views::ButtonListener
{
public:
    explicit DemoBubble( );
    virtual ~DemoBubble();

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

    // Overridden from views::ButtonListener:
    virtual void ButtonPressed(views::Button* sender,
        const views::Event& event);

private:
    views::TextButton* button1_;
    views::TextButton* button2_;
    views::TextButton* button3_;
    views::TextButton* button4_;
    views::TextButton* button5_;

    DISALLOW_COPY_AND_ASSIGN(DemoBubble);
};

#endif //__demo_bubble_h__