#ifndef MODEMSENDER_H
#define MODEMSENDER_H

#include <ctime>

#include <cmath>
#include "unistd.h"
#include "modem-proto.h"
#include "string.h"
#include "tracers.h"

class modemSender_c
{
    const size_t    MAX_PACKET_SIZE = 1024;
    const uint16_t  MAX_DATA_SIZE = MAX_PACKET_SIZE - sizeof (packetCommonHeader_t) - sizeof (packetDataHeader_t);
    const long      MAX_WAIT_TIME = 5000;
    const uint8_t   MAX_ERRORS = 3;

    static std::string errorMessages[];

    enum class error_c : uint32_t
    {
        ERROR_ACK_TIMEOUT,
        ERROR_INCORRECT_LENGTH,
        ERROR_BAD_FORMAT,
        ERROR_BAD_TYPE
    };

    enum class state_c : uint32_t
    {
        MODEM_SENDER_IDLE,
        MODEM_SENDER_SEND_RTS,
        MODEM_SENDER_WAIT_ACK,
        MODEM_SENDER_SEND_DATA
    } m_state;
    bool m_bIsRTSAcked;
    block_t &m_blockToModem;                    // блок данных от клиента для отправки по радиоканалу
    std::string &m_strMessageToClient;
    uint16_t m_nTotalPackets, m_nCurrentIdx;
    long m_nTimeMs;
    uint8_t errCounters[10];
    std::unique_ptr<uint8_t[]> m_pReceivedPacketsMask;

public:
    modemSender_c(block_t &blockToModem, std::string &strMessage);
    void operator()();

private:
    long getCurrentTimeMs (void) noexcept;
    inline void stopwatchStart() noexcept { m_nTimeMs = getCurrentTimeMs(); }
    inline long stopwatchStop() noexcept { return getCurrentTimeMs() - m_nTimeMs; }
    size_t sendData() noexcept;
    size_t sendRTS() noexcept;
    size_t receive(std::unique_ptr<uint8_t[]> &pPacket) noexcept;
    void error(enum error_c err, const std::string &msg = "") noexcept;
    void reset() noexcept;
    inline void allOk() { memset(errCounters, 0, sizeof errCounters); }
    inline void to(enum state_c newState) __attribute__((__always_inline__))
    {
        std::ostringstream oss;
        oss << "sender switch to ";
        switch (newState)
        {
        case state_c::MODEM_SENDER_IDLE:
            oss << "MODEM_SENDER_IDLE";
            break;
        case state_c::MODEM_SENDER_SEND_DATA:
            oss << "MODEM_SENDER_SEND_DATA";
            break;
        case state_c::MODEM_SENDER_SEND_RTS:
            oss << "MODEM_SENDER_SEND_RTS";
            break;
        case state_c::MODEM_SENDER_WAIT_ACK:
            oss << "MODEM_SENDER_WAIT_ACK";
            break;
        }
        LOG_STREAM(oss);
        m_state = newState;
    }
};

#endif // MODEMSENDER_H
