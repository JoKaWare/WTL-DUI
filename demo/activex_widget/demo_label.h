
#ifndef __demo_label_h__
#define __demo_label_h__

#pragma once


#include "ui/views/view.h"
#include "ui/views/widget/widget.h"
#include "ui/views/controls/link_listener.h"


namespace views
{
	class View;
	class Widget;
    class Label;
    class Separator;
}

class DemoLabel : public views::View, public views::LinkListener
{
public:
    explicit DemoLabel(  );
    virtual ~DemoLabel();

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

    // Overridden from views::LinkListener:
    virtual void LinkClicked(views::Link* source, int event_flags);

private:
    views::Link* link_;
    views::Link* link_custom_;
    views::Link* link_disable_;
    views::Link* link_disable_custom_;

    views::Separator* separator_;

    views::Label* label_;
    views::Label* label_align_left_;
    views::Label* label_align_right_;
    views::Label* label_multi_line_;

    DISALLOW_COPY_AND_ASSIGN(DemoLabel);
};

#endif //__demo_label_h__