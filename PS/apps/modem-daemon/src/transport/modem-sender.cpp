#include "modem-sender.h"

#include <iomanip>

/**
 * ERROR_ACK_TIMEOUT,
 * ERROR_INCORRECT_LENGTH,
 * ERROR_BAD_FORMAT,
 * ERROR_BAD_TYPE
 */
std::string modemSender_c::errorMessages[] = {
    "no ack",
    "bad length",
    "bad format",
    "bad type of packet",
};

modemSender_c::modemSender_c(block_t &blockToModem, std::string &strMessage) :
    m_state(state_c::MODEM_SENDER_IDLE),
    m_bIsRTSAcked(false),
    m_blockToModem(blockToModem), m_strMessageToClient(strMessage),
    m_nTotalPackets(0), m_nCurrentIdx(0)
{
    allOk();
}

//-------------------------------------------------------------------------------------------------
// управление автоматом
//-------------------------------------------------------------------------------------------------
void modemSender_c::operator()()
{
    std::ostringstream oss;
    uint16_t packetSize, checkSize;
    std::unique_ptr<uint8_t[]> packet;
    packetCommonHeader_t *pCommonHeader;
    packetAckHeader_t *pAckHeader;
    long timeElapsed;

    switch(m_state)
    {
    case state_c::MODEM_SENDER_IDLE:
        if (m_blockToModem.size() > 0)
        {
            // определить общее количество пакетов
            m_nTotalPackets = (uint16_t)ceil((double)m_blockToModem.size() / (MAX_PACKET_SIZE - sizeof(packetCommonHeader_t) - sizeof(packetDataHeader_t)));
            to(state_c::MODEM_SENDER_SEND_RTS);
            return;
        }
        break;

    case state_c::MODEM_SENDER_SEND_RTS:
        // запрос получателю на передачу данных
        sendRTS();
        to(state_c::MODEM_SENDER_WAIT_ACK);
        stopwatchStart();
        break;

    case state_c::MODEM_SENDER_WAIT_ACK:
        // ждем подтверждения
        if ((packetSize = receive(packet)) > 0)
        {
            // контроль минимальной длины пакета
            checkSize = sizeof (packetCommonHeader_t) + sizeof(packetAckHeader_t);
            if (packetSize < checkSize)
            {
                oss << "incorrect length: " << packetSize <<", minimum required: " << checkSize << " bytes";
                error(error_c::ERROR_INCORRECT_LENGTH, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // контроль длины пакета
            checkSize += m_nTotalPackets;
            if (packetSize != checkSize)
            {
                oss << "incorrect length, received " << packetSize << ", but expected: " << checkSize;
                error(error_c::ERROR_INCORRECT_LENGTH, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // контроль формата
            pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet.get());
            if (pCommonHeader->startToken != 0xAA55CC33)
            {
                oss << "incorrect start token: " << std::hex << "0x" << pCommonHeader->startToken << ", but expected: " << "0x" << 0xAA55CC33;
                error(error_c::ERROR_BAD_FORMAT, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // контроль типа пакета
            if (pCommonHeader->type != packetCommonHeader_t::type_c::ACK)
            {
                oss << "incorrect type: " << std::hex << "0x" << (int)pCommonHeader->type << ", need: " << (int)packetCommonHeader_t::type_c::ACK;
                error(error_c::ERROR_BAD_FORMAT, oss.str());
                LOG_STREAM(oss);
                break;
            }
            // пакет валиден
            allOk();
            m_bIsRTSAcked = true;
            pAckHeader = reinterpret_cast<packetAckHeader_t*>(packet.get() + sizeof(packetCommonHeader_t));
            oss << "ACK received, receivers expects " << pAckHeader->needPacketsCount << " packets";
            LOG_STREAM(oss);
            // обновить маски
            m_pReceivedPacketsMask.reset(new uint8_t[m_nTotalPackets]);
            memcpy(m_pReceivedPacketsMask.get(), packet.get() + sizeof(packetCommonHeader_t) + sizeof(packetAckHeader_t), m_nTotalPackets);
            if (pAckHeader->needPacketsCount > 0)
            {
                // отправителю нужны еще пакеты
                to(state_c::MODEM_SENDER_SEND_DATA);
            } else {
                // отправитель получил все пакетики
                reset();
            }
        } else if ((timeElapsed = stopwatchStop()) > MAX_WAIT_TIME) {
            oss << "timeout over " << timeElapsed << "ms";
            error(error_c::ERROR_ACK_TIMEOUT, oss.str());
            LOG_STREAM(oss);
        }
        break;

    case state_c::MODEM_SENDER_SEND_DATA:
        // пропускаем доставленные пакеты
        while((m_pReceivedPacketsMask[m_nCurrentIdx] == 0xFF) && m_nCurrentIdx < m_nTotalPackets) m_nCurrentIdx++;
        if (m_nCurrentIdx == m_nTotalPackets)
        {
            // все пакеты отправлены
            m_nCurrentIdx = 0;
            to(state_c::MODEM_SENDER_WAIT_ACK);
            stopwatchStart();
        } else {
            // отправляем очередной пакет
            sendData();
            m_nCurrentIdx++;
        }
        break;
    }
}

void modemSender_c::error(error_c err, const std::string &msg) noexcept
{
    if (++errCounters[(int)err] == MAX_ERRORS)
    {
        // достигли лимита ошибок
        // оставляем сообщение серверу
        m_strMessageToClient.clear();
        m_strMessageToClient.append(errorMessages[(int)err]);
        if (msg != "")
        {
            m_strMessageToClient.append("-");
            m_strMessageToClient.append(msg);
        }

        // сбрасываем автомат
        reset();
    } else {
        // повторить действие, если необходимо
        switch(m_state)
        {
        case state_c::MODEM_SENDER_WAIT_ACK:
            if (m_bIsRTSAcked == false && err == error_c::ERROR_ACK_TIMEOUT)
            {
                to(state_c::MODEM_SENDER_SEND_RTS);
                return;
            }
            break;
        default:;
        }
        stopwatchStart();
    }
}

void modemSender_c::reset() noexcept
{
    m_blockToModem.clear();
    m_pReceivedPacketsMask.reset(0);
    m_nTotalPackets = 0;
    m_nCurrentIdx = 0;
    m_bIsRTSAcked = false;
    allOk();
    to(state_c::MODEM_SENDER_IDLE);
}

//-------------------------------------------------------------------------------------------------
// функции ввода/вывода
//-------------------------------------------------------------------------------------------------
size_t modemSender_c::sendData() noexcept
{
    uint16_t offset = sizeof(packetCommonHeader_t) + sizeof (packetDataHeader_t);
    block_t::iterator itFirst = (m_blockToModem.begin() + m_nCurrentIdx * MAX_DATA_SIZE);
    block_t::iterator itLast = (itFirst + MAX_DATA_SIZE > m_blockToModem.end()) ? m_blockToModem.end() : itFirst + MAX_DATA_SIZE;

    // данные
    block_t packet(offset);
    packet.reserve(itLast - itFirst);
    packet.insert(packet.end(), itFirst, itLast);

    // заголовки
    packetCommonHeader_t *pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet.data());
    packetDataHeader_t *pDataHeader = reinterpret_cast<packetDataHeader_t*>(packet.data() + sizeof (packetCommonHeader_t));
    pCommonHeader->startToken = START_TOKEN;
    pCommonHeader->type = packetCommonHeader_t::type_c::DATA;
    pCommonHeader->length = packet.size();
    pDataHeader->number = m_nCurrentIdx;

    // отправляем
    return 0;
}

size_t modemSender_c::sendRTS() noexcept
{
    size_t packetLength = sizeof(packetCommonHeader_t) + sizeof(packetRTSHeader_t);
    uint8_t *packet = new uint8_t[packetLength];
    packetCommonHeader_t *pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet);
    packetRTSHeader_t *pRTSHeader = reinterpret_cast<packetRTSHeader_t*>(packet + sizeof(packetCommonHeader_t));

    // заголовки
    pRTSHeader->totalPacketsCount = m_nTotalPackets;
    pCommonHeader->startToken = START_TOKEN;
    pCommonHeader->type = packetCommonHeader_t::type_c::RTS;
    pCommonHeader->length = packetLength * sizeof *packet;

    // отправляем
    delete [] packet;
    return 0;
}

size_t modemSender_c::receive(std::unique_ptr<uint8_t[]> &pPacket) noexcept
{
    // if (m_pTcpSocket->bytesAvailable() > 0)
    // {
    //     QByteArray ba = m_pTcpSocket->readAll();
    //     std::unique_ptr<uint8_t[]> pReceivedPacket(new uint8_t[ba.size()]);
    //     memcpy(pReceivedPacket.get(), ba.data(), ba.size());
    //     pPacket.swap(pReceivedPacket);
    //     return ba.size();
    // } else 
    return 0;
}

//-------------------------------------------------------------------------------------------------
// сервисные функции
//-------------------------------------------------------------------------------------------------
long modemSender_c::getCurrentTimeMs (void) noexcept
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return round(spec.tv_sec*1.0e3 + spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds;
}
