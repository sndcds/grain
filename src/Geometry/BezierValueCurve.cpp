#include "Grain.hpp"
#include <algorithm>
#include <grain/Geometry/BezierValueCurve.hpp>
#include <grain/Geometry/Line.hpp>
#include <grain/Math/Math.hpp>

#include <new> // Include this header for placement new

namespace Grain {

    auto BezierValueCurvePoint::setPointType(PointType point_type) -> bool {
        bool changed = false;

        if (point_type != point_type_) {
            changed = true;

            if (point_type == PointType::Linear) {
            }
            else if (point_type == PointType::Corner) {
                if (point_type_ == PointType::Linear ||
                    point_type_ == PointType::Right ||
                    point_type_ == PointType::Left) {
                    left_.set(-1, 0);
                    right_.set(1, 0);
                }
            }
            else if (point_type == PointType::Smooth1) {
                if (point_type_ == PointType::Linear ||
                    point_type_ == PointType::Corner ||
                    point_type_ == PointType::Right ||
                    point_type_ == PointType::Left) {
                    left_.set(-1, 0);
                    right_.set(1, 0);
                }
            }
            else if (point_type == PointType::Smooth2) {
                if (point_type_ == PointType::Linear ||
                    point_type_ == PointType::Corner ||
                    point_type_ == PointType::Right ||
                    point_type_ == PointType::Left) {
                    left_.set(-1, 0);
                    right_.set(1, 0);
                }
                else if (point_type_ == PointType::Smooth1) {
                    left_ = (left_ - right_) * 0.5;
                    right_ = -left_;
                }
            }
            else if (point_type == PointType::Right) {
            }
            else if (point_type == PointType::Left) {
            }

            point_type_ = point_type;
        }

        if (changed) {
            bezier_value_curve_->mustUpdate();
        }

        return changed;
    }


    auto BezierValueCurvePoint::leftDistance() const -> double {
        return hasLeftControl() ? (left_pos_ - pos_).length() : 0;
    }


    auto BezierValueCurvePoint::rightDistance() const -> double {
        return hasRightControl() ? (right_ - pos_).length() : 0;
    }

    auto BezierValueCurvePoint::hasVisibleControlPoints() const -> bool {
        return
                point_type_ == PointType::Corner ||
                point_type_ == PointType::Smooth1 ||
                point_type_ == PointType::Smooth2;
    }


    auto BezierValueCurvePoint::hasLeftControl() const -> bool {
        return
            point_type_ == PointType::Corner ||
            point_type_ == PointType::Smooth1 ||
            point_type_ == PointType::Smooth2;
    }


    auto BezierValueCurvePoint::hasRightControl() const -> bool {
        return
            point_type_ == PointType::Corner ||
            point_type_ == PointType::Smooth1 ||
            point_type_ == PointType::Smooth2;
    }


    auto BezierValueCurvePoint::usesLeftControl() const -> bool {
        return
            point_type_ == PointType::Corner ||
            point_type_ == PointType::Smooth1 ||
            point_type_ == PointType::Smooth2 ||
            point_type_ == PointType::Left;
    }


    auto BezierValueCurvePoint::usesRightControl() const -> bool {
        return
            point_type_ == PointType::Corner ||
            point_type_ == PointType::Smooth1 ||
            point_type_ == PointType::Smooth2 ||
            point_type_ == PointType::Right;
    }


    auto BezierValueCurvePoint::isSmooth() const -> bool {
        return
            point_type_ == PointType::Smooth1 ||
            point_type_ == PointType::Smooth2;
    }


    auto BezierValueCurvePoint::changeStatus(Status mask, bool flag) -> bool {
        Status old_status = status_;
        status_ = static_cast<Status>(flag ? status_ | mask : status_ & ~mask);
        return status_ != old_status;
    }


    void BezierValueCurvePoint::invertSelection() {
        status_ = static_cast<Status>(((status_ & Status::Selected) != 0) ? status_ & ~Status::Selected : status_ | Status::Selected);
    }


    auto BezierValueCurvePoint::setFixed(bool h_flag, bool v_flag) -> bool {
        bool changed = false;
        changed |= changeStatus(Status::FixedX, h_flag);
        changed |= changeStatus(Status::FixedY, v_flag);
        return changed;
    }


