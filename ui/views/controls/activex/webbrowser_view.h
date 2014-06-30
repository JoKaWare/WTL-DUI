
#ifndef __view_webbrowser_view_h__
#define __view_webbrowser_view_h__


#include <atlbase.h>
#include <atlcom.h>
#include <exdisp.h>
#include "ui/views/controls/activex/activex_host.h"
#include "ui/views/view.h"

namespace views
{

    class WebBrowserView : public View, public AxHostDelegate
    {
    public:
        explicit WebBrowserView();
        virtual ~WebBrowserView();

        IWebBrowser2* webbrowser() const { return webbrowser_; }
        bool Navigate(const std::wstring& url);

    public:
        // Overridden from View:
        virtual void Layout();
        virtual void ViewHierarchyChanged(bool is_add, View* parent, View* child);
        virtual void VisibilityChanged(View* starting_from, bool is_visible);

        // Overridden from AxHostDelegate:
        virtual HWND GetAxHostWindow() const;
        virtual void OnAxCreate(AxHost* host);
        virtual void OnAxInvalidate(const gfx::Rect& rect);

    private:
        scoped_ptr<AxHost> ax_host_;
        base::win::ScopedComPtr<IWebBrowser2> webbrowser_;

        bool initialized_;
    };

} //namespace view

#endif //__view_webbrowser_view_h__