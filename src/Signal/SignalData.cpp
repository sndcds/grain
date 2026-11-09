
#include <grain/Signal/SignalData.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

namespace Grain {

namespace {

struct SampleFormat {
    int32_t bits;
    int32_t bytes;
};


[[nodiscard]]
auto getSampleFormat(
    SignalSampleType sample_type,
    SampleFormat& format
) -> bool {

    switch (sample_type) {
        case SignalSampleType::Int8:
            format = {8, 1};
            return true;
        case SignalSampleType::Int16:
            format = {16, 2};
            return true;
        case SignalSampleType::Int32:
            format = {32, 4};
            return true;
        case SignalSampleType::Float32:
            format = {32, 4};
            return true;
        case SignalSampleType::Undefined:
        default:
            return false;
    }
}


[[nodiscard]]
bool calculateDataSize(
    int32_t channel_count,
    int64_t sample_count,
    int32_t bytes_per_sample,
    size_t& result
) {

    if (channel_count <= 0 ||
        sample_count <= 0 ||
        bytes_per_sample <= 0) {
        return false;
    }

    const auto channels = static_cast<size_t>(channel_count);
    const auto samples = static_cast<uint64_t>(sample_count);
    const auto bytes = static_cast<size_t>(bytes_per_sample);

    if (samples > std::numeric_limits<size_t>::max() / channels) {
        return false;
    }

    const size_t all_samples =
        static_cast<size_t>(samples) * channels;

    if (all_samples > std::numeric_limits<size_t>::max() / bytes) {
        return false;
    }

    result = all_samples * bytes;
    return true;
}


template<typename T>
[[nodiscard]]
auto clampNumericValue(long double value) -> T {

    if constexpr (std::numeric_limits<T>::is_integer) {
        const long double minimum =
            static_cast<long double>(std::numeric_limits<T>::lowest());

        const long double maximum =
            static_cast<long double>(std::numeric_limits<T>::max());

        value = std::clamp(value, minimum, maximum);
    }

    return static_cast<T>(value);
}


template<typename T>
[[nodiscard]]
auto readSample(
    SignalSampleType sample_type,
    const SignalSamplePtr& data,
    size_t index
) -> T {

    switch (sample_type) {
        case SignalSampleType::Int8: return static_cast<T>(data.i8[index]);
        case SignalSampleType::Int16: return static_cast<T>(data.i16[index]);
        case SignalSampleType::Int32: return static_cast<T>(data.i32[index]);
        case SignalSampleType::Float32: return static_cast<T>(data.f32[index]);
        default: return static_cast<T>(0);
    }
}


template<typename T>
void writeSample(
    SignalSampleType sample_type,
    SignalSamplePtr& data,
    size_t index,
    T value
) {

    const auto converted = static_cast<long double>(value);

    switch (sample_type) {
        case SignalSampleType::Int8:
            data.i8[index] = clampNumericValue<int8_t>(converted);
            break;
        case SignalSampleType::Int16:
            data.i16[index] = clampNumericValue<int16_t>(converted);
            break;
        case SignalSampleType::Int32:
            data.i32[index] = clampNumericValue<int32_t>(converted);
            break;
        case SignalSampleType::Float32:
            data.f32[index] = static_cast<float>(value);
            break;
        default:
            break;
    }
}

} // namespace


//------------------------------------------------------------------------------
//  Construction and destruction.
//------------------------------------------------------------------------------

SignalData::~SignalData() {
    free();
}


SignalData::SignalData(SignalData&& other) noexcept {
    swap(other);
}

SignalData& SignalData::operator=(SignalData&& other) noexcept {
    if (this != &other) {
        free();
        swap(other);
    }

    return *this;
}


//------------------------------------------------------------------------------
//  Configuration.
//------------------------------------------------------------------------------

auto SignalData::configure(
    int32_t channel_count,
    int32_t sample_rate,
    int64_t sample_count,
    SignalSampleType sample_type
) -> int32_t {

    // Validate configuration.
    if (channel_count <= 0 ||
        sample_rate <= 0 ||
        sample_count <= 0 ||
        sample_type == SignalSampleType::Undefined) {
        return kErrInvalidConfiguration;
    }

    // Determine sample format.
    int32_t bytes_per_sample = 0;

    switch (sample_type) {
        case SignalSampleType::Int8:
            bytes_per_sample = sizeof(int8_t);
            break;

        case SignalSampleType::Int16:
            bytes_per_sample = sizeof(int16_t);
            break;

        case SignalSampleType::Int32:
            bytes_per_sample = sizeof(int32_t);
            break;

        case SignalSampleType::Float32:
            bytes_per_sample = sizeof(float);
            break;

        default:
            return kErrUnsupportedSampleType;
    }

    // Calculate allocation size safely.
    const auto max_size = std::numeric_limits<size_t>::max();

    const auto channels = static_cast<size_t>(channel_count);

    if (std::cmp_greater(sample_count, max_size)) {
        return kErrInvalidConfiguration;
    }

    const auto samples = static_cast<size_t>(sample_count);

    if (samples > max_size / channels) {
        return kErrInvalidConfiguration;
    }

    const size_t total_samples = samples * channels;

    if (total_samples > max_size /
                            static_cast<size_t>(bytes_per_sample)) {
        return kErrInvalidConfiguration;
    }

    const size_t data_byte_size =
        total_samples * static_cast<size_t>(bytes_per_sample);

    // Allocate new storage without modifying the current configuration.
    SignalSamplePtr new_data;

    switch (sample_type) {
        case SignalSampleType::Int8:
            new_data.i8 = new (std::nothrow) int8_t[total_samples]{};
            break;

        case SignalSampleType::Int16:
            new_data.i16 = new (std::nothrow) int16_t[total_samples]{};
            break;

        case SignalSampleType::Int32:
            new_data.i32 = new (std::nothrow) int32_t[total_samples]{};
            break;

        case SignalSampleType::Float32:
            new_data.f32 = new (std::nothrow) float[total_samples]{};
            break;

        default:
            return kErrUnsupportedSampleType;
    }

    if (new_data.raw == nullptr) {
        return kErrAllocationFailed;
    }

    // Replace existing storage only after successful allocation.
    free();

    data_ = new_data;

    sample_type_ = sample_type;
    sample_rate_ = sample_rate;
    channel_count_ = channel_count;
    sample_count_ = sample_count;

    bits_per_sample_ = bytes_per_sample * 8;
    bytes_per_sample_ = bytes_per_sample;

    data_byte_size_ = data_byte_size;
    last_sample_index_ = sample_count - 1;

    return kErrNone;
}


auto SignalData::growIfNeeded(
    int64_t sample_count
) -> int32_t {

    // Validate current configuration.
    if (!isConfigured() || !hasData() || sample_count < 0) {
        return kErrInvalidConfiguration;
    }

    // Nothing to do if the current allocation is sufficient.
    if (sample_count <= sample_count_) {
        return kErrNone;
    }

    // Validate sample type.
    if (bytes_per_sample_ <= 0) {
        return kErrUnsupportedSampleType;
    }

    // Calculate allocation size safely.
    const auto max_size = std::numeric_limits<size_t>::max();

    const auto channels = static_cast<size_t>(channel_count_);

    if (std::cmp_greater(sample_count, max_size)) {
        return kErrInvalidConfiguration;
    }

    const auto samples = static_cast<size_t>(sample_count);

    if (samples > max_size / channels) {
        return kErrInvalidConfiguration;
    }

    const size_t total_samples = samples * channels;

    const auto bytes_per_sample =
        static_cast<size_t>(bytes_per_sample_);

    if (total_samples > max_size / bytes_per_sample) {
        return kErrInvalidConfiguration;
    }

    const size_t new_data_byte_size =
        total_samples * bytes_per_sample;

    // Calculate the number of existing interleaved samples.
    const auto old_total_samples =
        static_cast<size_t>(sample_count_) * channels;

    // Allocate new storage.
    SignalSamplePtr new_data;

    switch (sample_type_) {
        case SignalSampleType::Int8:
            new_data.i8 = new (std::nothrow)
                int8_t[total_samples]{};
            break;

        case SignalSampleType::Int16:
            new_data.i16 = new (std::nothrow)
                int16_t[total_samples]{};
            break;

        case SignalSampleType::Int32:
            new_data.i32 = new (std::nothrow)
                int32_t[total_samples]{};
            break;

        case SignalSampleType::Float32:
            new_data.f32 = new (std::nothrow)
                float[total_samples]{};
            break;

        default:
            return kErrUnsupportedSampleType;
    }

    if (new_data.raw == nullptr) {
        return kErrAllocationFailed;
    }

    // Copy existing interleaved samples.
    switch (sample_type_) {
        case SignalSampleType::Int8:
            std::copy_n(data_.i8, old_total_samples, new_data.i8);
            delete[] data_.i8;
            break;

        case SignalSampleType::Int16:
            std::copy_n(data_.i16, old_total_samples, new_data.i16);
            delete[] data_.i16;
            break;

        case SignalSampleType::Int32:
            std::copy_n(data_.i32, old_total_samples, new_data.i32);
            delete[] data_.i32;
            break;

        case SignalSampleType::Float32:
            std::copy_n(data_.f32, old_total_samples, new_data.f32);
            delete[] data_.f32;
            break;

        default:
            return kErrUnsupportedSampleType;
    }

    // Commit the new allocation.
    data_ = new_data;

    sample_count_ = sample_count;
    last_sample_index_ = sample_count - 1;
    data_byte_size_ = new_data_byte_size;

    return kErrNone;
}


//------------------------------------------------------------------------------
//  Memory management.
//------------------------------------------------------------------------------

void SignalData::clear() noexcept {

    if (!hasData()) {
        return;
    }

    const auto total_samples =
        static_cast<size_t>(sample_count_) *
        static_cast<size_t>(channel_count_);

    switch (sample_type_) {
    case SignalSampleType::Int8:
        std::fill_n(data_.i8, total_samples, int8_t{0});
        break;

    case SignalSampleType::Int16:
        std::fill_n(data_.i16, total_samples, int16_t{0});
        break;

    case SignalSampleType::Int32:
        std::fill_n(data_.i32, total_samples, int32_t{0});
        break;

    case SignalSampleType::Float32:
        std::fill_n(data_.f32, total_samples, 0.0f);
        break;

    default:
        break;
    }
}


void SignalData::free() noexcept {

    switch (sample_type_) {
        case SignalSampleType::Int8:
            delete[] data_.i8;
            break;

        case SignalSampleType::Int16:
            delete[] data_.i16;
            break;

        case SignalSampleType::Int32:
            delete[] data_.i32;
            break;

        case SignalSampleType::Float32:
            delete[] data_.f32;
            break;

        default:
            break;
    }

    data_.raw = nullptr;

    sample_type_ = SignalSampleType::Undefined;

    sample_rate_ = 0;
    channel_count_ = 0;
    sample_count_ = 0;

    bits_per_sample_ = 0;
    bytes_per_sample_ = 0;

    data_byte_size_ = 0;
    last_sample_index_ = 0;
}


//------------------------------------------------------------------------------
//  Sample storage.
//------------------------------------------------------------------------------

auto SignalData::dataPtr(
    int32_t channel,
    int64_t index
) const noexcept -> const void* {

    if (!hasSampleAtChannel(channel, index)) {
        return nullptr;
    }

    const auto offset =
        static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_) +
        static_cast<size_t>(channel);