    void BezierValueCurvePoint::setPos(const Vec2d& pos) {
        pos_ = pos;
        curveMustUpdate();
    }


    void BezierValueCurvePoint::setX(double x) {
        pos_.x = x; curveMustUpdate();
    }


    void BezierValueCurvePoint::setY(double y) {
        pos_.y = y; curveMustUpdate();
    }


    void BezierValueCurvePoint::clampX(const double min, const double max) {
        pos_.x = std::clamp(pos_.x, -max, max);
        curveMustUpdate();
    }


    void BezierValueCurvePoint::clampY(const double min, const double max) {
        pos_.y = std::clamp(pos_.y, -max, max);
        curveMustUpdate();
    }


    void BezierValueCurvePoint::curveMustUpdate() const {
        if (bezier_value_curve_ != nullptr) {
            bezier_value_curve_->mustUpdate();
        }
    }


    auto BezierValueCurvePoint_compare(const void* a, const void* b) -> int32_t {
        return static_cast<const BezierValueCurvePoint*>(a)->compare(
            static_cast<const BezierValueCurvePoint*>(b)
        );
    }

    void BezierValueCurvePoint::remember() {
        remembered_pos_ = pos_;
        remembered_status_ = status_;
    }


    void BezierValueCurvePoint::backToRememberedStatus() {
        status_ = remembered_status_;
    }


    int32_t BezierValueCurvePoint::compare(const BezierValueCurvePoint* point) const {
        if (point != nullptr) {
            if (pos_.x > point->pos_.x) {
                return 1;
            }
            if (pos_.x < point->pos_.x) {
                return -1;
            }
        }

        return 0;
    }


    BezierValueCurve::BezierValueCurve() {
        points_.reserve(16);
    }


    BezierValueCurve::BezierValueCurve(const BezierValueCurve& bezier_value_curve) {
        // TODO: Implement
        #pragma message("BezierValueCurve::BezierValueCurve(const BezierValueCurve& bezier_value_curve) must be implemented!")
    }


    BezierValueCurve::~BezierValueCurve() {
        delete weighted_samples_;
    }


    auto BezierValueCurve::isValid() const -> bool {
        if (mode_ == Mode::Envelope) {
            bool has_decay_begin = false;

            for (const auto& point : points_) {
                if ((point.status_ & BezierValueCurvePoint::Status::DecayBegin) != 0) {
                    has_decay_begin = true;
                }
            }

            if (!has_decay_begin) {
                return false;
            }
        }

        return true;
    }


    auto BezierValueCurve::selectedPointsCount() const -> int32_t {
        int32_t n = 0;

        for (const auto& point : points_) {
            if (point.isSelected()) {
                n++;
            }
        }

        return n;
    }


    auto BezierValueCurve::decayPointIndex() const -> int32_t {
        int32_t index = 0;

        for (const auto& point : points_) {
            if (point.isDecayBegin()) {
                return index;
            }
            index++;
        }

        return -1;
    }


    void BezierValueCurve::rangeY(double& out_min_y, double& out_max_y) const {
        out_min_y =  std::numeric_limits<double>::max();
        out_max_y = -std::numeric_limits<double>::max();

        for (const auto& point : points_) {
            double y = point.pos_.y;
            out_min_y = std::min(y, out_min_y);
            out_max_y = std::max(y, out_max_y);
        }
    }


    auto BezierValueCurve::bbox(bool selected_only) const -> Rectd {
        //  _update();    TODO: Is call to _update necessary?

        double min_x = std::numeric_limits<double>::max();
        double min_y = std::numeric_limits<double>::max();
        double max_x = std::numeric_limits<double>::lowest();
        double max_y = std::numeric_limits<double>::lowest();

        for (const auto& point : points_) {
            if (point.isSelected() || !selected_only) {
                min_x = std::min(point.pos_.x, min_x);
                max_x = std::max(point.pos_.x, max_x);
                min_y = std::min(point.pos_.y, min_y);
                max_y = std::max(point.pos_.y, max_y);
            }
        }

        return { min_x, min_y, max_x - min_x, max_y - min_y };
    }


    auto BezierValueCurve::pointAtIndex(int32_t index) const -> const BezierValueCurvePoint* {
        return index >= 0 && index < static_cast<int32_t>(points_.size()) ? &points_[index] : nullptr;
    }


