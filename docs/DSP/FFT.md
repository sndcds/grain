
# Fast Fourier Transform (FFT)

Grain provides a real-valued Fast Fourier Transform through
`Grain::FFT`.

The implementation uses a platform-independent public API with
selectable internal backends.

## Features

- Real-to-complex forward transforms.
- Complex-to-real inverse transforms.
- Configurable transform size.
- Automatic backend selection.
- Portable C++ implementation.
- Optional platform-optimized implementations.
- No temporary allocations during transforms.

## Transform conventions

For a transform size of N:

| Property | Size |
|---|---:|
| Real input samples | N |
| Complex frequency bins | N / 2 + 1 |
| Real inverse output samples | N |

The spectrum contains the non-redundant frequency bins of a real-valued
signal, starting at DC and ending at Nyquist for even transform sizes.

The forward transform is unnormalized.

The inverse transform is normalized by 1 / N.

The imaginary components of the DC and Nyquist bins are ignored during
the inverse transform.

## Backend selection

The backend is selected through `FFTConfiguration`.

### Automatic

`FFTBackendType::Automatic` selects the best available implementation
for the requested transform size.

Current selection order:

1. Accelerate, when available and compatible with the requested size.
2. Portable, as the fallback implementation.

If optimized backend initialization fails because of an allocation or
other resource error, that error is returned rather than silently
falling back.

### Portable

`FFTBackendType::Portable` selects the platform-independent C++
implementation.

It supports every positive transform size.

The implementation uses:

- Iterative radix-2 Cooley-Tukey FFT for power-of-two sizes.
- Direct DFT for other sizes.

The radix-2 implementation has O(N log N) time complexity.

The direct implementation has O(N²) time complexity.

### Accelerate

`FFTBackendType::Accelerate` selects Apple's Accelerate/vDSP implementation.

It is available on macOS and currently supports even transform sizes
greater than or equal to 2.

Explicitly requesting Accelerate on an unsupported platform or with an
unsupported size returns `Result::Unsupported`.

### FFTW

`FFTBackendType::FFTW` is reserved for a future FFTW implementation.

It currently returns `Result::Unsupported`.

## Memory management

Backend setup and working buffers are allocated during `FFT::create()`.

The current implementations reuse their working buffers during
transforms. Neither `forward()` nor `inverse()` allocates temporary
memory.

An FFT instance therefore owns its working memory for its entire lifetime.

## Thread safety

An individual FFT instance is not thread-safe for concurrent transforms.

Do not call `forward()` and `inverse()` concurrently on the same instance.

Separate FFT instances can be used independently on different threads.

## Usage

### Create an FFT instance

```cpp
#include <grain/DSP/FFT/FFT.hpp>

#include <complex>
#include <memory>
#include <vector>

using namespace Grain;

FFTConfiguration configuration;
configuration.size = 1024;
configuration.backend = FFTBackendType::Automatic;

std::unique_ptr<FFT> fft;

const Result result = FFT::create(configuration, fft);

if (result != Result::Success) {
    // Handle initialization error.
    return;
}
```

### Forward transform

```cpp
std::vector<float> samples(fft->size());
std::vector<std::complex<float>> spectrum(fft->binCount());

const Result result = fft->forward(samples, spectrum);

if (result != Result::Success) {
    // Handle transform error.
    return;
}
```

### Inverse transform

```cpp
std::vector<float> reconstructed(fft->size());

const Result result = fft->inverse(spectrum, reconstructed);

if (result != Result::Success) {
    // Handle transform error.
    return;
}
```

### Query the selected backend

```cpp
switch (fft->backend()) {
    case FFTBackendType::Portable:
        // Portable implementation.
        break;

    case FFTBackendType::Accelerate:
        // Apple Accelerate implementation.
        break;

    default:
        break;
}
```

## Performance considerations

For power-of-two transform sizes, the portable implementation uses
an O(N log N) radix-2 algorithm.

For arbitrary transform sizes, it currently uses a direct O(N²) DFT.

When performance is important, prefer power-of-two transform sizes
and use `FFTBackendType::Automatic` unless a specific backend is required.

The selected backend can be queried through `FFT::backend()`.
