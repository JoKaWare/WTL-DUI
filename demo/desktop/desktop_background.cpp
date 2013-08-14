
#include "desktop_background.h"

#include "ui/gfx/canvas_skia_paint.h"
#include "ui/gfx/path.h"

#include "ui/views/view.h"

namespace views
{
    namespace desktop
    {

        DesktopBackground::DesktopBackground() {}

        DesktopBackground::~DesktopBackground() {}

        void DesktopBackground::Paint(gfx::Canvas* canvas, View* view) const
        {
            // Paint the sky.
			gfx::Rect rc( 0, 0, view->width(), view->height() );
            canvas->FillRect( rc,SK_ColorCYAN );

            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::kFill_Style);

            // Paint the rolling fields of green.
            {
                gfx::Path path;
                path.moveTo(0, view->height()/2);
                path.cubicTo(view->height()/4, view->height()/4,
                    view->height()/2, view->height()/2,
                    SkIntToScalar(view->width()), view->height()/2);
                path.lineTo(SkIntToScalar(view->width()),
                    SkIntToScalar(view->height()));
                path.lineTo(0, SkIntToScalar(view->height()));
                path.close();

                paint.setColor(SK_ColorGREEN);
                canvas->DrawPath(path, paint);
            }

            // Paint the shining sun.
            {
                gfx::Path path;
                path.addCircle(view->height()/4, view->height()/8,
                    view->height()/16);
                path.close();

                paint.setColor(SK_ColorYELLOW);
                canvas->DrawPath(path, paint);
            }
        }

    } //namespace desktop
} //namespace view