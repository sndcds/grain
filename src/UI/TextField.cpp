#include <grain/UI/TextField.hpp>
#include "grain/String/Character.hpp"
#include "grain/Graphics/TextLayout.hpp"

namespace Grain {

    TextField::TextField(View* view) : Component(view) {
        can_get_focus_ = true;
        text_alignment_ = Alignment::Left;
        editable_ = true;
        number_mode_ = false;

        // setMouseCursor(App::MouseCursor::IBeam); TODO: !!!!
    }

    TextField::~TextField() {
    }



    void TextField::draw(GraphicContext& gc) {
        static Font* font;
        if (font == nullptr) {
            font = new Grain::Font("SFPro-Regular", 15.0f);
        }

        _checkSelectionAndCursor();

        content_rect_ = bounds();

        const double padding_left = 10.0;
        const double padding_right = 10.0;

        const Color background_color = {1, 1, 1, 1};
        Color text_color = {0, 0, 0, 1};
        const Color selection_color = {1, 0, 0, 1};
        const Color selection_background_color = {1, 0, 0, 0.2};

        const bool enabled = enabled_;
        const bool focused = focused_;

        if (!enabled) {
            // text_color.setBlend(background_color, 0.5f);
        }

        gc.setFillColor(background_color);
        gc.fillRect(bounds_);

        const int32_t text_length = textLength();

        if (text_length > 0) {
            auto layout = gc.createTextLayout(text_, font);

            if (layout) {
                const double content_width = content_rect_.width;
                const double text_width = layout->width();

                text_min_x_offset_ = static_cast<float>(std::min(
                    0.0,
                    content_width - text_width
                ));

                text_max_x_offset_ =
                    text_alignment_ == Alignment::Right
                        ? std::max(0.0, content_width - text_width)
                        : 0.0;

                text_x_offset_ = std::clamp(
                    text_x_offset_,
                    text_min_x_offset_,
                    text_max_x_offset_
                );

                const double text_y =
                    bounds_.centerY() -
                    (layout->ascent() - layout->descent()) / 2.0;

                const double cursor_offset =
                    layout->offsetForIndex(cursor_index_);

                double text_x =
                    content_rect_.x + text_x_offset_;

                double cursor_x = text_x + cursor_offset;

                if (cursor_must_be_visible_) {
                    float adjustment = 0.0f;

                    if (cursor_x < content_rect_.x + padding_left) {
                        adjustment =
                            content_rect_.x + padding_left - cursor_x;
                    } else if (
                        cursor_x > content_rect_.right() - padding_right
                    ) {
                        adjustment =
                            content_rect_.right() - padding_right - cursor_x;
                    }

                    text_x_offset_ = std::clamp(
                        text_x_offset_ + adjustment,
                        text_min_x_offset_,
                        text_max_x_offset_
                    );

                    text_x = content_rect_.x + text_x_offset_;
                    cursor_x = text_x + cursor_offset;
                }

                // Keep the cursor and text offset consistent after clamping.
                text_x_offset_ = std::clamp(
                    text_x_offset_,
                    text_min_x_offset_,
                    text_max_x_offset_
                );

                text_x = content_rect_.x + text_x_offset_;
                cursor_x = text_x + cursor_offset;

                // Draw selection background.
                if (
                    selection_begin_ >= 0 &&
                    selection_end_ > selection_begin_
                ) {
                    const double offset1 =
                        layout->offsetForIndex(selection_begin_);

                    const double offset2 =
                        layout->offsetForIndex(selection_end_);

                    const double x1 = std::clamp(
                        text_x + std::min(offset1, offset2),
                        content_rect_.x,
                        content_rect_.right()
                    );

                    const double x2 = std::clamp(
                        text_x + std::max(offset1, offset2),
                        content_rect_.x,
                        content_rect_.right()
                    );

                    if (x2 > x1) {
                        Rectd selection_rect = content_rect_;
                        selection_rect.x = x1;
                        selection_rect.width = x2 - x1;

                        gc.setFillColor(selection_background_color);
                        gc.fillRect(selection_rect);
                    }
                }

                // Draw text clipped to the content rectangle.
                gc.save();
                gc.clip(content_rect_);

                gc.drawTextLayout(
                    *layout,
                    {text_x, text_y},
                    text_color
                );

                // Redraw the selected range using the selection text color.
                if (
                    focused &&
                    selection_begin_ >= 0 &&
                    selection_end_ > selection_begin_
                ) {
                    gc.drawTextLayoutRange(
                        *layout,
                        {text_x, text_y},
                        selection_begin_,
                        selection_end_,
                        selection_color
                    );
                }

                gc.restore();

                // Draw cursor after text and selection.
                if (enabled && focused) {
                    drawCursor(gc, static_cast<float>(cursor_x));
                }
            }
        } else {
            if (enabled && focused) {
                const double x =
                    text_alignment_ == Alignment::Right
                        ? content_rect_.right() - padding_right
                        : content_rect_.x + padding_left;

                drawCursor(gc, static_cast<float>(x));
            }
        }

        // Draw placeholder when the field is empty.
        if (text_length < 1) {
            const Alignment alignment =
                text_alignment_ == Alignment::Right
                    ? Alignment::Right
                    : Alignment::Left;

            Color info_color = {0, 1, 0, 1};

            gc.drawTextInRect(
                info_text_,
                bounds_,
                alignment,
                font,
                info_color
            );
        }

        if (focused) {
            gc.setFillColor({1, 0, 0, 1});
            // gc.fillFrame(boundsRect(), 2);
        }
    }


