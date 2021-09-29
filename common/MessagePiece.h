//
// Created by Yi Lu on 8/30/18.
//

#pragma once

#include "common/StringPiece.h"

namespace star {

/*
 * MessagePiece header format
 *
 * | Message type (7 => 128) | Message length (12 => 4096) | table id (5 => 32)
 * | partition id (8 => 256) |
 *
 * Note that, the header is included in the message length.
 */

class MessagePiece {

public:
  using header_type = uint64_t;

  MessagePiece(const MessagePiece &messagePiece)
      : stringPiece(messagePiece.stringPiece) {}

  MessagePiece(const StringPiece &stringPiece) : stringPiece(stringPiece) {}

  uint32_t get_message_type() const {
    return (get_header() >> MESSAGE_TYPE_OFFSET) & MESSAGE_TYPE_MASK;
  }

  uint32_t get_message_length() const {
    return (get_header() >> MESSAGE_LENGTH_OFFSET) & MESSAGE_LENGTH_MASK;
  }

  uint32_t get_table_id() const {
    return (get_header() >> TABLE_ID_OFFSET) & TABLE_ID_MASK;
  }

  uint32_t get_partition_id() const {
    return (get_header() >> PARTITION_ID_OFFSET) & PARTITION_ID_MASK;
  }

  StringPiece toStringPiece() {
    return StringPiece(stringPiece.data() + get_header_size(),
                       get_message_length() - get_header_size());
  }

  bool operator==(const MessagePiece &that) const {
    return stringPiece == that.stringPiece;
  }

  bool operator!=(const MessagePiece &that) const {
    return stringPiece != that.stringPiece;
  }

private:
  header_type get_header() const {
    return *reinterpret_cast<const header_type *>(stringPiece.data());
  }

public:
  StringPiece stringPiece;

public:
  static uint32_t get_header_size() { return sizeof(header_type); }

  static header_type construct_message_piece_header(uint32_t message_type,
                                                 uint32_t message_length,
                                                 std::size_t table_id,
                                                 std::size_t partition_id) {
    DCHECK(message_type < (1ull << 7));
    DCHECK(message_length < (1ull << 24));
    DCHECK(table_id < (1ull << 5));
    DCHECK(partition_id < (1ull << 32));

    return (((uint64_t)message_type) << MESSAGE_TYPE_OFFSET) +
           (((uint64_t)message_length) << MESSAGE_LENGTH_OFFSET) +
           (((uint64_t)table_id) << TABLE_ID_OFFSET) +
           (((uint64_t)partition_id) << PARTITION_ID_OFFSET);
  }

  static constexpr uint32_t get_message_length(header_type header) {
    return (header >> MESSAGE_LENGTH_OFFSET) & MESSAGE_LENGTH_MASK;
  }

public:
  static constexpr uint64_t MESSAGE_TYPE_MASK = 0x7f;
  static constexpr uint64_t MESSAGE_TYPE_OFFSET = 25+24+8;
  static constexpr uint64_t MESSAGE_LENGTH_MASK = 0xfffff;
  static constexpr uint64_t MESSAGE_LENGTH_OFFSET = 13+24;
  static constexpr uint64_t TABLE_ID_MASK = 0x1f;
  static constexpr uint64_t TABLE_ID_OFFSET = 32;
  static constexpr uint64_t PARTITION_ID_MASK = 0xffffffff;
  static constexpr uint64_t PARTITION_ID_OFFSET = 0;
};
} // namespace star
