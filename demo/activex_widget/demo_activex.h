
#ifndef __demo_activex_h__
#define __demo_activex_h__

#pragma once

#include "ui/views/controls/button/button.h"

 

namespace views
{
    class FlashView;
    class MediaPlayerView;
    class NativeTextButton;
}

class DemoActiveX : public views::View, public views::ButtonListener
{
public:
    explicit DemoActiveX( );
    virtual ~DemoActiveX();

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
    views::FlashView* flash_view_;
    views::MediaPlayerView* media_player_;
    views::NativeTextButton* play_flash_;
    views::NativeTextButton* play_wmv_;
    views::NativeTextButton* create_flash_popup_;

    DISALLOW_COPY_AND_ASSIGN(DemoActiveX);
};

#endif //__demo_activex_h__