    void TextField::drawCursor(GraphicContext& gc, float x) const {
        Rectd rect = bounds_;
        rect.x = x - 1.0;
        rect.width = 2.0;

        const Color color = {0, 0, 1, 1};

        gc.setFillColor(color);
        gc.fillRect(rect);
    }


    void TextField::setText(const char* text) {
        text_ = text;
        invalidate();
    }


    int32_t TextField::textLength() {
        return static_cast<int32_t>(text_.length());
    }


    void TextField::setEnabled(bool enabled) {
        if (enabled_ == enabled) {
            return;
        }

        enabled_ = enabled;
        cursor_index_ = 0;
        invalidate();
    }


    void TextField::setNumberMode(bool mode) {
        if (mode == number_mode_) {
            return;
        }

        number_mode_ = mode;
        if (number_mode_) {
            value_ = 0;
            text_alignment_ = Alignment::Right;
            invalidate();
        }
    }


    void TextField::stepNumber(bool use_big_step, bool negative) {
        FixValue step = use_big_step ? big_step_ : step_;

        if (negative) {
            step =- step;
        }

        bool value_changed = false;

        FixValue new_value = value_;
        new_value += step;

        std::string step_string = step_.toString(FixValue::kDecimalPrecision);

        if (step_flip_mode_) {
            new_value.flip(min_, max_);
        }
        else {
            new_value.clamp(min_, max_);
        }

        value_changed = new_value != value_;
        setValue(new_value);
        setCursorToEnd();

        // TODO: implement!
        // Update receiver (if one is connected) and fire action
        if (value_changed) {
            // fireAction(ActionType::None, nullptr);
        }
    }


    bool TextField::setSelection(int32_t begin, int32_t end) {
        bool changed = false;
        int32_t max = textLength();

        if (end < begin) {
            std::swap(begin, end);
        }

        begin = std::clamp<int32_t>(begin, 0, max);
        end = std::clamp<int32_t>(end, 0, max);

        if (begin != selection_begin_ || end != selection_end_) {
            selection_begin_ = begin;
            selection_end_ = end;
            changed = true;
        }

        if (cursor_index_ != end) {
            cursor_index_ = end;
            changed = true;
        }

        if (changed) {
            _checkSelectionAndCursor();
            invalidate();
        }

        return changed;
    }


    bool TextField::setCursor(int32_t cursor_index, bool selection_mode) {
        if (selection_mode) {
            setSelection(selection_drag_start_, cursor_index);
        }
        else {
            removeSelection();
        }

        cursor_index = std::clamp<int32_t>(cursor_index, 0, textLength());
        if (cursor_index != cursor_index_) {
            cursor_index_ = cursor_index;
            invalidate();
            return true;
        }

        return false;
    }