    switch (sample_type_) {
    case SignalSampleType::Int8:
        return &data_.i8[offset];

    case SignalSampleType::Int16:
        return &data_.i16[offset];

    case SignalSampleType::Int32:
        return &data_.i32[offset];

    case SignalSampleType::Float32:
        return &data_.f32[offset];

    default:
        return nullptr;
    }
}


auto SignalData::mutDataPtr(
    int32_t channel,
    int64_t index
) noexcept -> void* {

    if (!hasSampleAtChannel(channel, index)) {
        return nullptr;
    }

    const auto offset =
        static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_) +
        static_cast<size_t>(channel);

    switch (sample_type_) {
    case SignalSampleType::Int8:
        return &data_.i8[offset];

    case SignalSampleType::Int16:
        return &data_.i16[offset];

    case SignalSampleType::Int32:
        return &data_.i32[offset];

    case SignalSampleType::Float32:
        return &data_.f32[offset];

    default:
        return nullptr;
    }
}


//------------------------------------------------------------------------------
//  Typed sample access.
//------------------------------------------------------------------------------

auto SignalData::readInt8(
    int32_t channel,
    int64_t index
) const -> int8_t {

    if (!hasSampleAtChannel(channel, index)) {
        return 0;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    return readSample<int8_t>(sample_type_, data_, sample_index);
}


auto SignalData::readInt16(
    int32_t channel,
    int64_t index
) const -> int16_t {

    if (!hasSampleAtChannel(channel, index)) {
        return 0;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    return readSample<int16_t>(sample_type_, data_, sample_index);
}


auto SignalData::readInt32(
    int32_t channel,
    int64_t index
) const -> int32_t {

    if (!hasSampleAtChannel(channel, index)) {
        return 0;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    return readSample<int32_t>(sample_type_, data_, sample_index);
}


auto SignalData::readFloat(
    int32_t channel,
    int64_t index
) const -> float {

    if (!hasSampleAtChannel(channel, index)) {
        return 0.0F;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    return readSample<float>(sample_type_, data_, sample_index);
}


auto SignalData::readDouble(
    int32_t channel,
    int64_t index
) const -> double {

    if (!hasSampleAtChannel(channel, index)) {
        return 0.0;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    return readSample<double>(sample_type_, data_, sample_index);
}


//------------------------------------------------------------------------------
//  Typed sample writing.
//------------------------------------------------------------------------------

void SignalData::writeInt8(
    int32_t channel,
    int64_t index,
    int8_t value
) {

    if (!hasSampleAtChannel(channel, index)) {
        return;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    writeSample(sample_type_, data_, sample_index, value);
}


void SignalData::writeInt16(
    int32_t channel,
    int64_t index,
    int16_t value
) {

    if (!hasSampleAtChannel(channel, index)) {
        return;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    writeSample(sample_type_, data_, sample_index, value);
}


void SignalData::writeInt32(
    int32_t channel,
    int64_t index,
    int32_t value
) {

    if (!hasSampleAtChannel(channel, index)) {
        return;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    writeSample(sample_type_, data_, sample_index, value);
}


void SignalData::writeFloat(
    int32_t channel,
    int64_t index,
    float value
) {

    if (!hasSampleAtChannel(channel, index)) {
        return;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    writeSample(sample_type_, data_, sample_index, value);
}


void SignalData::writeDouble(
    int32_t channel,
    int64_t index,
    double value
) {

    if (!hasSampleAtChannel(channel, index)) {
        return;
    }

    const auto sample_index =
        (static_cast<size_t>(index) *
        static_cast<size_t>(channel_count_)) +
        static_cast<size_t>(channel);

    writeSample(sample_type_, data_, sample_index, value);
}


//------------------------------------------------------------------------------
//  Utilities.
//------------------------------------------------------------------------------

auto SignalData::hasSameFormatAs(
    const SignalData& other
) const noexcept -> bool {

    return sample_type_ == other.sample_type_ &&
           channel_count_ == other.channel_count_ &&
           sample_rate_ == other.sample_rate_;
}


//------------------------------------------------------------------------------
//  Swap.
//------------------------------------------------------------------------------

void SignalData::swap(SignalData& other) noexcept {
    using std::swap;

    swap(sample_type_, other.sample_type_);
    swap(sample_rate_, other.sample_rate_);
    swap(channel_count_, other.channel_count_);
    swap(sample_count_, other.sample_count_);

    swap(data_.raw, other.data_.raw);

    swap(bits_per_sample_, other.bits_per_sample_);
    swap(bytes_per_sample_, other.bytes_per_sample_);

    swap(data_byte_size_, other.data_byte_size_);
    swap(last_sample_index_, other.last_sample_index_);
}

} // namespace Grain
