#pragma once

#include <cstddef>
#include <cstdint>

#include <grain/Signal/SignalData.hpp>


namespace Grain {

class Signal {

public:
    Signal() = default;
    ~Signal() = default;

    Signal(const Signal&) = delete;
    auto operator = (const Signal&) -> Signal& = delete;

    Signal(Signal&&) noexcept = default;
    auto operator = (Signal&&) noexcept -> Signal& = default;


    //--------------------------------------------------------------------------
    //  Configuration.
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto configure(
        int32_t channel_count,
        int32_t sample_rate,
        int64_t sample_count,
        SignalSampleType sample_type
    ) -> int32_t {
        return data_.configure(
            channel_count,
            sample_rate,
            sample_count,
            sample_type
        );
    }

    [[nodiscard]]
    auto growIfNeeded(int64_t sample_count) -> int32_t {
        return data_.growIfNeeded(sample_count);
    }

    void clear() {
        data_.clear();
    }

    void free() {
        data_.free();
    }


    //--------------------------------------------------------------------------
    //  Configuration information.
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto sampleType() const noexcept -> SignalSampleType {
        return data_.sampleType();
    }

    [[nodiscard]]
    auto sampleRate() const noexcept -> int32_t {
        return data_.sampleRate();
    }

    [[nodiscard]]
    auto channelCount() const noexcept -> int32_t {
        return data_.channelCount();
    }

    [[nodiscard]]
    auto sampleCount() const noexcept -> int64_t {
        return data_.sampleCount();
    }

    [[nodiscard]]
    auto lastSampleIndex() const noexcept -> int64_t {
        return data_.lastSampleIndex();
    }

    [[nodiscard]]
    auto allChannelSampleCount() const noexcept -> int64_t {
        return data_.allChannelSampleCount();
    }

    [[nodiscard]]
    auto bitsPerSample() const noexcept -> int32_t {
        return data_.bitsPerSample();
    }

    [[nodiscard]]
    auto bytesPerSample() const noexcept -> int32_t {
        return data_.bytesPerSample();
    }

    [[nodiscard]]
    auto dataSize() const noexcept -> std::size_t {
        return data_.dataSize();
    }


    //--------------------------------------------------------------------------
    //  Information.
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto hasData() const noexcept -> bool {
        return data_.hasData();
    }

    [[nodiscard]]
    auto isConfigured() const noexcept -> bool {
        return data_.isConfigured();
    }

    [[nodiscard]]
    auto isIntegerType() const noexcept -> bool {
        return data_.isIntegerType();
    }

    [[nodiscard]]
    auto isFloatingPointType() const noexcept -> bool {
        return data_.isFloatingPointType();
    }

    [[nodiscard]]
    auto isInt8Type() const noexcept -> bool {
        return data_.isInt8Type();
    }

    [[nodiscard]]
    auto isInt16Type() const noexcept -> bool {
        return data_.isInt16Type();
    }

    [[nodiscard]]
    auto isInt32Type() const noexcept -> bool {
        return data_.isInt32Type();
    }

    [[nodiscard]]
    auto isFloatType() const noexcept -> bool {
        return data_.isFloatType();
    }

    [[nodiscard]]
    auto isMono() const noexcept -> bool {
        return data_.isMono();
    }

    [[nodiscard]]
    auto isStereo() const noexcept -> bool {
        return data_.isStereo();
    }

    [[nodiscard]]
    auto isQuadra() const noexcept -> bool {
        return data_.isQuadra();
    }

    [[nodiscard]]
    auto hasChannel(int32_t channel) const noexcept -> bool {
        return data_.hasChannel(channel);
    }

    [[nodiscard]]
    auto hasChannelAndData(int32_t channel) const noexcept -> bool {
        return data_.hasChannelAndData(channel);
    }

    [[nodiscard]]
    auto hasSampleAtChannel(
        int32_t channel,
        int64_t index
    ) const noexcept -> bool {
        return data_.hasSampleAtChannel(channel, index);
    }


    //--------------------------------------------------------------------------
    //  Sample storage access.
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto dataPtr() const noexcept -> const void* {
        return data_.dataPtr();
    }

    [[nodiscard]]
    auto mutDataPtr() noexcept -> void* {
        return data_.mutDataPtr();
    }

    [[nodiscard]]
    auto dataPtr(
        int32_t channel,
        int64_t index = 0
    ) const -> const void* {
        return data_.dataPtr(channel, index);
    }

    [[nodiscard]]
    auto mutDataPtr(
        int32_t channel,
        int64_t index = 0
    ) -> void* {
        return data_.mutDataPtr(channel, index);
    }


    //--------------------------------------------------------------------------
    //  Typed sample access.
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto readInt8(
        int32_t channel,
        int64_t index
    ) const -> int8_t {
        return data_.readInt8(channel, index);
    }

    [[nodiscard]]
    auto readInt16(
        int32_t channel,
        int64_t index
    ) const -> int16_t {
        return data_.readInt16(channel, index);
    }

    [[nodiscard]]
    auto readInt32(
        int32_t channel,
        int64_t index
    ) const -> int32_t {
        return data_.readInt32(channel, index);
    }

    [[nodiscard]]
    auto readFloat(
        int32_t channel,
        int64_t index
    ) const -> float {
        return data_.readFloat(channel, index);
    }

    [[nodiscard]]
    auto readDouble(
        int32_t channel,
        int64_t index
    ) const -> double {
        return data_.readDouble(channel, index);
    }

    void writeInt8(
        int32_t channel,
        int64_t index,
        int8_t value
    ) {
        data_.writeInt8(channel, index, value);
    }

    void writeInt16(
        int32_t channel,
        int64_t index,
        int16_t value
    ) {
        data_.writeInt16(channel, index, value);
    }

    void writeInt32(
        int32_t channel,
        int64_t index,
        int32_t value
    ) {
        data_.writeInt32(channel, index, value);
    }

    void writeFloat(
        int32_t channel,
        int64_t index,
        float value
    ) {
        data_.writeFloat(channel, index, value);
    }

    void writeDouble(
        int32_t channel,
        int64_t index,
        double value
    ) {
        data_.writeDouble(channel, index, value);
    }


private:
    SignalData data_;
};

} // namespace Grain