    auto BezierValueCurve::mutPointAtIndex(int32_t index) -> BezierValueCurvePoint* {
        return index >= 0 && index < static_cast<int32_t>(points_.size()) ? &points_[index] : nullptr;
    }


    auto BezierValueCurve::firstSelectedPoint() -> BezierValueCurvePoint* {
        for (auto& point : points_) {
            if (point.isSelected()) {
                return &point;
            }
        }

        return nullptr;
    }


    auto BezierValueCurve::decayBeginPoint() -> BezierValueCurvePoint* {
        for (auto& point : points_) {
            if (point.isDecayBegin()) {
                return &point;
            }
        }

        return nullptr;
    }


    void BezierValueCurve::setLimits(double min_x, double max_x, double min_y, double max_y) {
        limit_min_x_ = min_x;
        limit_max_x_ = max_x;
        limit_min_y_ = min_y;
        limit_max_y_ = max_y;
    }


    auto BezierValueCurve::decayBeginIndex() const -> int32_t {
        int32_t index = 0;
        for (const auto& point : points_) {
            if (point.isDecayBegin()) {
                return index;
            }
            index++;
        }

        return -1;
    }


    void BezierValueCurve::setDecayBeginIndex(int32_t decay_begin_index) {
        int32_t index = 0;
        for (auto& point : points_) {
            point.changeStatus(BezierValueCurvePoint::Status::DecayBegin, index == decay_begin_index);
            index++;
        }
    }


    void BezierValueCurve::selectAllPoints() {
        for (auto& point : points_) {
            point.select();
        }
    }


    void BezierValueCurve::deselectAllPoints() {
        for (auto& point : points_) {
            point.deselect();
        }
    }


    void BezierValueCurve::selectPointsInRect(const Rectd& rect)  {
        Rectd selection_rect(rect);
        selection_rect.makePositiveSize();

        for (int32_t index = 0; index < length(); index++) {
            if (auto *point = mutPointAtIndex(index)) {
                if (selection_rect.contains(point->pos_)) {
                    point->select();
                }
                else {
                    point->backToRememberedStatus();
                }
            }
        }
    }


    auto BezierValueCurve::setTypeOfSelectedPoints(BezierValueCurvePoint::PointType point_type) -> bool {
        bool changed = false;

        int32_t point_n = length();
        if (point_n < 2) {
            return changed;
        }

        for (int32_t i = 0; i < point_n; i++) {
            auto *l = mutPointAtIndex(i - 1);
            auto *c = mutPointAtIndex(i);
            auto *r = mutPointAtIndex(i + 1);

            if (c->isSelected() && c->point_type_ != point_type) {
                switch (point_type) {
                    case BezierValueCurvePoint::PointType::Linear:
                        break;

                    case BezierValueCurvePoint::PointType::Corner:
                        if (c->point_type_ == BezierValueCurvePoint::PointType::Linear ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Right ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Left) {
                            if (l != nullptr) {
                                c->left_.x = (l->pos_.x - c->pos_.x) / 2;
                                c->left_.y = 0;
                            }
                            if (r != nullptr) {
                                c->right_.x = (r->pos_.x - c->pos_.x) / 2;
                                c->right_.y = 0;
                            }
                        }
                        break;

                    case BezierValueCurvePoint::PointType::Smooth1:
                        if (c->point_type_ == BezierValueCurvePoint::PointType::Linear ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Corner ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Right ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Left) {
                            if (l != nullptr) {
                                c->left_.x = (l->pos_.x - c->pos_.x) / 2;
                                c->left_.y = 0;
                            }
                            if (r != nullptr) {
                                c->right_.x = (r->pos_.x - c->pos_.x) / 2;
                                c->right_.y = 0;
                            }
                        }
                        break;

                    case BezierValueCurvePoint::PointType::Smooth2:
                        if (c->point_type_ == BezierValueCurvePoint::PointType::Linear ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Corner ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Right ||
                            c->point_type_ == BezierValueCurvePoint::PointType::Left) {
                            double length = 0;
                            int32_t n = 0;
                            if (l != nullptr) {
                                length = c->pos_.x - l->pos_.x;
                                n++;
                            }
                            if (r != nullptr) {
                                length += r->pos_.x - c->pos_.x;
                                n++;
                            }
                            if (n > 0) {
                                length /= n * 2;
                                c->left_.x = -length;
                                c->right_.x = length;
                                c->left_.y = 0;
                                c->right_.y = 0;
                            }
                        }
                        else if (c->point_type_ == BezierValueCurvePoint::PointType::Smooth1) {
                            Vec2d v;
                            if ((l != nullptr) && (r != nullptr)) {
                                v = (-c->left_ + c->right_) * 0.5;
                                c->left_ = -v;
                                c->right_ = v;
                            }
                            if (l == nullptr) {
                                c->left_ = -c->right_;
                            }
                            else if (r == nullptr) {
                                c->right_ = -c->left_;
                            }
                        }
                        break;

                    default:
                        break;
                }

                c->point_type_ = point_type;
                changed = true;
            }
        }

        return changed;
    }


