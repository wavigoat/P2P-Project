
#ifndef MESSAGES_H
#define MESSAGES_H

#include "sharedHeaders.h"
#include <cstddef>
#include <cstdint>
#include <vector>

struct Message {
    MessageType type;
    std::vector<uint8_t> payload;
};

std::vector<uint8_t> encodeMessage(const Message& message);

Message decodeMessage(const std::vector<uint8_t>& frame);

Message makeIndexMessage(MessageType type, uint32_t pieceIndex);

uint32_t readPieceIndex(const Message& message);

Message makePieceMessage(uint32_t pieceIndex, const std::vector<uint8_t>& data);

std::vector<uint8_t> readPieceData(const Message& message);

#endif
