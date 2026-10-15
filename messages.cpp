
#include "messages.h"
#include <limits>
#include <stdexcept>

namespace {

constexpr size_t MAX_PAYLOAD_BYTES = 16 * 1024 * 1024;

//convert 32-bit to 4 bytes in big-endian order
void appendU32(std::vector<uint8_t>& out, uint32_t value) {
    out.push_back(static_cast<uint8_t>(value >> 24));
    out.push_back(static_cast<uint8_t>(value >> 16));
    out.push_back(static_cast<uint8_t>(value >> 8));
    out.push_back(static_cast<uint8_t>(value));
}

uint32_t getU32(const uint8_t* bytes) { //convert bytes to 32-bit in big-endian order
    uint32_t result = 0;
    //shift result by 8 bits
    for (int i = 0; i < 4; i++) {
        result = (result << 8) | bytes[i]; //add next byte to result
    }

    return result;
}
//checks if message fllows protocol format
void validate(const Message& message) {
    size_t payloadSize = message.payload.size();
    MessageType type = message.type;
    //check if messages are larger than maximum payload size
    if (payloadSize > MAX_PAYLOAD_BYTES) {
        throw std::runtime_error("Payload is too large");
    }
    // Messages that should not contain any payload
    if (type == MessageType::CHOKE ||
        type == MessageType::UNCHOKE ||
        type == MessageType::INTERESTED ||
        type == MessageType::NOT_INTERESTED) {

        if (payloadSize != 0) {
            throw std::runtime_error("Unexpected payload");
        }
    }
    // Have and Request messages should contain a 4-byte piece index
    else if (type == MessageType::HAVE ||
             type == MessageType::REQUEST) {

        if (payloadSize != 4) {
            throw std::runtime_error("Invalid piece index size");
        }
    }
    // Bitfield message
    else if (type == MessageType::BITFIELD) {

    }
    // Piece contains 4-byte index followed by file data
    else if (type == MessageType::PIECE) {
        if (payloadSize < 5) { //needs atleast 5 bytes for index and data
            throw std::runtime_error("Invalid piece message");
        }
    }
    // Reject unknown message types
    else {
        throw std::runtime_error("Unknown message type");
    }
}

}
//convert message object into byte vector for transmission
std::vector<uint8_t> encodeMessage(const Message& message) {
    validate(message); //message needs to be validated before encoding
    // Check if the message can fit in a 32-bit length field
    if (message.payload.size() > std::numeric_limits<uint32_t>::max() - 1u)
        throw std::runtime_error("Message cannot fit a 32-bit length");

    std::vector<uint8_t> frame; //reserve memory for length, type, and payload
    frame.reserve(message.payload.size() + 5);
    appendU32(frame, static_cast<uint32_t>(message.payload.size() + 1)); //4-byte filed is not counted
    frame.push_back(static_cast<uint8_t>(message.type)); //add message as single byte
    frame.insert(frame.end(), message.payload.begin(), message.payload.end()); //copy payload bytes
    return frame;
}

//convert TCP message into Message object
Message decodeMessage(const std::vector<uint8_t>& frame) {

    // Make sure the message contains at least a length and type
    if (frame.size() < 5) {
        throw std::runtime_error("Message is too short");
    }

    // Read the message length from the first 4 bytes
    uint32_t messageLength = getU32(frame.data());

    // Check if the message length is valid
    if (messageLength == 0) {
        throw std::runtime_error("Invalid message length");
    }
    if (messageLength > MAX_PAYLOAD_BYTES + 1) {
        throw std::runtime_error("Message is too large");
    }
    if (messageLength != frame.size() - 4) {
        throw std::runtime_error("Message length does not match");
    }
    // Get the message type from the fifth byte
    MessageType type = static_cast<MessageType>(frame[4]);
    // Copy the remaining bytes into the payload
    std::vector<uint8_t> payload;

    for (size_t i = 5; i < frame.size(); i++) {
        payload.push_back(frame[i]);
    }
    // Create the decoded message
    Message message;
    message.type = type;
    message.payload = payload;

    validate(message);
    return message;
}


//create have or request message
Message makeIndexMessage(MessageType type, uint32_t pieceIndex) {
    if (type != MessageType::HAVE && type != MessageType::REQUEST) { //only have and request messages use this
        throw std::invalid_argument("Invalid message type");
    }
    Message message;
    message.type = type;
    //convert index to 4 bytes in big-endian order and add to payload
    int i = 3;
    while (i >= 0) {
        uint8_t byte = (pieceIndex >> (i * 8)) & 0xFF;
        message.payload.push_back(byte);
        i--;
    }
    return message;
}


//read 4 byte piece index from have, request, or piece message
uint32_t readPieceIndex(const Message& message) {
    if (message.type != MessageType::HAVE &&
        message.type != MessageType::REQUEST &&
        message.type != MessageType::PIECE) {
        throw std::invalid_argument("Invalid message type");
    }

    validate(message); //verify payload size is correct for message type

    uint32_t pieceIndex = 0;
    int i = 0;
    
    while (i < 4) {
        pieceIndex = (pieceIndex << 8) | message.payload[i]; //shift existing number left by 8 bits and add next byte
        i++;
    }

    return pieceIndex;
}

//create piece message
Message makePieceMessage(uint32_t pieceIndex, const std::vector<uint8_t>& data) {
    if (data.empty()) throw std::invalid_argument("Cannot construct empty piece"); //must contain at least 1 byte of data
    Message message{MessageType::PIECE, {}};
    appendU32(message.payload, pieceIndex); //store piece index as 4 bytes of payload
    message.payload.insert(message.payload.end(), data.begin(), data.end());
    validate(message); //check that message is valid before returning
    return message;
}
//extract file data from piece message
std::vector<uint8_t> readPieceData(const Message& message) {
    if (message.type != MessageType::PIECE) //used with piece messages only
        throw std::invalid_argument("Not a PIECE message");
    validate(message); //verify that message is valid before extracting data
    return {message.payload.begin() + 4, message.payload.end()};
}
