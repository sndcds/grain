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
    Signal& operator=(const Signal&) = delete;

    Signal(Signal&&) noexcept = default;
    Signal& operator=(Signal&&) noexcept = default;


    // Configuration

    [[nodiscard]]
    std::int32_t configure(
        std::int32_t channel_count,
        std::int32_t sample_rate,
        std::int64_t sample_count,
        SignalSampleType sample_type
    ) {
        return data_.configure(
            channel_count,
            sample_rate,
            sample_count,
            sample_type
        );
    }

    [[nodiscard]]
    std::int32_t growIfNeeded(std::int64_t sample_count) {
        return data_.growIfNeeded(sample_count);
    }

    void clear() {
        data_.clear();
    }

    void free() {
        data_.free();
    }


    // Configuration information

    [[nodiscard]]
    SignalSampleType sampleType() const noexcept {
        return data_.sampleType();
    }

    [[nodiscard]]
    std::int32_t sampleRate() const noexcept {
        return data_.sampleRate();
    }

    [[nodiscard]]
    std::int32_t channelCount() const noexcept {
        return data_.channelCount();
    }

    [[nodiscard]]
    std::int64_t sampleCount() const noexcept {
        return data_.sampleCount();
    }

    [[nodiscard]]
    std::int64_t lastSampleIndex() const noexcept {
        return data_.lastSampleIndex();
    }

    [[nodiscard]]
    std::int64_t allChannelSampleCount() const noexcept {
        return data_.allChannelSampleCount();
    }

    [[nodiscard]]
    std::int32_t bitsPerSample() const noexcept {
        return data_.bitsPerSample();
    }

    [[nodiscard]]
    std::int32_t bytesPerSample() const noexcept {
        return data_.bytesPerSample();
    }

    [[nodiscard]]
    std::size_t dataSize() const noexcept {
        return data_.dataSize();
    }


    // Information

    [[nodiscard]]
    bool hasData() const noexcept {
        return data_.hasData();
    }

    [[nodiscard]]
    bool isConfigured() const noexcept {
        return data_.isConfigured();
    }

    [[nodiscard]]
    bool isIntegerType() const noexcept {
        return data_.isIntegerType();
    }

    [[nodiscard]]
    bool isFloatingPointType() const noexcept {
        return data_.isFloatingPointType();
    }

    [[nodiscard]]
    bool isInt8Type() const noexcept {
        return data_.isInt8Type();
    }

    [[nodiscard]]
    bool isInt16Type() const noexcept {
        return data_.isInt16Type();
    }

    [[nodiscard]]
    bool isInt32Type() const noexcept {
        return data_.isInt32Type();
    }

    [[nodiscard]]
    bool isFloatType() const noexcept {
        return data_.isFloatType();
    }

    [[nodiscard]]
    bool isMono() const noexcept {
        return data_.isMono();
    }

    [[nodiscard]]
    bool isStereo() const noexcept {
        return data_.isStereo();
    }

    [[nodiscard]]
    bool isQuadra() const noexcept {
        return data_.isQuadra();
    }

    [[nodiscard]]
    bool hasChannel(std::int32_t channel) const noexcept {
        return data_.hasChannel(channel);
    }

    [[nodiscard]]
    bool hasChannelAndData(std::int32_t channel) const noexcept {
        return data_.hasChannelAndData(channel);
    }

    [[nodiscard]]
    bool hasSampleAtChannel(
        std::int32_t channel,
        std::int64_t index
    ) const noexcept {
        return data_.hasSampleAtChannel(channel, index);
    }


    // Sample storage access

    [[nodiscard]]
    const void* dataPtr() const noexcept {
        return data_.dataPtr();
    }

    [[nodiscard]]
    void* mutDataPtr() noexcept {
        return data_.mutDataPtr();
    }

    [[nodiscard]]
    const void* dataPtr(
        std::int32_t channel,
        std::int64_t index = 0
    ) const {
        return data_.dataPtr(channel, index);
    }

    [[nodiscard]]
    void* mutDataPtr(
        std::int32_t channel,
        std::int64_t index = 0
    ) {
        return data_.mutDataPtr(channel, index);
    }


    // Typed sample access

    [[nodiscard]]
    std::int8_t readInt8(
        std::int32_t channel,
        std::int64_t index
    ) const {
        return data_.readInt8(channel, index);
    }

    [[nodiscard]]
    std::int16_t readInt16(
        std::int32_t channel,
        std::int64_t index
    ) const {
        return data_.readInt16(channel, index);
    }

    [[nodiscard]]
    std::int32_t readInt32(
        std::int32_t channel,
        std::int64_t index
    ) const {
        return data_.readInt32(channel, index);
    }

    [[nodiscard]]
    float readFloat(
        std::int32_t channel,
        std::int64_t index
    ) const {
        return data_.readFloat(channel, index);
    }

    [[nodiscard]]
    double readDouble(
        std::int32_t channel,
        std::int64_t index
    ) const {
        return data_.readDouble(channel, index);
    }

    void writeInt8(
        std::int32_t channel,
        std::int64_t index,
        std::int8_t value
    ) {
        data_.writeInt8(channel, index, value);
    }

    void writeInt16(
        std::int32_t channel,
        std::int64_t index,
        std::int16_t value
    ) {
        data_.writeInt16(channel, index, value);
    }

    void writeInt32(
        std::int32_t channel,
        std::int64_t index,
        std::int32_t value
    ) {
        data_.writeInt32(channel, index, value);
    }

    void writeFloat(
        std::int32_t channel,
        std::int64_t index,
        float value
    ) {
        data_.writeFloat(channel, index, value);
    }

    void writeDouble(
        std::int32_t channel,
        std::int64_t index,
        double value
    ) {
        data_.writeDouble(channel, index, value);
    }


private:
    SignalData data_;
};

} // namespace Grain