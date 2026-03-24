#pragma once

#include "ipi/api/sender.hpp"
#include "ipi/api/types.hpp"
#include "ipi/core/message_frame.hpp"
#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ipi::api {

template <typename MessageT>
struct J2735PayloadTraits;

template <>
struct J2735PayloadTraits<j2735::BasicSafetyMessage> {
    static constexpr J2735MessageType apiType = J2735MessageType::BSM;
    static constexpr MessageType frameType = MessageType::BSM;

    static std::vector<std::uint8_t> encode_uper(const v2x::UperCodec& codec,
                                                 const j2735::BasicSafetyMessage& message) {
        return codec.encode(message);
    }

    static j2735::BasicSafetyMessage decode_uper(const v2x::UperCodec& codec,
                                                 const std::vector<std::uint8_t>& payload) {
        return codec.decode_bsm(payload);
    }

    static std::vector<std::uint8_t> encode_bytes(const j2735::BasicSafetyMessage& message) {
        return message.to_bytes();
    }

    static j2735::BasicSafetyMessage decode_bytes(const std::vector<std::uint8_t>& payload) {
        return j2735::BasicSafetyMessage::from_bytes(payload);
    }
};

template <>
struct J2735PayloadTraits<j2735::MapMessage> {
    static constexpr J2735MessageType apiType = J2735MessageType::MAP;
    static constexpr MessageType frameType = MessageType::MAP;

    static std::vector<std::uint8_t> encode_uper(const v2x::UperCodec& codec,
                                                 const j2735::MapMessage& message) {
        return codec.encode(message);
    }

    static j2735::MapMessage decode_uper(const v2x::UperCodec& codec,
                                         const std::vector<std::uint8_t>& payload) {
        return codec.decode_map(payload);
    }

    static std::vector<std::uint8_t> encode_bytes(const j2735::MapMessage& message) {
        return message.to_bytes();
    }

    static j2735::MapMessage decode_bytes(const std::vector<std::uint8_t>& payload) {
        return j2735::MapMessage::from_bytes(payload);
    }
};

template <>
struct J2735PayloadTraits<j2735::SpatMessage> {
    static constexpr J2735MessageType apiType = J2735MessageType::SPAT;
    static constexpr MessageType frameType = MessageType::SPAT;

    static std::vector<std::uint8_t> encode_uper(const v2x::UperCodec& codec,
                                                 const j2735::SpatMessage& message) {
        return codec.encode(message);
    }

    static j2735::SpatMessage decode_uper(const v2x::UperCodec& codec,
                                          const std::vector<std::uint8_t>& payload) {
        return codec.decode_spat(payload);
    }

    static std::vector<std::uint8_t> encode_bytes(const j2735::SpatMessage& message) {
        return message.to_bytes();
    }

    static j2735::SpatMessage decode_bytes(const std::vector<std::uint8_t>& payload) {
        return j2735::SpatMessage::from_bytes(payload);
    }
};

template <>
struct J2735PayloadTraits<j2735::SignalRequestMessage> {
    static constexpr J2735MessageType apiType = J2735MessageType::SRM;
    static constexpr MessageType frameType = MessageType::SRM;

    static std::vector<std::uint8_t> encode_uper(const v2x::UperCodec& codec,
                                                 const j2735::SignalRequestMessage& message) {
        return codec.encode(message);
    }

    static j2735::SignalRequestMessage decode_uper(const v2x::UperCodec& codec,
                                                   const std::vector<std::uint8_t>& payload) {
        return codec.decode_srm(payload);
    }

    static std::vector<std::uint8_t> encode_bytes(const j2735::SignalRequestMessage& message) {
        return message.to_bytes();
    }

    static j2735::SignalRequestMessage decode_bytes(const std::vector<std::uint8_t>& payload) {
        return j2735::SignalRequestMessage::from_bytes(payload);
    }
};

template <>
struct J2735PayloadTraits<j2735::SignalStatusMessage> {
    static constexpr J2735MessageType apiType = J2735MessageType::SSM;
    static constexpr MessageType frameType = MessageType::SSM;

