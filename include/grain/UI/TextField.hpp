#pragma once

#include <grain/UI/Component.hpp>
#include <grain/Type/FixValue.hpp>


namespace Grain {

class TextField : public Component {
    FixValue value_ = 0;

protected:
    FixValue min_ = -1000000;
    FixValue max_ = 1000000;
    FixValue step_ = 1;
    FixValue big_step_ = 10;

    bool step_flip_mode_ = false;
    bool pass_word_mode_ = false;
    void* var_ptr_ = nullptr;

    int32_t fractional_digits_ = 2;
    bool value_changed_ = false;

    float beam_width_ = 2.0f;

    int32_t cursor_index_ = 0;
    int32_t selection_begin_ = -1, selection_end_ = -1;
    int32_t selection_drag_start_ = -1;

    float text_x_offset_ = 0;
    float text_min_x_offset_ = 0;
    float text_max_x_offset_ = 0;
    Rectd content_rect_;
    bool cursor_must_be_visible_ = true;

    bool show_loupe_ = false; // TODO: For search fields
    bool show_loupe_if_empty_ = false;

    Alignment text_alignment_ = Alignment::Left;
    std::array<float, 4> padding_ = {};

    String info_text_;
    String text_;

public:
    explicit TextField(View* view);

    ~TextField() override;


    void draw(GraphicContext& gc) override;

    void drawCursor(GraphicContext& gc, float x) const;

    [[nodiscard]] virtual auto beamWidth() const -> float {
        return beam_width_;
    }

    [[nodiscard]] virtual auto cursorIndex() const -> int32_t {
        return cursor_index_;
    }

    [[nodiscard]] virtual auto clampedCursorIndex() const -> int32_t {
        return std::clamp<int32_t>(cursor_index_, 0, text_.length());
    }

    [[nodiscard]] virtual auto selectionBegin() const -> int32_t {
        return selection_begin_;
    }

    [[nodiscard]] virtual auto selectionEnd() const -> int32_t {
        return selection_end_;
    }

    [[nodiscard]] virtual auto selectionLength() const -> int32_t {
        return hasSelection() ? selection_end_ - selection_begin_ + 1 : 0;
    }

    [[nodiscard]] virtual auto selectionDragStart() const -> int32_t {
        return selection_drag_start_;
    }

    [[nodiscard]] virtual auto stepFlipMode() const -> bool {
        return step_flip_mode_;
    }

    [[nodiscard]] virtual auto isPasswordMode() const -> bool {
        return pass_word_mode_;
    }

    void setText(const char* text);
    auto textLength() -> int32_t;

    void setEnabled(bool enabled) override;

    void setNumberMode(bool mode) override;

    void stepNumber(bool use_big_step, bool negative);

    virtual void setBeamWidth(float beam_width) {
        beam_width_ = beam_width;
        invalidate();
    }

    virtual void setCursorIndex(int32_t cursor_index) {
        cursor_index_ = cursor_index;
        invalidate();
    }

    virtual void setSelectionBegin(int32_t selection_begin) {
        selection_begin_ = selection_begin;
        invalidate();;
    }

    virtual void setSelectionEnd(int32_t selection_end) {
        selection_end_ = selection_end;
        invalidate();
    }

    virtual void setSelectionDragStart(int32_t selection_drag_start) {
        selection_drag_start_ = selection_drag_start;
    }

    virtual auto setSelection(int32_t begin, int32_t end) -> bool;

    virtual auto setCursor(int32_t cursor_index) -> bool {
        return setCursor(cursor_index, false);
    }

    virtual auto setCursor(int32_t cursor_index, bool selection_mode) -> bool;

    virtual auto moveCursor(int32_t offset, bool shift_pressed) -> bool;

    virtual auto setCursorToEnd() -> bool {
        return setCursor(textLength());
    }

    virtual auto selectAll() -> bool {
        return setSelection(0, textLength());
    }

    virtual auto selectWordAtCursor() -> bool;

    virtual auto removeSelection() -> bool;

    [[nodiscard]] virtual auto hasSelection() const -> bool {
        return selection_begin_ >= 0 && selection_end_ > selection_begin_;
    }

    virtual auto removeCharAheadOfCursor() -> bool;

    virtual auto removeSelectedText() -> bool;

    void insertText(const char* text);

    virtual void updateEdit();


    static void setReceiverComponent(Component* receiver) {
        (void)receiver;
        /* TODO: Implement!
        receiver_component_ = receiver;
        if (receiver_component_) {
            receiver_component_->setTextField(this);
        }
        */
    }


    void setFractionalDigits(int32_t fractional_digits) {
        fractional_digits_ =
            std::clamp<int32_t>(fractional_digits, 0, FixValue::kDecimalPrecision);
    }

    [[nodiscard]] auto value() const -> FixValue {
        return value_;
    }

    [[nodiscard]] auto valueAsInt32() const -> int32_t {
        return value_.asInt32();
    }

    [[nodiscard]] auto valueAsInt64() const -> int64_t {
        return value_.asInt64();
    }

    [[nodiscard]] auto valueAsDouble() const -> double {
        return value_.asDouble();
    }

    auto setValue(const FixValue& value) -> bool {
        bool result = value_.set(value, min_, max_, fractional_digits_);
        setText(value_.toString().c_str());
        invalidate();
        return result;
    }

    auto setValueInt(int32_t value) -> bool {
        return setValue(FixValue(value));
    }

    void setValueRange(const FixValue& min, const FixValue& max) {
        setNumberMode(true);
        min_ = min;
        max_ = max;
    }

    virtual void setValueRangeInt32(int32_t min, int32_t max) {
        setNumberMode(true);
        min_.setInt64(min);
        max_.setInt64(max);
    }

    virtual void setValueRangeReal64(double min, double max) {
        setNumberMode(true);
        min_.setDouble(min);
        max_.setDouble(max);
    }

    virtual void setStep(FixValue step, FixValue big_step) {
        setNumberMode(true);
        step_ = step;
        big_step_ = big_step;
    }

    virtual void setStepReal64(double step, double big_step) {
        setNumberMode(true);
        step_.setDouble(step);
        big_step_.setDouble(big_step);
    }

    void setStepFlipMode(bool step_flip_mode) {
        step_flip_mode_ = step_flip_mode;
    }

    void setPasswordMode(bool password_mode) {
        pass_word_mode_ = password_mode;
        invalidate();
    }

    void enablePasswordMode() {
        setPasswordMode(true);
    }

    void disablePasswordMode() {
        setPasswordMode(false);
    }


    void setInfoText(const char* text) {
        info_text_ = text;
        invalidate();
    }

    auto handleEvent(const Event& event) -> bool override;
    auto _handleKeyDown(const Event& event) -> bool;
    void _handleCommandShortcut(uint32_t key_char);
    auto _handleEditingKey(
       const Event& event,
       KeyCode key,
       int32_t& new_cursor_index
    ) -> bool;
    void _handleDeleteKey();
    void _handleTextInput(const Event& event);
    void _updateCursorAndSelection(
        const Event& event,
        int32_t new_cursor_index
    );



    void becomeFirstResponder();

    void resignFirstResponder();

    auto cursorIndexAtPos(Vec2d pos) -> int32_t;

    void _checkSelectionAndCursor();

    auto copyToPasteboard() -> int32_t;

    auto pasteFromPasteboard() -> int32_t;
};

} // End of namespace Grain
