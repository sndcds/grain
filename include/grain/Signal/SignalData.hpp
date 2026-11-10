#pragma once

#include <cstddef>
#include <cstdint>


namespace Grain {

    union SignalSamplePtr {
        void* raw = nullptr;

        int8_t* i8;
        int16_t* i16;
        int32_t* i32;
        float* f32;
    };

    enum class SignalSampleType : uint8_t {
        Undefined,
        Int8,
        Int16,
        Int32,
        Float32
    };

    /**
     * @class SignalData
     * @brief Owns and manages the sample data of a Signal.
     *
     * SignalData is responsible for:
     * - Sample memory allocation and deallocation.
     * - Sample format and dimensions.
     * - Interleaved multichannel sample storage.
     * - Typed sample access and conversion to/from float.
     *
     * SignalData does not perform signal processing.
     */
    class SignalData {

    public:
        enum {
            kErrNone = 0,
            kErrInvalidConfiguration,
            kErrUnsupportedSampleType,
            kErrAllocationFailed
        };

        SignalData() = default;

        ~SignalData();

        SignalData(const SignalData&) = delete;
        auto operator = (const SignalData&) -> SignalData& = delete;

        SignalData(SignalData&& other) noexcept;
        auto operator = (SignalData&& other) noexcept -> SignalData&;


        //----------------------------------------------------------------------
        //  Configuration
        //----------------------------------------------------------------------

        [[nodiscard]] auto configure(
            int32_t channel_count,
            int32_t sample_rate,
            int64_t sample_count,
            SignalSampleType sample_type
        ) -> int32_t;

        [[nodiscard]] auto growIfNeeded(int64_t sample_count) -> int32_t;

        void clear() noexcept;

        void free() noexcept;


        //----------------------------------------------------------------------
        //  Configuration information
        //----------------------------------------------------------------------

        [[nodiscard]] auto sampleType() const -> SignalSampleType { return sample_type_; }
        [[nodiscard]] auto channelCount() const -> int32_t { return channel_count_; }
        [[nodiscard]] auto sampleRate() const -> int32_t { return sample_rate_; }

        [[nodiscard]] auto sampleCount() const -> int64_t {return sample_count_; }

        [[nodiscard]]
        auto lastSampleIndex() const noexcept -> int64_t {
            return last_sample_index_;
        }

        [[nodiscard]]
        auto allChannelSampleCount() const noexcept -> int64_t {
            return sample_count_ * channel_count_;
        }

        [[nodiscard]]
        auto bitsPerSample() const noexcept -> int32_t {
            return bits_per_sample_;
        }

        [[nodiscard]]
        auto bytesPerSample() const noexcept -> int32_t {
            return bytes_per_sample_;
        }

        [[nodiscard]]
        auto dataSize() const noexcept -> size_t {
            return data_byte_size_;
        }


        //----------------------------------------------------------------------
        //  Information
        //----------------------------------------------------------------------

        [[nodiscard]]
        auto hasData() const noexcept -> bool {
            return data_byte_size_ > 0 && data_.raw != nullptr;
        }

        [[nodiscard]]
        auto isConfigured() const noexcept -> bool {
            return channel_count_ > 0 &&
                   sample_rate_ > 0 &&
                   sample_count_ > 0 &&
                   sample_type_ != SignalSampleType::Undefined;
        }

        [[nodiscard]]
        auto isIntegerType() const noexcept -> bool {
            return sample_type_ == SignalSampleType::Int8 ||
                   sample_type_ == SignalSampleType::Int16 ||
                   sample_type_ == SignalSampleType::Int32;
        }

        [[nodiscard]]
        auto isFloatingPointType() const noexcept -> bool {
            return sample_type_ == SignalSampleType::Float32;
        }

        [[nodiscard]]
        auto isInt8Type() const noexcept -> bool {
            return sample_type_ == SignalSampleType::Int8;
        }

        [[nodiscard]]
        auto isInt16Type() const noexcept -> bool {
            return sample_type_ == SignalSampleType::Int16;
        }

        [[nodiscard]]
        auto isInt32Type() const noexcept -> bool {
            return sample_type_ == SignalSampleType::Int32;
        }

        [[nodiscard]]
        auto isFloatType() const noexcept -> bool {
            return sample_type_ == SignalSampleType::Float32;
        }

        [[nodiscard]]
        auto isMono() const noexcept -> bool {
            return channel_count_ == 1;
        }

        [[nodiscard]]
        auto isStereo() const noexcept -> bool {
            return channel_count_ == 2;
        }

        [[nodiscard]]
        auto isQuadra() const noexcept -> bool {
            return channel_count_ == 4;
        }

        [[nodiscard]]
        auto hasChannel(int32_t channel) const noexcept -> bool {
            return channel >= 0 && channel < channel_count_;
        }

        [[nodiscard]]
        auto hasChannelAndData(int32_t channel) const noexcept -> bool {
            return hasChannel(channel) && hasData();
        }

        [[nodiscard]]
        auto hasSampleAtChannel(
            int32_t channel,
            int64_t index
        ) const noexcept -> bool {
            return hasChannelAndData(channel) &&
                   index >= 0 &&
                   index < sample_count_;
        }


        //----------------------------------------------------------------------
        //  Sample storage
        //----------------------------------------------------------------------

        [[nodiscard]]
        auto dataPtr() const noexcept -> const void* {
            return data_.raw;
        }

        [[nodiscard]]
        auto mutDataPtr() const noexcept -> void* {
            return data_.raw;
        }

        [[nodiscard]]
        auto dataPtr(
            int32_t channel,
            int64_t index = 0
        ) const noexcept -> const void*;

        [[nodiscard]]
        auto mutDataPtr(
            int32_t channel,
            int64_t index = 0
        ) noexcept -> void*;


        // Typed sample access

        [[nodiscard]]
        auto readInt8(
            int32_t channel,
            int64_t index
        ) const -> int8_t;

        [[nodiscard]]
        auto readInt16(
            int32_t channel,
            int64_t index
        ) const -> int16_t;

        [[nodiscard]]
        auto readInt32(
            int32_t channel,
            int64_t index
        ) const -> int32_t;

        [[nodiscard]]
        auto readFloat(
            int32_t channel,
            int64_t index
        ) const -> float;

        [[nodiscard]]
        auto readDouble(
            int32_t channel,
            int64_t index
        ) const -> double;


        void writeInt8(
            int32_t channel,
            int64_t index,
            int8_t value
        );

        void writeInt16(
            int32_t channel,
            int64_t index,
            int16_t value
        );

        void writeInt32(
            int32_t channel,
            int64_t index,
            int32_t value
        );

        void writeFloat(
            int32_t channel,
            int64_t index,
            float value
        );

        void writeDouble(
            int32_t channel,
            int64_t index,
            double value
        );


        //----------------------------------------------------------------------
        //  Utilities
        //----------------------------------------------------------------------

        [[nodiscard]]
        auto hasSameFormatAs(const SignalData& other) const noexcept -> bool;


        //----------------------------------------------------------------------
        //  Swap
        //----------------------------------------------------------------------

        void swap(SignalData& other) noexcept;


    private:
        SignalSampleType sample_type_ = SignalSampleType::Undefined;

        int32_t sample_rate_ = 0;
        int32_t channel_count_ = 0;
        int64_t sample_count_ = 0;

        SignalSamplePtr data_;

        int32_t bits_per_sample_ = 0;
        int32_t bytes_per_sample_ = 0;

        size_t data_byte_size_ = 0;

        int64_t last_sample_index_ = 0;
    };


} // namespace Grain
