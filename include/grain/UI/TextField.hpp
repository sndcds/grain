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
    float padding_[4]{};

    String info_text_;
    String text_;

public:
    explicit TextField(View* view);

    ~TextField() override;


    void draw(GraphicContext& gc) override;

    void drawCursor(GraphicContext& gc, float x) const;

    [[nodiscard]] virtual float beamWidth() const {
        return beam_width_;
    }

    [[nodiscard]] virtual int32_t cursorIndex() const {
        return cursor_index_;
    }

    [[nodiscard]] virtual int32_t clampedCursorIndex() const {
        return std::clamp<int32_t>(cursor_index_, 0, text_.length());
    }

    [[nodiscard]] virtual int32_t selectionBegin() const {
        return selection_begin_;
    }

    [[nodiscard]] virtual int32_t selectionEnd() const {
        return selection_end_;
    }

    [[nodiscard]] virtual int32_t selectionLength() const {
        return hasSelection() ? selection_end_ - selection_begin_ + 1 : 0;
    }

    [[nodiscard]] virtual int32_t selectionDragStart() const {
        return selection_drag_start_;
    }

    [[nodiscard]] virtual bool stepFlipMode() const {
        return step_flip_mode_;
    }

    [[nodiscard]] virtual bool isPasswordMode() const {
        return pass_word_mode_;
    }

    void setText(const char* text);
    int32_t textLength();

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

    virtual bool setSelection(int32_t begin, int32_t end);

    virtual bool setCursor(int32_t cursor_index) {
        return setCursor(cursor_index, false);
    }

    virtual bool setCursor(int32_t cursor_index, bool selection_mode);

    virtual bool moveCursor(int32_t offset, bool shift_pressed);

    virtual bool setCursorToEnd() {
        return setCursor(textLength());
    }

    virtual bool selectAll() {
        return setSelection(0, textLength());
    }

    virtual bool selectWordAtCursor();

    virtual bool removeSelection();

    [[nodiscard]] virtual bool hasSelection() const {
        return selection_begin_ >= 0 && selection_end_ > selection_begin_;
    }

    virtual bool removeCharAheadOfCursor();

    virtual bool removeSelectedText();

    void insertText(const char* text);

    virtual void updateEdit();


    void setReceiverComponent(Component* receiver) {
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

    FixValue value() const {
        return value_;
    }

    int32_t valueAsInt32() const {
        return value_.asInt32();
    }

    int32_t valueAsInt64() const {
        return value_.asInt32();
    }

    double valueAsDouble() const {
        return value_.asDouble();
    }

    bool setValue(const FixValue& value) {
        bool result = value_.set(value, min_, max_, fractional_digits_);
        setText(value_.toString().c_str());
        invalidate();
        return result;
    }

    bool setValueInt(int32_t value) {
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

    bool handleEvent(const Event& event) override;
    bool _handleKeyDown(const Event& event);
    void _handleCommandShortcut(uint32_t key_char);
    bool _handleEditingKey(
       const Event& event,
       KeyCode key,
       int32_t& new_cursor_index
    );
    void _handleDeleteKey();
    void _handleTextInput(const Event& event);
    void _updateCursorAndSelection(
        const Event& event,
        int32_t new_cursor_index
    );



    void becomeFirstResponder();

    void resignFirstResponder();

    int32_t cursorIndexAtPos(Vec2d pos);

    void _checkSelectionAndCursor();

    int32_t copyToPasteboard();

    int32_t pasteFromPasteboard();
};

} // End of namespace Grain
