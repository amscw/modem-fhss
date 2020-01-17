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
	-- регистр статуса LINK (0-ой бит - link on, 1-ый бит - link off)
	constant ADDR_REG_LINK_SR                    : integer := 12;
	-- регистр включения прерываний LINK (0-ой бит - link on, 1-ый бит - link off)
	constant ADDR_REG_LINK_IR                    : integer := 13;
	-- Количество неверных CRC в принятых пакетах
	constant ADDR_REG_MLIP_CNTCRC                : integer := 14;
	constant ADDR_REG_DMA_TX_CNT                 : integer := 15;
	constant ADDR_REG_DMA_RX_CNT                 : integer := 16;
	constant ADDR_REG_MLIP_TX_CNT                : integer := 17;
	constant ADDR_REG_MLIP_RX_CNT                : integer := 18;
	-- Сброс Канального и Физического уровня
	constant ADDR_REG_M_RST                      : integer := 19;
	-- Конфигурация Master/Slave (1 - Master, 0 - Slave)
	constant ADDR_REG_M_MASTER                   : integer := 20;
	-- Режим работы ("00" - Симплексный, "01" - Полудуплексный, "10" - Полнодуплексный)
	constant ADDR_REG_M_MODE                     : integer := 21;
	-- Регистры DLink Level
	-- Размер пакета синхронизации
	constant ADDR_REG_DLINK_REQPACK_TR_SIZE      : integer := 22;
	-- Размер пакета Подтверждения
	constant ADDR_REG_DLINK_ACKPACK_TR_SIZE      : integer := 23;
	-- Размер пакета тренировочной последователдьности
	constant ADDR_REG_DLINK_DATAPACK_TR_SIZE     : integer := 24;
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
	constant ADDR_REG_DLINK_DATAPACK_MDATA_SIZE  : integer := 25;
	-- Размер пакета Данных Master -> Slave (для симплекса и дуплекса должен быть равен 
	-- ADDR_REG_DLINK_DATAPACK_MDATA_SIZE)
	constant ADDR_REG_DLINK_DATAPACK_SDATA_SIZE  : integer := 26;
	-- TH для поиска преамбулы <= LEN_PREAMBLE
	constant ADDR_REG_DLINK_NMATCH               : integer := 27;
	constant ADDR_REG_DLINK_LOOP                 : integer := 28;
	constant ADDR_REG_DLINK_EST_START            : integer := 29;
	constant ADDR_REG_DLINK_EST_MAX              : integer := 30;
	-- Регистры PHY Level
	-- DBG --
	-- Проскальзывание NCO вперед в сэмплах
	constant ADDR_REG_PHY_DBG_NCO_SLIP_FW        : integer := 31;
	-- Проскальзывание NCO назад в сэмплах
	constant ADDR_REG_PHY_DBG_NCO_SLIP_BW        : integer := 32;
	-- подключение для тестирования valid и sync от мастера к слэйву и обратно
	constant ADDR_REG_PHY_FORCED_SYNCH           : integer := 33;
	-- Включение хоппера
	constant ADDR_REG_PHY_M_EN_HOP               : integer := 34;
	-- Степень скользящего среднего в блоке PowerMeter
	constant ADDR_REG_PHY_M_PWRMET_AV_SIZE       : integer := 35;
	-- Включение цифровой заворотки модема
	constant ADDR_REG_PHY_M_LOOP                 : integer := 36;
	-- Включение блока AFC
	constant ADDR_REG_PHY_M_EN_AFC               : integer := 37;
	-- метод вычисления ошибки Гарднера
	constant ADDR_REG_PHY_M_GARD_SIGN            : integer := 38;
	-- gard_th порог ошибки Гарднера
	constant ADDR_REG_PHY_M_GARD_TH              : integer := 39;
	-- gard_mu_p для Гарднера
	constant ADDR_REG_PHY_M_GARD_MU_P            : integer := 40;
	-- Заморозка NCO
	constant ADDR_REG_PHY_M_NCO_FRZ              : integer := 41;
	-- Режим тюнинга NCO мастера
	constant ADDR_REG_PHY_M_NCO_TUNE_MODE        : integer := 42;
	-- Стартовое значение тюнинга NCO мастера
	constant ADDR_REG_PHY_M_NCO_TUNE_CNT         : integer := 43;
	-- Включение таймаута подсчета dphi после проскальзывания
	constant ADDR_REG_PHY_M_NCO_EN_TIMEOUT_SLIP  : integer := 44;
	-- Длина оценки проскальызваний
	constant ADDR_REG_PHY_M_NCO_EST_LEN          : integer := 45;
	-- Сброс dphi пере приемом мастера
	constant ADDR_REG_PHY_M_NCO_RST_DPHI         : integer := 46;
	-- Включение Farrow
	constant ADDR_REG_PHY_M_EN_FARROW            : integer := 47;
	-- Порог для эквалайзера
	constant ADDR_REG_PHY_M_TEQ_TH               : integer := 48;
	-- включение усреднения ошибки AFC
	constant ADDR_REG_PHY_M_AFC_EN_SMA           : integer := 49;
	-- Коэффициент для петлевого фильтра AFC
	constant ADDR_REG_PHY_M_AFC_SCALE            : integer := 50;
	-- Порог частотной отстройки
	constant ADDR_REG_PHY_M_AFC_ERR_TH           : integer := 51;
	-- Ручное задание частоты при отключенном хоппере
	constant ADDR_REG_PHY_M_FREQ_MANUAL          : integer := 52;
	-- Коэффициент расширения полосы ППРЧ
	constant ADDR_REG_PHY_M_BW_FACTOR            : integer := 53;
	-- Смещение Полосы ППРЧ относительно несущей частоты
	constant ADDR_REG_PHY_M_DC_OFFSET            : integer := 54;
	-- Смещение (шаг) между частотами ППРЧ
	constant ADDR_REG_PHY_M_FREQ_OFFSET          : integer := 55;
	-- Включение проскальзывания в hopper
	constant ADDR_REG_PHY_M_HOP_EN_SLIP          : integer := 56;
	-- Параметр инициализации вихря Мерсена
	constant ADDR_REG_PHY_M_HOP_SEED             : integer := 57;
	-- Период ожидания после проскальзывания
	constant ADDR_REG_PHY_M_HOP_TIMEOUT_AFT_SLIP : integer := 58;
	-- Период проверки ошибки частотной синхронизации с блока PowerMeter
	constant ADDR_REG_PHY_M_HOP_EST_PRD          : integer := 59;
	-- Начальная оценка синхронизации после проскальзывания
	constant ADDR_REG_PHY_M_HOP_EST_START        : integer := 60;
	-- Порог ошибки частной синхронизации
	constant ADDR_REG_PHY_M_HOP_ERR_TH           : integer := 61;
	-- Максимальное значение оценки частотной синхронизации
	constant ADDR_REG_PHY_M_HOP_EST_MAX          : integer := 62;
	-- Размер таблицы частот
	constant ADDR_REG_PHY_M_HOP_LOOKUP_SIZE      : integer := 63;
	-- Включить сброс синхронизации от DlinkLayer
	constant ADDR_REG_PHY_M_HOP_EN_LINK          : integer := 64;
	-- Порог срабатывания lock_detect
	constant ADDR_REG_PHY_M_HOP_LOCK_DETECT_TH   : integer := 65;
	-- Включение управления аналоговым усилителем передатчика
	constant ADDR_REG_PHY_IC_EN_AMP              : integer := 66;
	-- Включение управления аналоговым усилителем приемника
	constant ADDR_REG_PHY_IC_EN_LNA              : integer := 67;
	-- Включение ручного режима управления аттенюатором 
	constant ADDR_REG_PHY_IC_ATT_MODE            : integer := 68;
	-- Занчение аттенюации при ручном управлении
	constant ADDR_REG_PHY_IC_ATT_VALUE           : integer := 69;
	-- Нижний порог петли гистерезиса для AttCtrl
	constant ADDR_REG_PHY_IC_ATT_THLOW           : integer := 70;
	-- Верхний порог петли гистерезиса для AttCtrl
	constant ADDR_REG_PHY_IC_ATT_THHIGH          : integer := 71;
	-- Период обновления значения усиления в AD9361
	constant ADDR_REG_PHY_IC_ATT_UPD_CNT         : integer := 72;
	-- Состояние частотной синхронизации
	constant ADDR_REG_PHY_LOCK_DET               : integer := 73;
	-- проверка match вместе с синхрой по ППРЧ
	constant ADDR_REG_DLINK_EN_LOCK_DETECT       : integer := 74;
	-- Регистр состояния связи Dlink
	constant ADDR_REG_DLINK_LINK                 : integer := 75;
	-- Регистр кода модуляции
	--		BPSK_1_2  => "000"
	--		QPSK_1_2  => "001"
	--		QPSK_3_4  => "010"
	--		QAM16_1_2 => "011"
	--		QAM16_3_4 => "100"
	--		QAM64_2_3 => "101"
	--		QAM64_3_4 => "110"	
	constant ADDR_REG_DLINK_CODER_MOD            : integer := 76;
	-- Количество промежуточных пакетов между first и last (0 - ...)
	constant ADDR_REG_DLINK_CODER_NUMBW          : integer := 77;
	-- Включение заворотки кодера
	constant ADDR_REG_DLINK_CODER_LOOP           : integer := 78;
	-- Включение/отключение кодера
	constant ADDR_REG_DLINK_CODER_EN             : integer := 79;
	-- SEED для Scrambler (IUC & BSID & FRAME_NUMBER)
	constant ADDR_REG_DLINK_CODER_SEED           : integer := 80;
	-- инкремент/декремент усиления
	constant ADDR_REG_PHY_IC_GAIN_UPDATE         : integer := 81;
	-- Ожидание наступления связи после lock_det в мс
	constant ADDR_REG_WATCH_WAIT_MAX             : integer := 82;
	-- Заморозка NCO в мс
	constant ADDR_REG_WATCH_FRZ_MAX              : integer := 83;
	-- Включение watchdog
	constant ADDR_REG_WATCH_EN                   : integer := 84;
	-- Счетчик срабатываний сторожевого пса
	constant ADDR_REG_WATCH_DOG_WORK             : integer := 85;
	-- Включение SAP уровня
	constant ADDR_REG_SAP_EN                     : integer := 86;
	-- Заворотка SAP уровня
	constant ADDR_REG_SAP_LOOP                   : integer := 87;
	-- ключ SAP 
	constant ADDR_REG_SAP_KEY_SAP                : integer := 88;
	-- SAP_KEY_INTR := 0x01234567;
	constant ADDR_REG_SAP_KEY_INTR               : integer := 89;
	-- Размер пакета данных
	constant ADDR_REG_SAP_SIZE_PACK              : integer := 90;
	-- Размер преамбулы (до 10)
	constant ADDR_REG_SAP_SIZE_PR                : integer := 91;
	-- Включение берометра DLINK
	constant ADDR_REG_DLINK_BER_EN               : integer := 92;

end package Axi_pkg;