    bool TextField::moveCursor(int32_t offset, bool shift_pressed) {
        int32_t new_cursor_index = std::clamp<int32_t>(cursor_index_ + offset, 0, textLength());

        if (new_cursor_index != cursor_index_) {
            if (shift_pressed) {
                if (selection_drag_start_ < 0) {
                    selection_drag_start_ = cursor_index_;
                }
            }
            setCursor(new_cursor_index, shift_pressed);
        }
        else if (!shift_pressed) {
            removeSelection();
        }

        return false;
    }


    bool TextField::selectWordAtCursor() {
        bool result = false;

        if (text_ != nullptr) {
            int32_t n = textLength();
            int32_t begin = 0;
            int32_t end = n;
            char c;

            for (int32_t i = cursor_index_ - 1; i >= 0; i--) {
                if (text_.isAsciiAtIndex(i, c)) {
                    if (c == ' ' || c == '\t') {
                        begin = i + 1;
                        break;
                    }
                }
            }

            for (int32_t i = cursor_index_; i < n; i++) {
                if (text_.isAsciiAtIndex(i, c)) {
                    if (c == ' ' || c == '\t') {
                        end = i;
                        break;
                    }
                }
            }

            if (begin != selection_begin_ || end != selection_end_) {
                selection_begin_ = begin;
                selection_end_ = end;
                cursor_index_ = end;
                invalidate();
                return true;
            }
        }

        return result;
    }


    bool TextField::removeSelection() {
        if (selection_begin_ >= 0) {
            selection_begin_ = -1;
            selection_end_ = -1;
            selection_drag_start_ = -1;
            invalidate();
            return true;
        }
        else {
            return false;
        }
    }


    bool TextField::removeCharAheadOfCursor() {
        if (editable_ && cursor_index_ > 0) {
            text_.erase(cursor_index_ - 1, 1);
            cursor_index_--;

            invalidate();
            // textChangedAction(); TODO: Implement!

            return true;
        }
        else {
            return false;
        }
    }


    bool TextField::removeSelectedText() {
        if (editable_ && hasSelection()) {
            text_.erase(selection_begin_, selection_end_ - selection_begin_);
            setCursor(selection_begin_);
            selection_begin_ = -1;
            selection_drag_start_ = -1;

            invalidate();
            // textChangedAction(); TODO: Implement!

            return true;
        }
        else {
            return false;
        }
    }


    void TextField::insertText(const char* text) {
        if (editable_ && text && strlen(text) > 0) {
            removeSelectedText();

            int32_t lengthBeforeInsert = textLength();

            text_.insertAtCharacterIndex(text, cursor_index_);

            int32_t lengthAfterInsert = textLength();
            cursor_index_ += (lengthAfterInsert - lengthBeforeInsert);

            invalidate();
            // textChangedAction(); TODO: Implement!
        }
    }


    void TextField::updateEdit() {
        bool value_changed = false;

        if (number_mode_) {
            FixValue new_value(text_.utf8());
            new_value.setPrecision(fractional_digits_);
            value_changed = new_value != value_;
            value_.set(new_value, min_, max_, fractional_digits_);

            if (value_changed) {
                /* TODO: Implement!
                transmit();
                fireAction(ActionType::None, nullptr);
                */
            }
        }
        else {
            /* TODO: Implement!
            transmit();
            fireAction(ActionType::None, nullptr);
            */
        }
    }