    auto BezierValueCurve::horizontalCenterSelectedPoints() -> bool {
        bool changed = false;

        BezierValueCurvePoint* a = nullptr;
        BezierValueCurvePoint* b = nullptr;

        for (auto& point : points_) {
            if ((a != nullptr) && (b != nullptr)) {
                if (b->isSelected() && !a->isSelected() && !point.isSelected()) {
                    b->pos_.x = a->pos_.x + ((point.pos_.x - a->pos_.x) / 2);
                    changed = true;
                }
            }
            a = b;
            b = &point;
        }

        if (changed) {
            mustUpdate();
        }

        return changed;
    }


    auto BezierValueCurve::verticalCenterSelectedPoints() -> bool {
        bool changed = false;

        BezierValueCurvePoint* a = nullptr;
        BezierValueCurvePoint* b = nullptr;

        for (auto& point : points_) {
            if ((a != nullptr) && (b != nullptr)) {
                if (b->isSelected() && !a->isSelected() && !point.isSelected()) {
                    b->pos_.y = a->pos_.y + ((point.pos_.y - a->pos_.y) / 2);
                    changed = true;
                }
            }

            a = b;
            b = &point;
        }

        if (changed) {
            mustUpdate();
        }

        return changed;
    }


    auto BezierValueCurve::alignSelectedPoints(Alignment align) -> bool {
        int32_t n = selectedPointsCount();
        if (align == Alignment::No) {
            if (n < 1) {
                return false;
            }
        }
        else if (n < 2) {
            return false;
        }

        bool changed = false;
        Rectd rect = bbox(true);
        double y = 0.0;

        switch (align) {
            case Alignment::Top: y = rect.bottom(); break;
            case Alignment::Center: y = rect.centerY(); break;
            case Alignment::Bottom: y = rect.y; break;
            case Alignment::No: y = 0; break;
            default: return false;
        }

        for (auto& point : points_) {
            if (point.isSelected()) {
                point.pos_.y = y;
                changed = true;
            }
        }

        if (changed) {
            mustUpdate();
        }

        return changed;
    }


    auto BezierValueCurve::addPoint() -> BezierValueCurvePoint* {
        try {
            auto& point = points_.emplace_back();

            point.bezier_value_curve_ = this;
            mustUpdate();

            return &point;
        }
        catch (...) {
            return nullptr;
        }
    }


    auto BezierValueCurve::addLinearPoint(double x, double y) -> BezierValueCurvePoint* {
            return addPoint(x, y, 0, 0, 0, 0, BezierValueCurvePoint::PointType::Linear);
    }


    auto BezierValueCurve::addPoint(
            double x, double y,
            double lx, double ly,
            double rx, double ry,
            BezierValueCurvePoint::PointType point_type,
            BezierValueCurvePoint::Status status
    ) -> BezierValueCurvePoint* {
        points_.push_back(BezierValueCurvePoint());

        auto *point = mutLastPoint();
        if (point == nullptr) {
            return nullptr;
        }

        point->bezier_value_curve_ = this;
        point->pos_.x = x;
        point->pos_.y = y;
        point->left_.x = lx;
        point->left_.y = ly;
        point->right_.x = rx;
        point->right_.y = ry;
        point->point_type_ = point_type;
        point->status_ = status;

        mustUpdate();

        return point;
    }


    auto BezierValueCurve::removePoint(int32_t index) -> bool {
        if (index >= 0 && index < static_cast<int32_t>(points_.size()) - 1) {
            points_.removeAtIndex(index);
            mustUpdate();
            return true;
        }

        return false;
    }


