//
// FPort Protocol class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl_FPort.H>
#include <cstddef>
#include <cstdint>
#include <cstring>

Fl_FPort::Fl_FPort() : fport_cb_(nullptr), fport_user_data_(nullptr),
                       channels_{}, buffer_{}, state_(WAIT_SYNC),
                       frame_length_(0U), frame_type_(0U), buf_idx_(0U), flags_(0U),
                       rssi_(0U), escape_next_(false) {
  // Register the internal serial callback
  callback(serial_cb, this);
}

Fl_FPort::~Fl_FPort() = default;

int Fl_FPort::open(const char* const port_name) {
  int result = -1;

  if (Fl_Serial_Port::open(port_name) == 0) {
    // Configure for FPort: 115200 baud, 8N1
    static_cast<void>(set_baud_rate(115200));
    static_cast<void>(set_data_bits(DATA_8));
    static_cast<void>(set_parity(PARITY_NONE));
    static_cast<void>(set_stop_bits(STOP_1));
    result = 0;
  }

  return result;
}

void Fl_FPort::fport_callback(const Fl_FPort_Callback cb, void* const user_data) {
  fport_cb_ = cb;
  fport_user_data_ = user_data;
}

uint16_t Fl_FPort::channel(const int ch) const {
  uint16_t val = 0U;

  if ((ch >= 1) && (ch <= 16)) {
    const size_t idx = static_cast<size_t>(static_cast<unsigned int>(ch) - 1U);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    val = channels_[idx];
  }

  return val;
}

void Fl_FPort::serial_cb(Fl_Serial_Port* const p, void* const data) {
  if ((p != nullptr) && (data != nullptr)) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    auto* const fport = static_cast<Fl_FPort*>(data);
    uint8_t buf[256] = {};
    const int bytes_read = p->read_data(&buf[0], sizeof(buf));

    if (bytes_read > 0) {
      const size_t count = static_cast<size_t>(static_cast<unsigned int>(bytes_read));
      for (size_t i = 0U; i < count; i++) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        fport->process_byte(buf[i]);
      }
    }
  }
}

void Fl_FPort::process_byte(uint8_t b) {
  if (b == 0x7EU) {
    // Start of frame
    state_ = WAIT_LENGTH;
    escape_next_ = false;
  } else if (state_ == WAIT_SYNC) {
    // Drop byte while waiting for sync frame
  } else if (b == 0x7DU) {
    // Handle byte stuffing flag
    escape_next_ = true;
  } else {
    if (escape_next_) {
      b ^= 0x20U;
      escape_next_ = false;
    }

    switch (state_) {
      case WAIT_LENGTH:
        frame_length_ = b;
        state_ = WAIT_TYPE;
        break;

      case WAIT_TYPE:
        frame_type_ = b;
        buf_idx_ = 0U;
        if (frame_length_ > 1U) { // -1 for type
          state_ = WAIT_PAYLOAD;
        } else {
          state_ = WAIT_SYNC;
        }
        break;

      case WAIT_PAYLOAD:
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        buffer_[buf_idx_] = b;
        buf_idx_++;

        // Expected payload length: frame_length - 1 (type) + 1 (checksum)
        if (buf_idx_ >= static_cast<size_t>(frame_length_)) {
          // Full packet received, check checksum
          auto sum = static_cast<uint16_t>(static_cast<uint16_t>(frame_length_) + static_cast<uint16_t>(frame_type_));

          const size_t payload_len = static_cast<size_t>(frame_length_ - 1U);
          for (size_t i = 0U; i < payload_len; i++) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            sum = static_cast<uint16_t>(sum + static_cast<uint16_t>(buffer_[i]));
          }

          // FrSky Checksum logic
          sum = static_cast<uint16_t>(sum + static_cast<uint16_t>(sum >> 8U));
          sum &= 0x00FFU;
          const auto checksum = static_cast<uint8_t>(0xFFU - static_cast<uint8_t>(sum));

          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
          const uint8_t received_checksum = buffer_[static_cast<size_t>(frame_length_ - 1U)];

          if (checksum == received_checksum) {
            if ((frame_type_ == 0x00U) && (frame_length_ >= 25U)) { // Control (RC) frame
              decode_rc_frame();
              if (fport_cb_ != nullptr) {
                fport_cb_(this, fport_user_data_);
              }
            }
          }

          state_ = WAIT_SYNC;
        }
        break;

      default:
        state_ = WAIT_SYNC;
        break;
    }
  }
}

void Fl_FPort::decode_rc_frame() {
  const auto b = [this](const size_t idx) -> uint32_t {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return static_cast<uint32_t>(buffer_[idx]);
  };

  // FPort packs SBUS channels identically to SBUS (11 bits per channel)
  channels_[0]  = static_cast<uint16_t>((b(0U)          | (b(1U) << 8U))                  & 0x07FFU);
  channels_[1]  = static_cast<uint16_t>(((b(1U) >> 3U)  | (b(2U) << 5U))                  & 0x07FFU);
  channels_[2]  = static_cast<uint16_t>(((b(2U) >> 6U)  | (b(3U) << 2U)  | (b(4U) << 10U)) & 0x07FFU);
  channels_[3]  = static_cast<uint16_t>(((b(4U) >> 1U)  | (b(5U) << 7U))                  & 0x07FFU);
  channels_[4]  = static_cast<uint16_t>(((b(5U) >> 4U)  | (b(6U) << 4U))                  & 0x07FFU);
  channels_[5]  = static_cast<uint16_t>(((b(6U) >> 7U)  | (b(7U) << 1U)  | (b(8U) << 9U))  & 0x07FFU);
  channels_[6]  = static_cast<uint16_t>(((b(8U) >> 2U)  | (b(9U) << 6U))                  & 0x07FFU);
  channels_[7]  = static_cast<uint16_t>(((b(9U) >> 5U)  | (b(10U) << 3U))                 & 0x07FFU);
  channels_[8]  = static_cast<uint16_t>((b(11U)         | (b(12U) << 8U))                 & 0x07FFU);
  channels_[9]  = static_cast<uint16_t>(((b(12U) >> 3U) | (b(13U) << 5U))                 & 0x07FFU);
  channels_[10] = static_cast<uint16_t>(((b(13U) >> 6U) | (b(14U) << 2U) | (b(15U) << 10U)) & 0x07FFU);
  channels_[11] = static_cast<uint16_t>(((b(15U) >> 1U) | (b(16U) << 7U))                 & 0x07FFU);
  channels_[12] = static_cast<uint16_t>(((b(16U) >> 4U) | (b(17U) << 4U))                 & 0x07FFU);
  channels_[13] = static_cast<uint16_t>(((b(17U) >> 7U) | (b(18U) << 1U) | (b(19U) << 9U))  & 0x07FFU);
  channels_[14] = static_cast<uint16_t>(((b(19U) >> 2U) | (b(20U) << 6U))                 & 0x07FFU);
  channels_[15] = static_cast<uint16_t>(((b(20U) >> 5U) | (b(21U) << 3U))                 & 0x07FFU);

  // SBUS Flags
  flags_ = buffer_[22];

  // RSSI
  rssi_ = buffer_[23];
}