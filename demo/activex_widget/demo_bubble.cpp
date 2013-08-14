
#include "demo_bubble.h"

#include "ui/views/view.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "base/utf_string_conversions.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"

#include "demo_main.h"

namespace views {

struct BubbleConfig {
	string16 label;
	SkColor color;
	views::View* anchor_view;
	views::BubbleBorder::ArrowLocation arrow;
	bool fade_in;
	bool fade_out;
};

// Create four types of bubbles, one without arrow, one with an arrow, one
// that fades in, and another that fades out and won't close on the escape key.
BubbleConfig kRoundConfig = { ASCIIToUTF16("Round"), SK_ColorWHITE, NULL,
BubbleBorder::NONE, false, false };
BubbleConfig kArrowConfig = { ASCIIToUTF16("Arrow"), SK_ColorGRAY, NULL,
BubbleBorder::TOP_LEFT, false, false };
BubbleConfig kFadeInConfig = { ASCIIToUTF16("FadeIn"), SK_ColorYELLOW, NULL,
BubbleBorder::BOTTOM_RIGHT, true, false };
BubbleConfig kFadeOutConfig = { ASCIIToUTF16("FadeOut"), SK_ColorWHITE, NULL,
BubbleBorder::LEFT_TOP, false, true };


class ExampleBubbleDelegateView : public BubbleDelegateView {
public:
	explicit ExampleBubbleDelegateView(const BubbleConfig& config)
		: BubbleDelegateView(config.anchor_view, config.arrow),
		label_(config.label) {
			set_color(config.color);
	}

protected:
	virtual void Init() OVERRIDE {
		SetLayoutManager(new FillLayout());
		Label* label = new Label(label_);
		AddChildView(label);
	}

private:
	string16 label_;
};

}

DemoBubble::DemoBubble( ) : 
button1_(NULL), button2_(NULL), button3_(NULL),view_created_(false),
button4_(NULL), button5_(NULL) {}

DemoBubble::~DemoBubble() {}

std::wstring DemoBubble::GetTitle()
{
    return std::wstring(L"Bubble");
}

void DemoBubble::InitView(views::View* container)
{
    views::GridLayout* layout = new views::GridLayout(container);
    container->SetLayoutManager(layout);

    button1_ = new views::TextButton(this, L"显示Bubble");
    button2_ = new views::TextButton(this, L"显示Bubble");
    button3_ = new views::TextButton(this, L"显示Bubble");
    button4_ = new views::TextButton(this, L"显示Bubble");
    button5_ = new views::TextButton(this, L"显示Bubble");

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);
    views::ColumnSet* column_set1 = layout->AddColumnSet(1);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        0.0f, views::GridLayout::USE_PREF, 0, 0);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        0.0f, views::GridLayout::USE_PREF, 0, 0);
    
    layout->StartRow(0.0f, 0);
    layout->AddView(button1_);

    layout->StartRow(1.0f, 1);
    layout->AddView(button2_);
    layout->AddView(button3_);
    layout->AddView(button4_);

    layout->StartRow(0.0f, 0);
    layout->AddView(button5_);
}

void DemoBubble::ButtonPressed(views::Button* sender,
                               const views::Event& event)
{  
	views::BubbleConfig config;
	config = views::kRoundConfig;
	config.anchor_view = sender;
	config.label = L"这是一个绚丽的弹出式气泡窗口";
	config.arrow = views::BubbleBorder::FLOAT;

	views::ExampleBubbleDelegateView* bubble_delegate =
		new views::ExampleBubbleDelegateView(config);
	views::BubbleDelegateView::CreateBubble(bubble_delegate);
	bubble_delegate->Show();
}