    bool TextField::handleEvent(const Event& event) {
        cursor_must_be_visible_ = true;

        if (event.type_ == EventType::MouseButtonDown) {
            if (event.mouse_click_count_ == 2) {
                selectWordAtCursor();
            }
            else {
                int32_t new_cursor_index = cursorIndexAtPos(event.mouse_pos_);

                if (event.hasKeyModifierOnly(KeyModifiers::Shift)) {
                    if (hasSelection()) {
                        if (new_cursor_index < selection_begin_) {
                            selection_drag_start_ = selection_end_;
                        }
                        else if (new_cursor_index > selection_end_) {
                            selection_drag_start_ = selection_begin_;
                        }
                        else if (std::abs(new_cursor_index - selection_begin_) < std::abs(new_cursor_index - selection_end_)) {
                            selection_drag_start_ = selection_end_;
                        }
                        else {
                            selection_drag_start_ = selection_begin_;
                        }
                    }
                    else {
                        selection_drag_start_ = cursor_index_;
                    }
                }
                else {
                    selection_drag_start_ = new_cursor_index;
                }

                setCursor(new_cursor_index, selection_drag_start_);
            }

            return true;
        }

        if (event.type_ == EventType::KeyDown) {
            return _handleKeyDown(event);
        }


        return false;
    }

/*
    void TextField::handleMouseDrag(const Event& event) {
        cursor_must_be_visible_ = true;
        int32_t new_cursor_index = cursorIndexAtPos(event.mousePos());

        setCursor(new_cursor_index, true);
    }


    void TextField::handleMouseUp(const Event& event) {
        cursor_must_be_visible_ = true;

        if (isDelayed()) {
            fireAction(ActionType::None, nullptr);
        }

        if (hit(event) && isToggleMode()) {
            toggleSelection();
        }

        deHighlight();
    }


    void TextField::handleScrollWheel(const Event& event) {
        cursor_must_be_visible_ = false;
        text_x_offset_ += event.deltaX() * App::scrollWheelSpeed();

        needsDisplay();
    }

*/


    bool TextField::_handleKeyDown(const Event& event) {
        cursor_must_be_visible_ = true;

        std::cout << event.text_ << '\n';

        int32_t new_cursor_index = -1;

        if (!event.is_repeat_) {
            if (event.hasKeyModifierOnly(KeyModifiers::Command)) {
                _handleCommandShortcut(event.character_);
            }
            else {
                const auto key = static_cast<KeyCode>(event.character_);

                if (!_handleEditingKey(event, key, new_cursor_index)) {
                    _handleTextInput(event);
                }
            }
        }

        _updateCursorAndSelection(event, new_cursor_index);

        return true;
    }


    void TextField::_handleCommandShortcut(uint32_t key_char) {
        switch (key_char) {
        case 'a':
            selectAll();
            break;

        case 'c':
            copyToPasteboard();
            break;

        case 'v':
            pasteFromPasteboard();
            break;

        case 'x':
            if (copyToPasteboard() > 0) {
                removeSelectedText();
            }
            break;

        default:
            break;
        }
    }


    bool TextField::_handleEditingKey(
        const Event& event,
        KeyCode key,
        int32_t& new_cursor_index
    ) {
        switch (key) {
        case KeyCode::Enter:
        case KeyCode::CarriageReturn:
            updateEdit();
            selectAll();
            return true;

        case KeyCode::Backspace:
            if (editable_) {
                if (hasSelection()) {
                    removeSelectedText();
                }
                else {
                    removeCharAheadOfCursor();
                }
            }
            return true;

        case KeyCode::Delete:
            if (editable_) {
                if (hasSelection()) {
                    removeSelectedText();
                }
                else {
                    // removeCharAfterCursor(); // TODO: Implement this method.
                }
            }
            return true;

        case KeyCode::Up:
            if (number_mode_) {
                stepNumber(
                    event.hasKeyModifierOnly(KeyModifiers::Shift),
                    false);
            }
            else {
                new_cursor_index = 0;
            }
            return true;

        case KeyCode::Down:
            if (number_mode_) {
                stepNumber(
                    event.hasKeyModifierOnly(KeyModifiers::Shift),
                    true);
            }
            else {
                new_cursor_index = textLength();
            }
            return true;

        case KeyCode::Left:
            moveCursor(
                -1,
                event.hasKeyModifierOnly(KeyModifiers::Shift));
            return true;

        case KeyCode::Right:
            moveCursor(
                1,
                event.hasKeyModifierOnly(KeyModifiers::Shift));
            return true;

        default:
            return false;
        }
    }


