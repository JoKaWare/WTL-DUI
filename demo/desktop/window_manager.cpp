
#include "window_manager.h"

#include "ui/views/events/event.h"
#include "ui/views/widget/widget.h"

namespace
{

    views::WindowManager* window_manager = NULL;

    class NullWindowManager : public views::WindowManager
    {
    public:
        NullWindowManager() : mouse_capture_(NULL) {}

        virtual void StartMoveDrag(views::Widget* widget,
            const gfx::Point& screen_point)
        {
            NOTIMPLEMENTED();
        }

        virtual void StartResizeDrag(views::Widget* widget,
            const gfx::Point& screen_point,
            int hittest_code)
        {
            NOTIMPLEMENTED();
        }

        virtual bool SetMouseCapture(views::Widget* widget)
        {
            if(mouse_capture_ == widget)
            {
                return true;
            }
            if(mouse_capture_)
            {
                return false;
            }
            mouse_capture_ = widget;
            return true;
        }

        virtual bool ReleaseMouseCapture(views::Widget* widget)
        {
            if(widget && mouse_capture_!=widget)
            {
                return false;
            }
            mouse_capture_ = NULL;
            return true;
        }

        virtual bool HasMouseCapture(const views::Widget* widget) const
        {
            return mouse_capture_ == widget;
        }

        virtual bool HandleKeyEvent(views::Widget* widget,
            const views::KeyEvent& event)
        {
            return false;
        }

        virtual bool HandleMouseEvent(views::Widget* widget,
            const views::MouseEvent& event)
        {
            if(mouse_capture_)
            {
                views::MouseEvent translated(event, widget->GetRootView(),
                    mouse_capture_->GetRootView());
                mouse_capture_->OnMouseEvent(translated);
                return true;
            }
            return false;
        }

        void Register(views::Widget* widget) {}

    private:
        views::Widget* mouse_capture_;
    };

}

namespace views
{

    WindowManager::WindowManager() {}

    WindowManager::~WindowManager() {}

    // static
    void WindowManager::Install(WindowManager* wm)
    {
        window_manager = wm;
    }

    // static
    WindowManager* WindowManager::Get()
    {
        if(!window_manager)
        {
            window_manager = new NullWindowManager();
        }
        return window_manager;
    }

} //namespace view