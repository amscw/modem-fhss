#include "modem-receiver.h"


std::string modemReceiver_c::errorMessages[] = {
    "bad length",
    "bad format",
    "bad type of packet",
    "no data"
};

modemReceiver_c::modemReceiver_c(block_t &blockFromModem) : 
    m_state(state_c::MODEM_RECEIVER_LISTEN),
    m_blockFromModem(blockFromModem),
    m_nTotalPackets(0)
{
    allOk();
}

//-------------------------------------------------------------------------------------------------
// управление автоматом
//-------------------------------------------------------------------------------------------------
void modemReceiver_c::operator ()()
{
    std::ostringstream oss;
    std::unique_ptr<uint8_t[]> packet;
    uint16_t packetSize, checkSize;
    packetCommonHeader_t *pCommonHeader;
    packetRTSHeader_t *pRTSHeader;
    packetDataHeader_t *pDataHeader;
    long timeElapsed;
    size_t offset;

    switch (m_state)
    {
    case state_c::MODEM_RECEIVER_LISTEN:
        if ((packetSize = receive(packet)) > 0)
        {
            // контроль длины пакета
            checkSize = sizeof (packetCommonHeader_t) + sizeof(packetRTSHeader_t);
            if (packetSize != checkSize)
            {
                oss << "incorrect length: " << packetSize << ", need: " << checkSize << " bytes";
                LOG_STREAM(oss);
                break;
            }

            // контроль формата
            pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet.get());
            if (pCommonHeader->startToken != START_TOKEN)
            {
                oss << "incorrect start token: " << std::hex << "0x" << pCommonHeader->startToken << ", need: " << "0x" << START_TOKEN;
                LOG_STREAM(oss);
                break;
            }

            // можно обрабатывать
            if (pCommonHeader->type == packetCommonHeader_t::type_c::RTS)
            {
                pRTSHeader = reinterpret_cast<packetRTSHeader_t*>(packet.get() + sizeof (packetCommonHeader_t));
                m_nTotalPackets = pRTSHeader->totalPacketsCount;
                m_pReceivedPacketsMask.reset(new uint8_t[m_nTotalPackets]);
                memset(m_pReceivedPacketsMask.get(), 0, m_nTotalPackets);
                oss << "RTS received, expected " << m_nTotalPackets << " packets";
                LOG_STREAM(oss);
                to(state_c::MODEM_RECEIVER_SEND_ACK);
            }
        }
        break;

    case state_c::MODEM_RECEIVER_SEND_ACK:
        sendACK();
        to(state_c::MODEM_RECEIVER_RECEIVE_DATA);
        stopwatchStart();
        break;

    case state_c::MODEM_RECEIVER_RECEIVE_DATA:
        if ((packetSize = receive(packet)) > 0)
        {
            // контроль минимальной длины пакета
            checkSize = sizeof (packetCommonHeader_t) + sizeof(packetRTSHeader_t);
            if (packetSize < checkSize)
            {
                oss << "incorrect length: " << packetSize << ", minimum required: " << checkSize << " bytes";
                error(error_c::ERROR_INCORRECT_LENGTH, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // контроль формата
            pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet.get());
            if (pCommonHeader->startToken != START_TOKEN)
            {
                oss << "incorrect start token: " << std::hex << "0x" << pCommonHeader->startToken << ", need: " << "0x" << START_TOKEN;
                error(error_c::ERROR_BAD_FORMAT, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // контроль длины на соответствие заявленной
            if (pCommonHeader->length != packetSize)
            {
                oss << "incorrect length: " << packetSize << ", but declared: " << pCommonHeader->length << " bytes";
                error(error_c::ERROR_INCORRECT_LENGTH, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // контроль типа
            if (pCommonHeader->type != packetCommonHeader_t::type_c::DATA)
            {
                oss << "type of received packet - 0x" << std::hex << (int)pCommonHeader->type << ", need - 0x" << (int)packetCommonHeader_t::type_c::DATA;
                error(error_c::ERROR_BAD_TYPE, oss.str());
                LOG_STREAM(oss);
                break;
            }

            // пакет валидный, отмечаем
            pDataHeader = reinterpret_cast<packetDataHeader_t*>(packet.get() + sizeof(packetCommonHeader_t));
            m_pReceivedPacketsMask[pDataHeader->number] = 0xFF;

            // забираем
            m_listReceivedPackets.emplace_back(std::move(packet));

            oss << "received data packet #" << (int)pDataHeader->number << " (" << packetSize << " bytes)";
            LOG_STREAM(oss);
            allOk();

            if (m_listReceivedPackets.size() == m_nTotalPackets)
            {
                // все пакеты приняты
                to(state_c::MODEM_RECEIVER_PROCESS_DATA);
                break;
            } else if (pDataHeader->number == m_nTotalPackets - 1) {
                // НЕ все пакеты приняты, но больше не будет. Перезапрос.
                to(state_c::MODEM_RECEIVER_SEND_ACK);
                break;
            }
            // ждем следующий...
            stopwatchStart();
        } else if ((timeElapsed = stopwatchStop()) > MAX_WAIT_TIME) {
            oss << "timeout over " << timeElapsed << "ms";
            error(error_c::ERROR_TIMEOUT_WAIT_DATA, oss.str());
            LOG_STREAM(oss);
        }
        break;

    case state_c::MODEM_RECEIVER_PROCESS_DATA:
        m_listReceivedPackets.sort(comparePackets());
        // все пакеты отсортированы по порядку, теперь просто последовательно заполняем выходной блок
        m_blockFromModem.clear();
        offset = sizeof (packetCommonHeader_t) + sizeof (packetDataHeader_t);
        for (const std::unique_ptr<uint8_t[]> &packet : m_listReceivedPackets)
        {
            pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet.get());
            pDataHeader = reinterpret_cast<packetDataHeader_t*>(packet.get() + sizeof (packetCommonHeader_t));
            m_blockFromModem.insert(m_blockFromModem.end(), packet.get() + offset, packet.get() + pCommonHeader->length);
        }
        oss << "new block received from modem (total: " << m_blockFromModem.size() << " bytes)";
        LOG_STREAM(oss);
        // оповещаем отправителя об успешной доставке
        sendACK();
        // сбрасываем автомат
        reset();
        break;

    default:
        break;
    }
}

void modemReceiver_c::error(error_c err, const std::string &msg) noexcept
{
    if (++errCounters[(int)err] == MAX_ERRORS)
    {
        // достигли лимита ошибок
        reset();
    } else {
        // повторить действие, если необходимо
        switch(m_state)
        {
        case state_c::MODEM_RECEIVER_RECEIVE_DATA:
            if (err == error_c::ERROR_TIMEOUT_WAIT_DATA)
            {
                to(state_c::MODEM_RECEIVER_SEND_ACK);
                return;
            }
            break;
        default:;
        }
        stopwatchStart();
    }
}
void modemReceiver_c::reset() noexcept
{
    // уничтожаем пакет
    // m_blockFromModem.clear();

    // уничтожить все принятые пакеты
    m_listReceivedPackets.clear();

    // сбрасываем автомат
    m_nTotalPackets = 0;
    m_pReceivedPacketsMask.reset(0);
    m_listReceivedPackets.clear();
    allOk();
    to(state_c::MODEM_RECEIVER_LISTEN);
}

//-------------------------------------------------------------------------------------------------
// функции ввода/вывода
//-------------------------------------------------------------------------------------------------
size_t modemReceiver_c::sendACK() noexcept
{
    size_t packetLength = sizeof(packetCommonHeader_t) + sizeof(packetAckHeader_t) + m_nTotalPackets;
    uint8_t *packet = new uint8_t[packetLength];
    packetCommonHeader_t *pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet);
    packetAckHeader_t *pAckHeader = reinterpret_cast<packetAckHeader_t*>(packet + sizeof(packetCommonHeader_t));

    // заголовки
    pAckHeader->needPacketsCount = m_nTotalPackets - m_listReceivedPackets.size();
    pCommonHeader->startToken = START_TOKEN;
    pCommonHeader->type = packetCommonHeader_t::type_c::ACK;
    pCommonHeader->length = packetLength * sizeof *packet;

    // маска недостающих пакетов
    uint8_t *mask = reinterpret_cast<uint8_t*>(pAckHeader) + sizeof (packetAckHeader_t);
    memcpy(mask, m_pReceivedPacketsMask.get(), m_nTotalPackets);

    // отправляем
    delete [] packet;
    return 0;
}

size_t modemReceiver_c::receive(std::unique_ptr<uint8_t[]> &pPacket) noexcept
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
long modemReceiver_c::getCurrentTimeMs (void) noexcept
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return round(spec.tv_sec*1.0e3 + spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds;
}

void modemReceiver_c::showListReceivedPackets() noexcept
{
    std::ostringstream oss;
    std::cout << "total " << m_listReceivedPackets.size() << " packets" << std::endl;
    for (const std::unique_ptr<uint8_t[]> &packet : m_listReceivedPackets)
    {
        packetCommonHeader_t *pCommonHeader = reinterpret_cast<packetCommonHeader_t*>(packet.get());
        packetDataHeader_t *pDataHeader = reinterpret_cast<packetDataHeader_t*>(packet.get() + sizeof(packetCommonHeader_t));
        oss << "packet #" << (int)pDataHeader->number << " (" << std::hex << "0x" << (int)pCommonHeader->type << std::dec <<"), length = " << (int)pCommonHeader->length;
        std::cout << oss.str() << std::endl;
        oss.str("");
        oss.clear();
    }
}
