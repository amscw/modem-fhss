-----------------------------------------------------------------------------------------
-- Project : FHSS-modem
-- Company: АО "НТЦ Элинс"
-- Author :Мельников Александр Юрьевич
-- Date : 2017-11-30
-- File : axi_pkg.vhd
-- Design: FHSS-modem
-----------------------------------------------------------------------------------------
-- Description : package с регистрами axi
-----------------------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;
use ieee.math_real.all;

package Axi_pkg is
	-- Количество регистров AXI
	constant REG_CNT           : integer := 256;
	constant OPT_MEM_ADDR_BITS : integer := integer(ceil(log2(real(REG_CNT * 4))));

	-- регистр включения/выключения заворотки MLIP
	constant ADDR_REG_MLIP_LOOP                  : integer := 0;
	-- регистр управления DMA (0 бит - запуск DMA MM2P)
	constant ADDR_REG_DMA_CR                     : integer := 1;
	-- регистр статуса DMA (1 бит - прерывание intr_rx)
	constant ADDR_REG_DMA_SR                     : integer := 2;
	-- регистр включения прерываний DMA (1 бит - прерывание intr_rx)
	constant ADDR_REG_DMA_IR                     : integer := 3;
	-- адрес источника
	constant ADDR_REG_DMA_SA                     : integer := 4;
	-- адрес назначения
	constant ADDR_REG_DMA_DA                     : integer := 5;
	-- длина массива данных DMA для источника 
	constant ADDR_REG_DMA_SL                     : integer := 6;
	-- длина массива данных DMA для получателя (read only) 
	constant ADDR_REG_DMA_DL                     : integer := 7;
	-- регистр статуса MLIP (0-ой бит - intr_tx)
	constant ADDR_REG_MLIP_SR                    : integer := 8;
	-- регистр включения прерываний MLIP (0-ой бит - intr_tx)
	constant ADDR_REG_MLIP_IR                    : integer := 9;
	-- регистр rst MLIP (0-ой бит - Mlip_rst_o)
	constant ADDR_REG_MLIP_RST                   : integer := 10;
	-- регистр ce MLIP (0-ой бит - Mlip_ce_o)
	constant ADDR_REG_MLIP_CE                    : integer := 11;
	-- Включить подсчет битых пакетов (0-ой бит - Mlip_cntcrc_on)
	constant ADDR_REG_MLIP_CNTCRC_ON             : integer := 12;
	-- Включить поле размер пакета в пакете MLIP
	constant ADDR_REG_MLIP_EN_SIZE               : integer := 13;
	-- Включить поле CRC пакета в пакете MLIP
	constant ADDR_REG_MLIP_EN_CRC                : integer := 14;
	-- Порог совпадения преамбулы MLIP
	constant ADDR_REG_MLIP_MATCH_CNT             : integer := 15;
	-- Количество неверных CRC в принятых пакетах
	constant ADDR_REG_MLIP_CNTCRC                : integer := 16;
	-- Размер пакетов MLIP при выключенном поле размера пакета
	constant ADDR_REG_MLIP_SIZE_PACK             : integer := 17;
	constant ADDR_REG_DMA_TX_CNT                 : integer := 18;
	constant ADDR_REG_DMA_RX_CNT                 : integer := 19;
	constant ADDR_REG_MLIP_TX_CNT                : integer := 20;
	constant ADDR_REG_MLIP_RX_CNT                : integer := 21;
	-- Сброс Канального и Физического уровня
	constant ADDR_REG_M_RST                      : integer := 22;
	-- Конфигурация Master/Slave (1 - Master, 0 - Slave)
	constant ADDR_REG_M_MASTER                   : integer := 23;
	-- Режим работы ("00" - Симплексный, "01" - Полудуплексный, "10" - Полнодуплексный)
	constant ADDR_REG_M_MODE                     : integer := 24;
	-- Регистры DLink Level
	-- Размер пакета синхронизации
	constant ADDR_REG_DLINK_REQPACK_TR_SIZE      : integer := 25;
	-- Размер пакета Подтверждения
	constant ADDR_REG_DLINK_ACKPACK_TR_SIZE      : integer := 26;
	-- Размер пакета тренировочной последователдьности
	constant ADDR_REG_DLINK_DATAPACK_TR_SIZE     : integer := 27;
	-- для синхронизации с кодером и САП размеры пакетов рассчитываются по формуле:
	--	1) для ADDR_REG_DLINK_CODER_MOD
	--	   when BPSK_1_2  => th := 192;     
	--	   when QPSK_1_2  => th := 384;    
	--	   when QPSK_3_4  => th := 384;
	--	   when QAM16_1_2 => th := 768;    
	--	   when QAM16_3_4 => th := 768;
	--	   when QAM64_2_3 => th := 1152;
	--	   when QAM64_3_4 => th := 1152;
	--	2) DATA_SIZE = th*(2+DLINK_CODER_NUMBW)
	--	3) Размер пакета должен быть кратным (DATA_SIZE)/2
	-- Например BPSK_1_2, DLINK_CODER_NUMBW = 0, Размер = 768 символов
	-- Размер пакета Данных Master-> Slave
	constant ADDR_REG_DLINK_DATAPACK_MDATA_SIZE  : integer := 28;
	-- Размер пакета Данных Master -> Slave (для симплекса и дуплекса должен быть равен 
	-- ADDR_REG_DLINK_DATAPACK_MDATA_SIZE)
	constant ADDR_REG_DLINK_DATAPACK_SDATA_SIZE  : integer := 29;
	-- TH для поиска преамбулы <= LEN_PREAMBLE
	constant ADDR_REG_DLINK_NMATCH               : integer := 30;
	constant ADDR_REG_DLINK_LOOP                 : integer := 31;
	constant ADDR_REG_DLINK_EST_START            : integer := 32;
	constant ADDR_REG_DLINK_EST_MAX              : integer := 33;
	-- Регистры PHY Level
	-- DBG --
	-- Проскальзывание NCO вперед в сэмплах
	constant ADDR_REG_PHY_DBG_NCO_SLIP_FW        : integer := 34;
	-- Проскальзывание NCO назад в сэмплах
	constant ADDR_REG_PHY_DBG_NCO_SLIP_BW        : integer := 35;
	-- подключение для тестирования valid и sync от мастера к слэйву и обратно
	constant ADDR_REG_PHY_FORCED_SYNCH           : integer := 36;
	-- Включение хоппера
	constant ADDR_REG_PHY_M_EN_HOP               : integer := 37;
	-- Степень скользящего среднего в блоке PowerMeter
	constant ADDR_REG_PHY_M_PWRMET_AV_SIZE       : integer := 38;
	-- Включение цифровой заворотки модема
	constant ADDR_REG_PHY_M_LOOP                 : integer := 39;
	-- Включение блока AFC
	constant ADDR_REG_PHY_M_EN_AFC               : integer := 40;
	-- метод вычисления ошибки Гарднера
	constant ADDR_REG_PHY_M_GARD_SIGN            : integer := 41;
	-- gard_th порог ошибки Гарднера
	constant ADDR_REG_PHY_M_GARD_TH              : integer := 42;
	-- gard_mu_p для Гарднера
	constant ADDR_REG_PHY_M_GARD_MU_P            : integer := 43;
	-- Заморозка NCO
	constant ADDR_REG_PHY_M_NCO_FRZ              : integer := 44;
	-- Режим тюнинга NCO мастера
	constant ADDR_REG_PHY_M_NCO_TUNE_MODE        : integer := 45;
	-- Стартовое значение тюнинга NCO мастера
	constant ADDR_REG_PHY_M_NCO_TUNE_CNT         : integer := 46;
	-- Включение таймаута подсчета dphi после проскальзывания
	constant ADDR_REG_PHY_M_NCO_EN_TIMEOUT_SLIP  : integer := 47;
	-- Длина оценки проскальызваний
	constant ADDR_REG_PHY_M_NCO_EST_LEN          : integer := 48;
	-- Сброс dphi пере приемом мастера
	constant ADDR_REG_PHY_M_NCO_RST_DPHI         : integer := 49;
	-- Включение Farrow
	constant ADDR_REG_PHY_M_EN_FARROW            : integer := 50;
	-- Порог для эквалайзера
	constant ADDR_REG_PHY_M_TEQ_TH               : integer := 51;
	-- включение усреднения ошибки AFC
	constant ADDR_REG_PHY_M_AFC_EN_SMA           : integer := 52;
	-- Коэффициент для петлевого фильтра AFC
	constant ADDR_REG_PHY_M_AFC_SCALE            : integer := 53;
	-- Порог частотной отстройки
	constant ADDR_REG_PHY_M_AFC_ERR_TH           : integer := 54;
	-- Ручное задание частоты при отключенном хоппере
	constant ADDR_REG_PHY_M_FREQ_MANUAL          : integer := 55;
	-- Коэффициент расширения полосы ППРЧ
	constant ADDR_REG_PHY_M_BW_FACTOR            : integer := 56;
	-- Смещение Полосы ППРЧ относительно несущей частоты
	constant ADDR_REG_PHY_M_DC_OFFSET            : integer := 57;
	-- Смещение (шаг) между частотами ППРЧ
	constant ADDR_REG_PHY_M_FREQ_OFFSET          : integer := 58;
	-- Включение проскальзывания в hopper
	constant ADDR_REG_PHY_M_HOP_EN_SLIP          : integer := 59;
	-- Параметр инициализации вихря Мерсена
	constant ADDR_REG_PHY_M_HOP_SEED             : integer := 60;
	-- Период ожидания после проскальзывания
	constant ADDR_REG_PHY_M_HOP_TIMEOUT_AFT_SLIP : integer := 61;
	-- Период проверки ошибки частотной синхронизации с блока PowerMeter
	constant ADDR_REG_PHY_M_HOP_EST_PRD          : integer := 62;
	-- Начальная оценка синхронизации после проскальзывания
	constant ADDR_REG_PHY_M_HOP_EST_START        : integer := 63;
	-- Порог ошибки частной синхронизации
	constant ADDR_REG_PHY_M_HOP_ERR_TH           : integer := 64;
	-- Максимальное значение оценки частотной синхронизации
	constant ADDR_REG_PHY_M_HOP_EST_MAX          : integer := 65;
	-- Размер таблицы частот
	constant ADDR_REG_PHY_M_HOP_LOOKUP_SIZE      : integer := 66;
	-- Включить сброс синхронизации от DlinkLayer
	constant ADDR_REG_PHY_M_HOP_EN_LINK          : integer := 67;
	-- Порог срабатывания lock_detect
	constant ADDR_REG_PHY_M_HOP_LOCK_DETECT_TH   : integer := 68;
	-- Включение управления аналоговым усилителем передатчика
	constant ADDR_REG_PHY_IC_EN_AMP              : integer := 69;
	-- Включение управления аналоговым усилителем приемника
	constant ADDR_REG_PHY_IC_EN_LNA              : integer := 70;
	-- Включение ручного режима управления аттенюатором 
	constant ADDR_REG_PHY_IC_ATT_MODE            : integer := 71;
	-- Занчение аттенюации при ручном управлении
	constant ADDR_REG_PHY_IC_ATT_VALUE           : integer := 72;
	-- Нижний порог петли гистерезиса для AttCtrl
	constant ADDR_REG_PHY_IC_ATT_THLOW           : integer := 73;
	-- Верхний порог петли гистерезиса для AttCtrl
	constant ADDR_REG_PHY_IC_ATT_THHIGH          : integer := 74;
	-- Период обновления значения усиления в AD9361
	constant ADDR_REG_PHY_IC_ATT_UPD_CNT         : integer := 75;
	-- Состояние частотной синхронизации
	constant ADDR_REG_PHY_LOCK_DET               : integer := 76;
	-- проверка match вместе с синхрой по ППРЧ
	constant ADDR_REG_DLINK_EN_LOCK_DETECT       : integer := 77;
	-- Регистр состояния связи Dlink
	constant ADDR_REG_DLINK_LINK                 : integer := 78;
	-- Регистр кода модуляции
	--		BPSK_1_2  => "000"
	--		QPSK_1_2  => "001"
	--		QPSK_3_4  => "010"
	--		QAM16_1_2 => "011"
	--		QAM16_3_4 => "100"
	--		QAM64_2_3 => "101"
	--		QAM64_3_4 => "110"	
	constant ADDR_REG_DLINK_CODER_MOD            : integer := 79;
	-- Количество промежуточных пакетов между first и last (0 - ...)
	constant ADDR_REG_DLINK_CODER_NUMBW          : integer := 80;
	-- Включение заворотки кодера
	constant ADDR_REG_DLINK_CODER_LOOP           : integer := 81;
	-- Включение/отключение кодера
	constant ADDR_REG_DLINK_CODER_EN             : integer := 82;
	-- SEED для Scrambler (IUC & BSID & FRAME_NUMBER)
	constant ADDR_REG_DLINK_CODER_SEED           : integer := 83;
	-- инкремент/декремент усиления
	constant ADDR_REG_PHY_IC_GAIN_UPDATE         : integer := 84;
	-- Ожидание наступления связи после lock_det в мс
	constant ADDR_REG_WATCH_WAIT_MAX             : integer := 85;
	-- Заморозка NCO в мс
	constant ADDR_REG_WATCH_FRZ_MAX              : integer := 86;
	-- Включение watchdog
	constant ADDR_REG_WATCH_EN                   : integer := 87;
	-- Счетчик срабатываний сторожевого пса
	constant ADDR_REG_WATCH_DOG_WORK             : integer := 88;
	-- Включение SAP уровня
	constant ADDR_REG_SAP_EN                     : integer := 89;
	-- Заворотка SAP уровня
	constant ADDR_REG_SAP_LOOP                   : integer := 90;
	-- ключ SAP 
	constant ADDR_REG_SAP_KEY_SAP                : integer := 91;
	-- SAP_KEY_INTR := 0x01234567;
	constant ADDR_REG_SAP_KEY_INTR               : integer := 92;
	-- Размер пакета данных
	constant ADDR_REG_SAP_SIZE_PACK              : integer := 93;
	-- Размер преамбулы (до 10)
	constant ADDR_REG_SAP_SIZE_PR                : integer := 94;
	-- Включение берометра DLINK
	constant ADDR_REG_DLINK_BER_EN               : integer := 95;

end package Axi_pkg;