    auto BezierValueCurve::removeSelectedPoints() -> int32_t {
        int32_t result = 0;

        if (selectedPointsCount() > 0) {
            const auto n = static_cast<int32_t>(points_.size());
            for (int32_t i = n - 1; i >= 0; i--) {
                if (points_[i].isSelected()) {
                    points_.removeAtIndex(i);
                    result++;
                }
            }

            if (result > 0) {
                mustUpdate();
            }
        }

        return result;
    }


    void BezierValueCurve::clear() {
        points_.clear();
    }


    void BezierValueCurve::flipVertical() {
        if (hasPoints()) {
            double min_y;
            double max_y;
            rangeY(min_y, max_y);
            double offset = min_y + max_y;

            for (auto& point : points_) {
                point.pos_.y = offset - point.pos_.y;
                point.left_.y = -point.left_.y;
                point.right_.y = -point.right_.y;
            }

            mustUpdate();
        }
    }


    auto BezierValueCurve::split(int32_t segment_index, float t, bool select) -> bool {
        if (segment_index < 0 || segment_index > length() - 1 || t <= 0 || t >= 1) {
            return false;
        }

        _update();

        auto *point_a = mutPointAtIndex(segment_index);
        auto *point_b = mutPointAtIndex(segment_index + 1);
        if ((point_a == nullptr) || (point_b == nullptr)) {
            return false;
        }

        Bezier bezier(*point_a, *point_b);

        Bezier sub_bezier0;
        Bezier sub_bezier1;
        bezier.split(t, sub_bezier0, sub_bezier1);

        BezierValueCurvePoint::PointType point_type = BezierValueCurvePoint::PointType::Smooth1;
        if (point_a->point_type_ == BezierValueCurvePoint::PointType::Linear && point_b->point_type_ == BezierValueCurvePoint::PointType::Linear) {
            point_type = BezierValueCurvePoint::PointType::Linear;
        }

        auto *new_point = addPoint(sub_bezier1.pos_[0].x, sub_bezier1.pos_[0].y, 0, 0, 0, 0, point_type);
        if (new_point == nullptr) {
            return false;
        }

        if (select) {
            new_point->select();
        }

        new_point->left_ = sub_bezier0.pos_[2] - sub_bezier0.pos_[3];
        new_point->right_ = sub_bezier1.pos_[1] - sub_bezier1.pos_[0];

        point_a->right_ = sub_bezier0.pos_[1] - sub_bezier0.pos_[0];
        point_b->left_ = sub_bezier1.pos_[2] - sub_bezier1.pos_[3];

        if (point_a->point_type_ == BezierValueCurvePoint::PointType::Smooth2) {
            point_a->point_type_ = BezierValueCurvePoint::PointType::Smooth1;
        }
        if (point_b->point_type_ == BezierValueCurvePoint::PointType::Smooth2) {
            point_b->point_type_ = BezierValueCurvePoint::PointType::Smooth1;
        }

        mustUpdate();

        return true;
    }


    void BezierValueCurve::rememberAllPoints() {
        for (auto& point : points_) {
            point.remember();
        }
    }


    auto BezierValueCurve::moveRememberedSelectedPoints(const Vec2d& delta) -> bool {
        bool changed = false;

        for (auto& point : points_) {
            if (point.isSelected()) {
                if (!point.isXFixed()) {
                    double new_x = std::clamp<double>(point.remembered_pos_.x + delta.x, limit_min_x_, limit_max_x_);
                    if (new_x != point.pos_.x) {
                        point.pos_.x = new_x;
                        changed = true;
                    }
                }

                if (!point.isYFixed()) {
                    double new_y = std::clamp<double>(point.remembered_pos_.y + delta.y, limit_min_y_, limit_max_y_);
                    if (new_y != point.pos_.y) {
                        point.pos_.y = new_y;
                        changed = true;
                    }
                }
            }
        }

        if (changed) {
            mustUpdate();
        }

        return changed;
    }


    void BezierValueCurve::mustSort() {
        must_sort_ = true;
        modification_count_++;
    }


    void BezierValueCurve::mustUpdate() {
        must_sort_ = true;
        must_update_ = true;
        modification_count_++;
    }


