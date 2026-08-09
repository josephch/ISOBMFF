/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 DigiDNA - www.digidna.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        LoudnessBaseBox.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <LoudnessBaseBox.hpp>

namespace ISOBMFF {

struct Measurement {
  uint8_t method_definition;
  uint8_t method_value;
  uint8_t measurement_system;
  uint8_t reliability;
};

struct LoundnessBase {
  uint8_t EQ_set_ID;
  uint8_t downmix_ID;  // matching downmix
  uint8_t DRC_set_ID;  // to match a DRC box
  uint16_t bs_sample_peak_level;
  uint16_t bs_true_peak_level;
  uint8_t measurement_system_for_TP;
  uint8_t reliability_for_TP;
  uint8_t measurement_count;
  std::vector<Measurement> measurements;
};

class LoudnessBaseBox::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();
  uint8_t loudness_base_count;

  std::vector<LoundnessBase> loudnessBases;
};

LoudnessBaseBox::LoudnessBaseBox(const std::string& name)
    : FullBox(name), impl(std::make_unique<IMPL>()) {}

LoudnessBaseBox::LoudnessBaseBox(const LoudnessBaseBox& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

LoudnessBaseBox::LoudnessBaseBox(LoudnessBaseBox&& o) noexcept
    : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

LoudnessBaseBox::~LoudnessBaseBox() {}

LoudnessBaseBox& LoudnessBaseBox::operator=(LoudnessBaseBox o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(LoudnessBaseBox& o1, LoudnessBaseBox& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error LoudnessBaseBox::ReadData(Parser& parser, BinaryStream& stream) {
  uint8_t u8;
  uint16_t u16;
  Error err;

  err = FullBox::ReadData(parser, stream);
  if (err) return err;

  if (this->GetVersion() >= 1) {
    err = stream.ReadUInt8(u8);
    if (err) return err;
    impl->loudness_base_count = (u8 & 0x3F);
  } else {
    impl->loudness_base_count = 1;
  }
  for (size_t i = 0; i < impl->loudness_base_count; i++) {
    LoundnessBase loudnessBase;
    err = stream.ReadUInt8(u8);
    if (err) return err;
    loudnessBase.EQ_set_ID = (u8 & 0x3F);
    err = stream.ReadUInt8(u8);
    if (err) return err;
    loudnessBase.downmix_ID = (u8 & 0x1F) << 2;

    err = stream.ReadUInt8(u8);
    if (err) return err;
    loudnessBase.downmix_ID |= ((u8 & 0xC0) >> 6);
    loudnessBase.DRC_set_ID = (u8 & 0x3F);

    err = stream.ReadBigEndianUInt16(u16);
    if (err) return err;
    loudnessBase.bs_sample_peak_level = u16 >> 4;
    loudnessBase.bs_true_peak_level = (u16 & 0xF) << 8;

    err = stream.ReadUInt8(u8);
    if (err) return err;
    loudnessBase.bs_true_peak_level |= u8;

    err = stream.ReadUInt8(u8);
    if (err) return err;
    loudnessBase.measurement_system_for_TP = (u8 & 0xF0) >> 4;
    loudnessBase.reliability_for_TP = (u8 & 0x0F);

    err = stream.ReadUInt8(u8);
    if (err) return err;
    loudnessBase.measurement_count = u8;

    for (size_t i = 0; i < loudnessBase.measurement_count; i++) {
      Measurement item;
      err = stream.ReadUInt8(u8);
      if (err) return err;
      item.method_definition = u8;

      err = stream.ReadUInt8(u8);
      if (err) return err;
      item.method_value = u8;

      err = stream.ReadUInt8(u8);
      if (err) return err;
      item.measurement_system = (u8 & 0xF0) >> 4;
      item.reliability = (u8 & 0x0F);

      loudnessBase.measurements.push_back(item);
    }
    impl->loudnessBases.push_back(loudnessBase);
  }
  return err;
}

std::vector<std::pair<std::string, std::string> >
LoudnessBaseBox::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());
  size_t idx = 0;
  for (const LoundnessBase& loudnessBase : impl->loudnessBases) {
    props.push_back({"LoundnessBase Idx", std::to_string(idx)});
    props.push_back({"  EQ_set_ID", std::to_string(loudnessBase.EQ_set_ID)});
    props.push_back({"  downmix_ID", std::to_string(loudnessBase.downmix_ID)});
    props.push_back({"  DRC_set_ID", std::to_string(loudnessBase.DRC_set_ID)});
    props.push_back({"  bs_sample_peak_level",
                     std::to_string(loudnessBase.bs_sample_peak_level)});
    props.push_back({"  bs_true_peak_level",
                     std::to_string(loudnessBase.bs_true_peak_level)});
    props.push_back({"  measurement_system_for_TP",
                     std::to_string(loudnessBase.measurement_system_for_TP)});
    props.push_back({"  reliability_for_TP",
                     std::to_string(loudnessBase.reliability_for_TP)});
    props.push_back({"  measurement_count",
                     std::to_string(loudnessBase.measurement_count)});
    size_t measurementIdx = 0;
    for (const Measurement& measurement : loudnessBase.measurements) {
      props.push_back({"  Measurement Idx", std::to_string(measurementIdx)});
      props.push_back({"    method_definition",
                       std::to_string(measurement.method_definition)});
      props.push_back(
          {"    method_value", std::to_string(measurement.method_value)});
      props.push_back({"    measurement_system",
                       std::to_string(measurement.measurement_system)});
      props.push_back(
          {"    reliability", std::to_string(measurement.reliability)});
      measurementIdx++;
    }
    idx++;
  }
  return props;
}

LoudnessBaseBox::IMPL::IMPL() : loudness_base_count{0} {}
LoudnessBaseBox::IMPL::~IMPL() = default;
LoudnessBaseBox::IMPL::IMPL(const IMPL& o) = default;
}  // namespace ISOBMFF
