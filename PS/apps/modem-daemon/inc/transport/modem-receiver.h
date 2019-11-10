#ifndef MODEMRECEIVER_H
#define MODEMRECEIVER_H

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <utility>
#include <sstream>
#include <cmath>
#include "string.h"
#include "unistd.h"
#include "modem-proto.h"
#include "tracers.h"

class modemReceiver_c
{
    const size_t    MAX_PACKET_SIZE = 1024;
    const long      MAX_WAIT_TIME = 1500;
    const uint8_t   MAX_ERRORS = 3;

    struct comparePackets : public std::binary_function<std::unique_ptr<uint8_t[]>, std::unique_ptr<uint8_t[]>, bool>
    {
    public:
        bool operator ()(const std::unique_ptr<uint8_t[]> &packetA, const std::unique_ptr<uint8_t[]> &packetB)
        {
            packetDataHeader_t *pDataHeaderA = reinterpret_cast<packetDataHeader_t*>(packetA.get() + sizeof (packetCommonHeader_t));
            packetDataHeader_t *pDataHeaderB = reinterpret_cast<packetDataHeader_t*>(packetB.get() + sizeof (packetCommonHeader_t));
            //std::cout << "A: " << (int)(pDataHeaderA->number) << ", B: " << (int)(pDataHeaderB->number) << std::endl;
            return pDataHeaderA->number < pDataHeaderB->number;

        }
    };

    static std::string errorMessages[];

    enum class error_c : uint32_t{
        ERROR_INCORRECT_LENGTH,
        ERROR_BAD_FORMAT,
        ERROR_BAD_TYPE,
        ERROR_TIMEOUT_WAIT_DATA,
    };

    enum class state_c : uint32_t
    {
        MODEM_RECEIVER_IDLE,
        MODEM_RECEIVER_LISTEN,
        MODEM_RECEIVER_SEND_ACK,
        MODEM_RECEIVER_RECEIVE_DATA,
        MODEM_RECEIVER_PROCESS_DATA,
    } m_state;
    block_t &m_blockFromModem;
    uint16_t m_nTotalPackets;
    long m_nTimeMs;
    std::unique_ptr<uint8_t[]> m_pReceivedPacketsMask;
    std::list<std::unique_ptr<uint8_t[]>> m_listReceivedPackets;
    uint8_t errCounters[10];
    
public:
    modemReceiver_c(block_t &blockFromModem);
    void operator()();
    // void ToIdle();
    // void Resume();

private:
    long getCurrentTimeMs (void) noexcept;
    void showListReceivedPackets(void) noexcept;
    inline void stopwatchStart() noexcept { m_nTimeMs = getCurrentTimeMs(); }
    inline long stopwatchStop() noexcept { return getCurrentTimeMs() - m_nTimeMs; }
    size_t sendACK() noexcept;
    size_t receive(std::unique_ptr<uint8_t[]> &pPacket) noexcept;
    /**
     * @brief error завершает работу автомата с ошибкой
     * @param err код ошибки
     * @param msg сообщение об ошибке
     */
    void error(enum error_c err, const std::string &msg = "") noexcept;
    void reset() noexcept;
    inline void allOk() { memset(errCounters, 0, sizeof errCounters); }
    inline void to(enum state_c newState) __attribute__((__always_inline__))
    {
        std::ostringstream oss;
        oss << "receiver switch to ";
        switch (newState)
        {
        case state_c::MODEM_RECEIVER_IDLE:
            oss << "MODEM_RECEIVER_IDLE";
            break;
        case state_c::MODEM_RECEIVER_LISTEN:
            oss << "MODEM_RECEIVER_LISTEN";
            break;
        case state_c::MODEM_RECEIVER_SEND_ACK:
            oss << "MODEM_RECEIVER_SEND_ACK";
            break;
        case state_c::MODEM_RECEIVER_RECEIVE_DATA:
            oss << "MODEM_RECEIVER_RECEIVE_DATA";
            break;
        case state_c::MODEM_RECEIVER_PROCESS_DATA:
            oss << "MODEM_RECEIVER_PROCESS_DATA";
            break;
        }
        LOG_STREAM(oss);
        m_state = newState;
    }
};

#endif // MODEMRECEIVER_H