    auto BezierValueCurve::lookup(float t, int32_t resolution) -> float {
        _updateWeightedSamples(resolution);
        if (weighted_samples_ == nullptr) {
            return 0;
        }

        return weighted_samples_->lookup(t);
    }

/* TODO: !
    bool BezierValueCurve::fillLUT(LUT1* lut) noexcept {
        if (!lut) {
            return false;
        }

        if (!_updateWeightedSamples(lut->resolution())) {
            return false;
        }

        if (!lut->setByWeightedSamples(weighted_samples_)) {
            return false;
        }

        return true;
    }


    bool BezierValueCurve::fillEnvelopeAttackLUT(LUT1* lut) noexcept {
        if (lut && mode_ == Mode::Envelope) {
            int32_t end_index = decayBeginIndex();
            if (end_index >= 0) {
                return _updateLUT(lut, 0, end_index) == ErrorCode::None;
            }
        }

        return false;
    }


    bool BezierValueCurve::fillEnvelopeDecayLUT(LUT1* lut) noexcept {
        if (lut && mode_ == Mode::Envelope) {
            int32_t begin_index = decayBeginIndex();
            if (begin_index >= 0) {

                int32_t endIndex = length() - 1;
                if (endIndex > begin_index) {
                    return _updateLUT(lut, begin_index, endIndex) == ErrorCode::None;
                }
            }
        }

        return false;
    }


    ErrorCode BezierValueCurve::fillBuffer(int32_t start_index, int32_t end_index, float* buffer, int32_t length) noexcept {
        // TODO: Test, optimization

        if (!buffer) {
            return ErrorCode::NullData;
        }

        if (length < 1) {
            return ErrorCode::BadArgs;
        }

        int32_t point_count = this->length();
        if (point_count < 2) {
            return Error::specific(kErr_ToFewPoints);
        }

        if (start_index < 0 || start_index >= point_count - 1) {
            return Error::specific(kErr_StartIndexOutOfRange);
        }

        if (end_index < start_index + 1 || end_index >= point_count) {
            return Error::specific(kErr_EndIndexOutOfRange);
        }

        int32_t segment_count = end_index - start_index;

        auto start_point = pointAtIndex(start_index);
        auto end_point = pointAtIndex(end_index);

        double start_x = start_point->pos_.x;
        double end_x = end_point->pos_.x;
        double width = end_x - start_x;

        if (width <= 0) {
            return Error::specific(kErrInvalidWidth);
        }

        double t = 0;
        int32_t index1 = 0;
        for (int32_t si = 0; si < segment_count; si++) {
            int32_t pi1 = start_index + si;
            int32_t pi2 = pi1 + 1;

            auto p1 = pointAtIndex(pi1);
            auto p2 = pointAtIndex(pi2);

            double x1 = p1->pos_.x;
            double x2 = p2->pos_.x;
            double segment_width = x2 - x1;

            double f = segment_width / width;
            t += f;

            int32_t index2 = std::round(t * length);

            Vec2d bp1 = p1->pos_;
            Vec2d bp2 = p1->used_right_pos_;
            Vec2d bp3 = p2->used_left_pos_;
            Vec2d bp4 = p2->pos_;

            float scale = 1.0f / width;
            bp1.x = (bp1.x - start_x) * scale;
            bp2.x = (bp2.x - start_x) * scale;
            bp3.x = (bp3.x - start_x) * scale;
            bp4.x = (bp4.x - start_x) * scale;

            Bezier bezier(bp1, bp2, bp3, bp4);

            for (int32_t i = index1; i < index2; i++) {
                double x = bp1.x + static_cast<double>(i - index1) / (index2 - index1) * (bp4.x - bp1.x);

                double roots[3];
                int32_t n = Math::solveCubicBezier(bp1.x, bp2.x, bp3.x, bp4.x, x, roots);
                if (n > 0) {
                    double t = roots[0];
                    Vec2d sample = bezier.posOnCurve(t);
                    buffer[i] = sample.y_;
                }

            }

            index1 = index2;
        }


        return ErrorCode::None;
    }
*/

    void BezierValueCurve::_sortPoints() {
        if (must_sort_) {
            std::ranges::sort(points_, sortPointsCompareFunc);
            must_sort_ = false;
        }
    }


    auto BezierValueCurve::sortPointsCompareFunc(const BezierValueCurvePoint& a, const BezierValueCurvePoint& b) -> bool {
        return a.pos_.x < b.pos_.x;
    }


