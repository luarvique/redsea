/*
 * Copyright (c) Oona Räisänen
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
#ifndef DSP_SUBCARRIER_H_
#define DSP_SUBCARRIER_H_

#include <array>
#include <complex>

#include "src/common.h"
#include "src/dsp/liquid_wrappers.h"
#include "src/input.h"

namespace redsea {

class BiphaseDecoder {
 public:
  BiphaseDecoder() = default;
  Maybe<bool> push(std::complex<float> psk_symbol);

 private:
  std::complex<float> prev_psk_symbol_{0.0f, 0.0f};
  std::array<float, 128> clock_history_{};
  uint32_t clock_{};
  uint32_t clock_polarity_{};
};

class DeltaDecoder {
 public:
  DeltaDecoder() = default;
  unsigned decode(unsigned d);

 private:
  unsigned prev_{0};
};

class Subcarrier {
 public:
  explicit Subcarrier(float carrier_frequency, float samplerate);
  bool eof() const;
  BitBuffer processChunk(MPXBuffer& input_chunk);
  void reset();
  float getSecondsSinceLastReset() const;

 private:
  // Samples since last reset
  std::uint32_t sample_num_{0};
  const float resample_ratio_;

  liquid::FIRFilter fir_lpf_;
  liquid::AGC agc_;
  liquid::NCO oscillator_;
  liquid::SymSync symsync_;
  liquid::Modem modem_;
  liquid::Resampler resampler_;

  MPXBuffer resampled_chunk_{};

  bool is_eof_{false};

  DeltaDecoder delta_decoder_;
  BiphaseDecoder biphase_decoder_;

  std::complex<float> prev_sym_;
};

}  // namespace redsea

#endif  // DSP_SUBCARRIER_H_