    static std::vector<std::uint8_t> encode_uper(const v2x::UperCodec& codec,
                                                 const j2735::SignalStatusMessage& message) {
        return codec.encode(message);
    }

    static j2735::SignalStatusMessage decode_uper(const v2x::UperCodec& codec,
                                                  const std::vector<std::uint8_t>& payload) {
        return codec.decode_ssm(payload);
    }

    static std::vector<std::uint8_t> encode_bytes(const j2735::SignalStatusMessage& message) {
        return message.to_bytes();
    }

    static j2735::SignalStatusMessage decode_bytes(const std::vector<std::uint8_t>& payload) {
        return j2735::SignalStatusMessage::from_bytes(payload);
    }
};

template <typename MessageT>
J2735Payload pack_j2735_payload(const MessageT& message,
                                const v2x::UperCodec& codec,
                                J2735Encoding encoding = J2735Encoding::UPER,
                                std::optional<std::uint32_t> frameCounter = {}) {
    static_assert(!std::is_const_v<MessageT>, "MessageT should not be cv-qualified");
    J2735Payload payload;
    payload.type = J2735PayloadTraits<MessageT>::apiType;
    payload.encoding = encoding;
    payload.frameCounter = frameCounter;

    switch (encoding) {
        case J2735Encoding::UPER:
            payload.payload = J2735PayloadTraits<MessageT>::encode_uper(codec, message);
            break;
        case J2735Encoding::BYTES:
            payload.payload = J2735PayloadTraits<MessageT>::encode_bytes(message);
            break;
        case J2735Encoding::JSON:
        default:
            throw std::invalid_argument("JSON J2735 encoding is not implemented");
    }

    return payload;
}

template <typename MessageT>
MessageT unpack_j2735_payload(const J2735Payload& payload, const v2x::UperCodec& codec) {
    if (payload.type != J2735PayloadTraits<MessageT>::apiType) {
        throw std::invalid_argument("J2735Payload type does not match requested message type");
    }

    switch (payload.encoding) {
        case J2735Encoding::UPER:
            return J2735PayloadTraits<MessageT>::decode_uper(codec, payload.payload);
        case J2735Encoding::BYTES:
            return J2735PayloadTraits<MessageT>::decode_bytes(payload.payload);
        case J2735Encoding::JSON:
        default:
            throw std::invalid_argument("JSON J2735 encoding is not implemented");
    }
}

template <typename MessageT>
Envelope<J2735Payload> make_j2735_envelope(const EnvelopeMetadata& metadata,
                                           const MessageT& message,
                                           const v2x::UperCodec& codec,
                                           J2735Encoding encoding = J2735Encoding::UPER,
                                           std::optional<std::uint32_t> frameCounter = {}) {
    Envelope<J2735Payload> envelope;
    envelope.metadata = metadata;
    envelope.data = pack_j2735_payload(message, codec, encoding, frameCounter);
    return envelope;
}

template <typename MessageT>
MessageFrame make_message_frame(const MessageT& message,
                                const v2x::UperCodec& codec,
                                std::map<std::string, std::string> annotations = {}) {
    MessageFrame frame;
    frame.type = J2735PayloadTraits<MessageT>::frameType;
    frame.payload = J2735PayloadTraits<MessageT>::encode_uper(codec, message);
    frame.annotations = std::move(annotations);
    return frame;
}

template <typename MessageT>
MessageT unpack_message_frame(const MessageFrame& frame, const v2x::UperCodec& codec) {
    if (frame.type != J2735PayloadTraits<MessageT>::frameType) {
        throw std::invalid_argument("MessageFrame type does not match requested message type");
    }
    return J2735PayloadTraits<MessageT>::decode_uper(codec, frame.payload);
}

template <typename MessageT>
V2xQuery make_v2x_query(std::optional<Timestamp> since = {}, std::size_t limit = 50) {
    V2xQuery query;
    query.type = J2735PayloadTraits<MessageT>::apiType;
    query.since = since;
    query.limit = limit;
    return query;
}

} // namespace ipi::api