    auto BezierValueCurve::_updateWeightedSamples(int32_t resolution) -> bool {
        return _updateWeightedSamples(0, length() - 1, resolution);
    }


    auto BezierValueCurve::_updateWeightedSamples(
        int32_t start_point_index,
        int32_t end_point_index,
        int32_t resolution
    ) -> bool {
        if (weighted_samples_modification_count_ >= modification_count_ &&
            weighted_samples_resolution_ == resolution &&
            weighted_samples_start_point_index_ == start_point_index &&
            weighted_samples_end_point_index_ == end_point_index) {
            // Nothing changed since last update.
            return true;
        }

        if (resolution < 1) {
            return false;
        }

        if (length() < 2) {
            return false;
        }

        start_point_index = std::clamp<int32_t>(start_point_index, 0, length() - 1);
        end_point_index = std::clamp<int32_t>(end_point_index, start_point_index + 1, length() - 1);
        int32_t point_n = end_point_index - start_point_index + 1;
        if (point_n < 2) {
            return false;
        }

        if (weighted_samples_ == nullptr) {
            weighted_samples_ = new (std::nothrow) WeightedSamples(resolution);
        }
        else {
            if (weighted_samples_->setResolution(resolution) != WeightedSamples::kErrNone) {
                return false;
            }
        }

        if (weighted_samples_ == nullptr) {
            // GrApp::addError(Gr::ERR_FATAL, "BezierCurve::_updateWeightedSamples()"); TODO: !!!!
            return false;
        }

        weighted_samples_->clear();
        _update();

        const auto *start_point = pointAtIndex(start_point_index);
        const auto *end_point = pointAtIndex(end_point_index);
        if (start_point == nullptr || end_point == nullptr) {
            return false;
        }

        double min_x = start_point->pos_.x;
        double max_x = end_point->pos_.x;
        double width = max_x - min_x;
        if (width < std::numeric_limits<double>::epsilon()) {
            width = std::numeric_limits<double>::epsilon();
        }

        double scale = 1.0 / width;

        for (int32_t i = 1; i < point_n; i++) {
            const auto *cp1 = pointAtIndex(start_point_index + i - 1);
            const auto *cp2 = pointAtIndex(start_point_index + i);

            Vec2d bp1 = cp1->pos_;
            Vec2d bp2 = cp1->used_right_pos_;
            Vec2d bp3 = cp2->used_left_pos_;
            Vec2d bp4 = cp2->pos_;

            bp1.x = (bp1.x - min_x) * scale;
            bp2.x = (bp2.x - min_x) * scale;
            bp3.x = (bp3.x - min_x) * scale;
            bp4.x = (bp4.x - min_x) * scale;

            Bezier bezier(bp1, bp2, bp3, bp4);
            weighted_samples_->addBezier(bezier, 256); // TODO: configurable resolution?
        }

        weighted_samples_->finish();

        weighted_samples_modification_count_ = modification_count_;
        weighted_samples_resolution_ = resolution;
        weighted_samples_start_point_index_ = start_point_index;
        weighted_samples_end_point_index_ = end_point_index;

        return true;
    }

/* TODO: !
    Error BezierValueCurve::_updateLUT(LUT1* lut) noexcept {
        return _updateLUT(lut, 0, length() - 1);
    }


    ErrorCode BezierValueCurve::_updateLUT(LUT1* lut, int32_t start_point_index, int32_t end_point_index) noexcept {
        if (!lut) {
            return ErrorCode::NullData;
        }

        if (!_updateWeightedSamples(start_point_index, end_point_index, lut->resolution())) {
            return ErrorCode::Specific;
        }

        lut->setByWeightedSamples(weighted_samples_);

        return ErrorCode::None;
    }
*/

