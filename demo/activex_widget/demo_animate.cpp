
#include "demo_animate.h"

#include "ui/base/resource/resource_bundle.h"
#include "grit/ui_resources.h"
#include "ui/views/animation/bounds_animator.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"



class AppView : public views::View
{
public:
    AppView(int res_id, const std::wstring& app_name);
    virtual ~AppView();

    void SaveBounds() { save_bounds_ = bounds(); }
    gfx::Rect GetSaveBounds() const { return save_bounds_; }

private:
    gfx::Rect save_bounds_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(AppView);
};

AppView::AppView(int res_id, const std::wstring& app_name)
{
    views::GridLayout* layout = new views::GridLayout(this);
    SetLayoutManager(layout);

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        0.0f, views::GridLayout::USE_PREF, 0, 0);

    layout->StartRow(0.0f, 0);
    views::ImageView* image = new views::ImageView();
    image->SetImage(
        ui::ResourceBundle::GetSharedInstance().GetBitmapNamed(res_id));
    layout->AddView(image);

    layout->StartRow(0.0f, 0);
    views::Label* label = new views::Label(app_name);
    layout->AddView(label);
}

AppView::~AppView()
{
}

class AppLayout : public views::LayoutManager
{
public:
    AppLayout(int inside_border_horizontal_spacing,
        int inside_border_vertical_spacing,
        int between_child_spacing);

    // Overridden from view::LayoutManager:
    virtual gfx::Size GetPreferredSize(views::View* host)
    {
        return gfx::Size();
    }
    virtual void Layout(views::View* host);

private:
    // Spacing between child views and host view border.
    const int inside_border_horizontal_spacing_;
    const int inside_border_vertical_spacing_;

    // Spacing to put in between child views.
    const int between_child_spacing_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(AppLayout);
};

AppLayout::AppLayout(int inside_border_horizontal_spacing,
                     int inside_border_vertical_spacing,
                     int between_child_spacing)
                     : inside_border_horizontal_spacing_(inside_border_horizontal_spacing),
                     inside_border_vertical_spacing_(inside_border_vertical_spacing),
                     between_child_spacing_(between_child_spacing) {}

void AppLayout::Layout(views::View* host)
{
    gfx::Rect childArea(gfx::Rect(host->size()));
    childArea.Inset(host->GetInsets());
    childArea.Inset(inside_border_horizontal_spacing_,
        inside_border_vertical_spacing_);
    int x = childArea.x();
    int y = childArea.y();
    for(int i=0; i<host->child_count(); ++i)
    {
        views::View* child = host->child_at(i);
        if(child->visible())
        {
            gfx::Size size(child->GetPreferredSize());
            gfx::Rect bounds(x, y, 0, size.height());
            if(x + size.width() <= childArea.right())
            {
                bounds.set_width(size.width());
                x += size.width() + between_child_spacing_;
            }
            else
            {
                x = childArea.x();
                y += size.height() + between_child_spacing_;
                bounds.SetRect(x, y, size.width(), size.height());

                x += size.width() + between_child_spacing_;
            }

            // Clamp child view bounds to |childArea|.
            child->SetBoundsRect(bounds.Intersect(childArea));
        }
    }
}

struct AppInfo 
{
    int icon_res_id;
    std::wstring app_name;
};
namespace
{
    const AppInfo app_infos[] =
    {
        { IDR_CLOSE,  L"app1"  },
        { IDR_CLOSE_H,  L"app2"  },
        { IDR_CLOSE_P,  L"app3"  },
        { IDR_MENU_CHECK,  L"app4"  },
        
    };
}

class AppContainer : public views::View, public views::BoundsAnimatorObserver
{
public:
    AppContainer();
    virtual ~AppContainer();

    void DoAnimate();

    // Overridden from view::View:
    virtual void Layout()
    {
        views::View::Layout();

        for(int i=0; i<child_count(); ++i)
        {
            AppView* child = static_cast<AppView*>(child_at(i));
            child->SaveBounds();
        }
    }

    // Overridden from view::BoundsAnimatorObserver:
    virtual void OnBoundsAnimatorDone(views::BoundsAnimator* animator)
    {
        DCHECK(animator == &bounds_animator_);
        if(show_)
        {
            for(int i=0; i<child_count(); ++i)
            {
                AppView* child = static_cast<AppView*>(child_at(i));
                child->SaveBounds();
            }
        }
    }

private:
    views::BoundsAnimator bounds_animator_;
    bool show_;

    DISALLOW_COPY_AND_ASSIGN(AppContainer);
};

AppContainer::AppContainer() : bounds_animator_(this), show_(true)
{
    bounds_animator_.AddObserver(this);

    SetLayoutManager(new AppLayout(10, 10, 10));
    for(int i=0; i<arraysize(app_infos); ++i)
    {
        AddChildView(new AppView(app_infos[i].icon_res_id,
            app_infos[i].app_name));
    }
}

AppContainer::~AppContainer()
{
    bounds_animator_.Cancel();
}

void AppContainer::DoAnimate()
{
    show_ = !show_;
    for(int i=0; i<child_count(); ++i)
    {
        AppView* child = static_cast<AppView*>(child_at(i));
        if(show_)
        {
            bounds_animator_.AnimateViewTo(child, child->GetSaveBounds());
        }
        else
        {
            gfx::Point target_orign(-child->width(), -child->height());
            gfx::Rect layout_rect = child->bounds();
            gfx::Point app_view_center = layout_rect.CenterPoint();
            gfx::Point view_center = GetLocalBounds().CenterPoint();
            if(app_view_center.x() >= view_center.x() &&
                app_view_center.y() <= view_center.y())
            {
                target_orign.set_x(GetLocalBounds().right());
            }
            else if(app_view_center.x() <= view_center.x() &&
                app_view_center.y() >= view_center.y())
            {
                target_orign.set_y(GetLocalBounds().bottom());
            }
            else if(app_view_center.x() >= view_center.x() &&
                app_view_center.y() >= view_center.y())
            {
                target_orign.set_x(GetLocalBounds().right());
                target_orign.set_y(GetLocalBounds().bottom());
            }
            bounds_animator_.AnimateViewTo(child,
                gfx::Rect(target_orign, child->size()));
        }
    }
}


DemoAnimate::DemoAnimate() : view_created_(false),
apps_container_(NULL), button_animate_(NULL)  {}

DemoAnimate::~DemoAnimate() {}

std::wstring DemoAnimate::GetTitle()
{
    return std::wstring(L"Animate");
}



void DemoAnimate::InitView(views::View* container)
{
    views::GridLayout* layout = new views::GridLayout(container);
    container->SetLayoutManager(layout);

    apps_container_ = new AppContainer();
    button_animate_ = new views::NativeTextButton(this, L"ÑÝÊ¾¶¯»­");

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);

    layout->StartRow(1.0f, 0);
    layout->AddView(apps_container_);

    layout->StartRow(0.0f, 0);
    layout->AddView(button_animate_);
}

void DemoAnimate::ButtonPressed(views::Button* sender,
                                const views::Event& event)
{
    if(button_animate_ == sender)
    {
        (static_cast<AppContainer*>(apps_container_))->DoAnimate();
    }
}