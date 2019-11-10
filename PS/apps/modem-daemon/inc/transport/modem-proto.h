#ifndef MODEMPROTO_H
#define MODEMPROTO_H

#include <iostream>
#include <memory>
#include <vector>

#define START_TOKEN     (0xAA55CC33)

using block_t = std::vector<uint8_t>;

struct packetCommonHeader_t
{
    uint32_t startToken;
    uint16_t length;                // длина всего пакета
    enum class type_c : uint8_t {
        RTS, ACK, DATA
    } type;
    uint8_t reserved;
} __attribute__((__packed__));

struct packetRTSHeader_t
{
    uint16_t totalPacketsCount;
} __attribute__((__packed__));

struct packetAckHeader_t
{
    uint16_t needPacketsCount;      // количество "1" битов в маске запроса. 0 - все пакеты переданы успешно. FIXME: пока бит->байт
} __attribute__((__packed__));

struct packetDataHeader_t
{
    uint16_t number;
} __attribute__((__packed__));

#endif // MODEMPROTO_H