    void TextField::_handleDeleteKey() {
        if (!editable_) {
            return;
        }

        if (hasSelection()) {
            removeSelectedText();
        }
        else {
            removeCharAheadOfCursor();
        }

        // TODO: Implement continuous-update behavior.
    }


    void TextField::_handleTextInput(const Event& event) {
        if (!editable_) {
            return;
        }

        const uint32_t key_char = event.character_;

        if (number_mode_ && !isAsciiNumericLiteralCharacter(key_char)) {
            return;
        }

        // TODO: Implement text input using the platform text-input system.
        // macOS should eventually use NSTextInputClient integration.
    }


    void TextField::_updateCursorAndSelection(
        const Event& event,
        int32_t new_cursor_index
    ) {
        if (new_cursor_index < 0) {
            return;
        }

        const bool shift_pressed =
            event.hasKeyModifierOnly(KeyModifiers::Shift);

        if (shift_pressed) {
            if (selectionDragStart() < 0) {
                setSelectionDragStart(clampedCursorIndex());
            }
        }
        else {
            setSelectionDragStart(-1);
        }

        setCursor(new_cursor_index, shift_pressed);
    }


    void TextField::becomeFirstResponder() {
        selectAll();
        invalidate();
    }


    void TextField::resignFirstResponder() {
        updateEdit();
        cursor_index_ = 0;
        selection_begin_ = selection_end_ = -1;

        invalidate();
    }


    int32_t TextField::cursorIndexAtPos(Vec2d pos) {
        /* TODO: Implement!
        int32_t cursor_index = -1;
        auto style = App::guiStyleAtIndex(style_index_);

        // TODO: Eventually use font->indexForCharAtPos inside direct CoreText functions here.

        if (hasText() && style) {
            _checkSelectionAndCursor();

            Rectd content_rect = content_rect_;

            CFStringRef cf_str = CFStringCreateWithCString(NULL, text_->utf8(), kCFStringEncodingUTF8);
            CFMutableAttributedStringRef cf_attr_str = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
            CFAttributedStringReplaceString(cf_attr_str, CFRangeMake(0, 0), cf_str);

            CFRange range = CFRangeMake(0, text_->length());
            if (range.length > 0) {
                CFAttributedStringSetAttribute(cf_attr_str, range, kCTFontAttributeName, style->font()->ctFont());
            }

            CTLineRef line = CTLineCreateWithAttributedString(cf_attr_str);
            double text_x = content_rect.x_ + text_x_offset_;

            double min_delta = std::numeric_limits<float>::max();
            for (int32_t i = 0; i <= textLength(); i++) {
                double cx = text_x + CTLineGetOffsetForStringIndex(line, i, NULL);
                double delta = std::fabs(pos.x_ - cx);
                if (delta < min_delta) {
                    min_delta = delta;
                    cursor_index = i;
                }
            }

            CFRelease(cf_str);
            CFRelease(cf_attr_str);
            CFRelease(line);
        }
        return cursor_index;
    */
        return 0;
    }


    void TextField::_checkSelectionAndCursor() {
        selection_begin_ = std::clamp<int32_t>(selection_begin_, 0, textLength());
        selection_end_ = std::clamp<int32_t>(selection_end_, selection_begin_, textLength());

        if (selectionLength() > 0) {
            cursor_index_ = std::clamp<int32_t>(cursor_index_, selection_begin_, selection_end_);
        }
    }


    int32_t TextField::copyToPasteboard() {
        int32_t result = 0;

        if (hasSelection()) {
            /* TODO; Implement!
            text_.copyToPasteboard(selection_begin_, selection_end_ - selection_begin_);
            */
            result = selectionLength();
        }

        return result;
    }


    int32_t TextField::pasteFromPasteboard() {
        int32_t result = 0;

        if (hasSelection()) {
            cursor_index_ = selection_begin_;
            text_.erase(selection_begin_, selection_end_ - selection_begin_);
            removeSelection();
        }

        /* TODO: Implement!
        result = static_cast<int32_t>(text_.pasteFromPasteboard(cursor_index_));
        */
        cursor_index_ += result;

        invalidate();
        // textChangedAction(); TODO: Implement!

        return result;
    }

} // End of namespace Grain