    bool BezierValueCurve::_update() {
        _sortPoints();
        int32_t point_n = length();

        if (must_update_ && point_n > 1) {

            // Pass 1, left to right
            for (int32_t i = 0; i < point_n - 1; i++) {
                auto *p0 = mutPointAtIndex(i);
                auto *p1 = mutPointAtIndex(i + 1);

                p0->right_.x = std::max<double>(p0->right_.x, 0);

                Vec2d v = p0->right_;
                if (v.x > 0) {
                    if (v.x > p1->pos_.x - p0->pos_.x) {
                        v.x = p1->pos_.x - p0->pos_.x;
                        v.y = p0->right_.y * v.x / p0->right_.x;
                    }
                }

                p0->right_pos_ = p0->pos_ + p0->right_;
                p0->used_right_pos_ = p0->pos_ + v;
            }

            // Pass 2, right to left
            for (int32_t i = point_n - 1; i > 0; i--) {
                auto *p0 = mutPointAtIndex(i);
                auto *p1 = mutPointAtIndex(i - 1);

                p0->left_.x = std::min<double>(p0->left_.x, 0);

                Vec2d v = p0->left_;
                if (v.x < 0) {
                    if (v.x < p1->pos_.x - p0->pos_.x) {
                        v.x = p1->pos_.x - p0->pos_.x;
                        v.y = p0->left_.y * v.x / p0->left_.x;
                    }
                }
                p0->left_pos_ = p0->pos_ + p0->left_;
                p0->used_left_pos_ = p0->pos_ + v;
            }

            // Pass 3, automatic right continuity
            auto *p = mutPointAtIndex(0);
            if (p->point_type_ == BezierValueCurvePoint::PointType::Right) {
                p->used_right_pos_ = p->pos_;
            }

            for (int32_t i = 1; i < point_n - 1; i++) {
                auto *c = mutPointAtIndex(i);

                if (c->point_type_ == BezierValueCurvePoint::PointType::Right) {
                    auto *l = mutPointAtIndex(i - 1);
                    auto *r = mutPointAtIndex(i + 1);

                    Vec2d v = c->pos_ - (l->usesRightControl() ? l->used_right_pos_ : l->pos_);
                    Lined line({0.0, 0.0}, {v.x, v.y});
                    double right_pos = (r->pos_.x - c->pos_.x) / 2;

                    Lined top_line({0.0, 1.0}, {right_pos, 1.0});
                    Vec2d iv;
                    if (line.intersects(top_line, iv)) {
                        if (iv.x > 0 && iv.length() < v.length()) {
                            v = iv;
                        }
                    }

                    Lined bottom_line({0.0, -1.0}, {right_pos, -1.0});
                    if (line.intersects(bottom_line, iv)) {
                        if (iv.x > 0 && iv.length() < v.length()) {
                            v = iv;
                        }
                    }

                    Lined right_line({right_pos, -1.0}, {right_pos, 1.0});
                    if (line.intersects(right_line, iv)) {
                        if (iv.x > 0 && iv.length() < v.length()) {
                            v = iv;
                        }
                    }

                    c->used_right_pos_ = c->pos_ + v;
                }
            }

            // Pass 4, automatic left continuity
            p = mutPointAtIndex(point_n - 1);
            if (p->point_type_ == BezierValueCurvePoint::PointType::Left) {
                p->used_left_pos_ = p->pos_;
            }

            for (int32_t i = point_n - 2; i > 0; i--) {
                auto *c = mutPointAtIndex(i);

                if (c->point_type_ == BezierValueCurvePoint::PointType::Left) {
                    auto *l = mutPointAtIndex(i - 1);
                    auto *r = mutPointAtIndex(i + 1);

                    Vec2d v = (r->usesLeftControl() ? r->used_left_pos_ : r->pos_) - c->pos_;
                    Lined line({0.0, 0.0}, {v.x, v.y});
                    double left_pos = (l->pos_.x - c->pos_.x) / 2;

                    Vec2d iv;

                    Lined left_line({left_pos, -1.0}, {left_pos, 1.0});
                    if (line.intersects(left_line, iv)) {
                        if (iv.x < 0 && iv.length() < v.length()) {
                            v = -iv;
                        }
                    }

                    Lined top_line({left_pos, 1.0}, {0.0, 1.0});
                    if (line.intersects(top_line, iv)) {
                        if (iv.x < 0 && iv.length() < v.length()) {
                            v = -iv;
                        }
                    }

                    Lined bottom_line({left_pos, -1.0}, {0.0, -1.0});
                    if (line.intersects(bottom_line, iv)) {
                        if (iv.x < 0 && iv.length() < v.length()) {
                            v = -iv;
                        }
                    }

                    c->used_left_pos_ = c->pos_ - v;
                }
            }

            must_update_ = false;
            return true;
        }

        return false;
    }

} // End of namespace Grain
