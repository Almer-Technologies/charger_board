// RH_RF95.h
//
// Definitions for HopeRF LoRa radios per:
// http://www.hoperf.com/upload/rf/RFM95_96_97_98W.pdf
// http://www.hoperf.cn/upload/rfchip/RF96_97_98.pdf
//
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2014 Mike McCauley
// $Id: RH_RF95.h,v 1.6 2015/01/02 21:38:24 mikem Exp $
//

#ifndef RH_RF95_h
#define RH_RF95_h

// This is the maximum number of interrupts the driver can support
// Most Arduinos can handle 2, Megas can handle more
#define RH_RF95_NUM_INTERRUPTS 3

// Max number of octets the LORA Rx/Tx FIFO can hold
#define RH_RF95_FIFO_SIZE 255

// This is the maximum number of bytes that can be carried by the LORA.
// We use some for headers, keeping fewer for RadioHead messages
#define RH_RF95_MAX_PAYLOAD_LEN RH_RF95_FIFO_SIZE

// The length of the headers we add.
// The headers are inside the LORA's payload
#define RH_RF95_HEADER_LEN 4

// This is the maximum message length that can be supported by this driver.
// Can be pre-defined to a smaller size (to save SRAM) prior to including this header
// Here we allow for 1 byte message length, 4 bytes headers, user data and 2 bytes of FCS
#ifndef RH_RF95_MAX_MESSAGE_LEN
 #define RH_RF95_MAX_MESSAGE_LEN (RH_RF95_MAX_PAYLOAD_LEN - RH_RF95_HEADER_LEN)
#endif

// The crystal oscillator frequency of the module
#define RH_RF95_FXOSC 32000000.0

// The Frequency Synthesizer step = RH_RF95_FXOSC / 2^^19
#define RH_RF95_FSTEP  (RH_RF95_FXOSC / 524288)


// Register names (LoRa Mode, from table 85)
#define RH_RF95_REG_00_FIFO                                0x00
#define RH_RF95_REG_01_OP_MODE                             0x01
#define RH_RF95_REG_02_RESERVED                            0x02
#define RH_RF95_REG_03_RESERVED                            0x03
#define RH_RF95_REG_04_RESERVED                            0x04
#define RH_RF95_REG_05_RESERVED                            0x05
#define RH_RF95_REG_06_FRF_MSB                             0x06
#define RH_RF95_REG_07_FRF_MID                             0x07
#define RH_RF95_REG_08_FRF_LSB                             0x08
#define RH_RF95_REG_09_PA_CONFIG                           0x09
#define RH_RF95_REG_0A_PA_RAMP                             0x0a
#define RH_RF95_REG_0B_OCP                                 0x0b
#define RH_RF95_REG_0C_LNA                                 0x0c
#define RH_RF95_REG_0D_FIFO_ADDR_PTR                       0x0d
#define RH_RF95_REG_0E_FIFO_TX_BASE_ADDR                   0x0e
#define RH_RF95_REG_0F_FIFO_RX_BASE_ADDR                   0x0f
#define RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR                0x10
#define RH_RF95_REG_11_IRQ_FLAGS_MASK                      0x11
#define RH_RF95_REG_12_IRQ_FLAGS                           0x12
#define RH_RF95_REG_13_RX_NB_BYTES                         0x13
#define RH_RF95_REG_14_RX_HEADER_CNT_VALUE_MSB             0x14
#define RH_RF95_REG_15_RX_HEADER_CNT_VALUE_LSB             0x15
#define RH_RF95_REG_16_RX_PACKET_CNT_VALUE_MSB             0x16
#define RH_RF95_REG_17_RX_PACKET_CNT_VALUE_LSB             0x17
#define RH_RF95_REG_18_MODEM_STAT                          0x18
#define RH_RF95_REG_19_PKT_SNR_VALUE                       0x19
#define RH_RF95_REG_1A_PKT_RSSI_VALUE                      0x1a
#define RH_RF95_REG_1B_RSSI_VALUE                          0x1b
#define RH_RF95_REG_1C_HOP_CHANNEL                         0x1c
#define RH_RF95_REG_1D_MODEM_CONFIG1                       0x1d
#define RH_RF95_REG_1E_MODEM_CONFIG2                       0x1e
#define RH_RF95_REG_1F_SYMB_TIMEOUT_LSB                    0x1f
#define RH_RF95_REG_20_PREAMBLE_MSB                        0x20
#define RH_RF95_REG_21_PREAMBLE_LSB                        0x21
#define RH_RF95_REG_22_PAYLOAD_LENGTH                      0x22
#define RH_RF95_REG_23_MAX_PAYLOAD_LENGTH                  0x23
#define RH_RF95_REG_24_HOP_PERIOD                          0x24
#define RH_RF95_REG_25_FIFO_RX_BYTE_ADDR                   0x25
#define RH_RF95_REG_26_MODEM_CONFIG3                       0x26

#define RH_RF95_REG_40_DIO_MAPPING1                        0x40
#define RH_RF95_REG_41_DIO_MAPPING2                        0x41
#define RH_RF95_REG_42_VERSION                             0x42

#define RH_RF95_REG_4B_TCXO                                0x4b
#define RH_RF95_REG_4D_PA_DAC                              0x4d
#define RH_RF95_REG_5B_FORMER_TEMP                         0x5b
#define RH_RF95_REG_61_AGC_REF                             0x61
#define RH_RF95_REG_62_AGC_THRESH1                         0x62
#define RH_RF95_REG_63_AGC_THRESH2                         0x63
#define RH_RF95_REG_64_AGC_THRESH3                         0x64

// RH_RF95_REG_01_OP_MODE                             0x01
#define RH_RF95_LONG_RANGE_MODE                       0x80
#define RH_RF95_ACCESS_SHARED_REG                     0x40
#define RH_RF95_MODE                                  0x07
#define RH_RF95_MODE_SLEEP                            0x00
#define RH_RF95_MODE_STDBY                            0x01
#define RH_RF95_MODE_FSTX                             0x02
#define RH_RF95_MODE_TX                               0x03
#define RH_RF95_MODE_FSRX                             0x04
#define RH_RF95_MODE_RXCONTINUOUS                     0x05
#define RH_RF95_MODE_RXSINGLE                         0x06
#define RH_RF95_MODE_CAD                              0x07

// RH_RF95_REG_09_PA_CONFIG                           0x09
#define RH_RF95_PA_SELECT                             0x80
#define RH_RF95_OUTPUT_POWER                          0x0f

// RH_RF95_REG_0A_PA_RAMP                             0x0a
#define RH_RF95_LOW_PN_TX_PLL_OFF                     0x10
#define RH_RF95_PA_RAMP                               0x0f
#define RH_RF95_PA_RAMP_3_4MS                         0x00
#define RH_RF95_PA_RAMP_2MS                           0x01
#define RH_RF95_PA_RAMP_1MS                           0x02
#define RH_RF95_PA_RAMP_500US                         0x03
#define RH_RF95_PA_RAMP_250US                         0x0
#define RH_RF95_PA_RAMP_125US                         0x05
#define RH_RF95_PA_RAMP_100US                         0x06
#define RH_RF95_PA_RAMP_62US                          0x07
#define RH_RF95_PA_RAMP_50US                          0x08
#define RH_RF95_PA_RAMP_40US                          0x09
#define RH_RF95_PA_RAMP_31US                          0x0a
#define RH_RF95_PA_RAMP_25US                          0x0b
#define RH_RF95_PA_RAMP_20US                          0x0c
#define RH_RF95_PA_RAMP_15US                          0x0d
#define RH_RF95_PA_RAMP_12US                          0x0e
#define RH_RF95_PA_RAMP_10US                          0x0f

// RH_RF95_REG_0B_OCP                                 0x0b
#define RH_RF95_OCP_ON                                0x20
#define RH_RF95_OCP_TRIM                              0x1f

// RH_RF95_REG_0C_LNA                                 0x0c
#define RH_RF95_LNA_GAIN                              0xe0
#define RH_RF95_LNA_BOOST                             0x03
#define RH_RF95_LNA_BOOST_DEFAULT                     0x00
#define RH_RF95_LNA_BOOST_150PC                       0x11

// RH_RF95_REG_11_IRQ_FLAGS_MASK                      0x11
#define RH_RF95_RX_TIMEOUT_MASK                       0x80
#define RH_RF95_RX_DONE_MASK                          0x40
#define RH_RF95_PAYLOAD_CRC_ERROR_MASK                0x20
#define RH_RF95_VALID_HEADER_MASK                     0x10
#define RH_RF95_TX_DONE_MASK                          0x08
#define RH_RF95_CAD_DONE_MASK                         0x04
#define RH_RF95_FHSS_CHANGE_CHANNEL_MASK              0x02
#define RH_RF95_CAD_DETECTED_MASK                     0x01

// RH_RF95_REG_12_IRQ_FLAGS                           0x12
#define RH_RF95_RX_TIMEOUT                            0x80
#define RH_RF95_RX_DONE                               0x40
#define RH_RF95_PAYLOAD_CRC_ERROR                     0x20
#define RH_RF95_VALID_HEADER                          0x10
#define RH_RF95_TX_DONE                               0x08
#define RH_RF95_CAD_DONE                              0x04
#define RH_RF95_FHSS_CHANGE_CHANNEL                   0x02
#define RH_RF95_CAD_DETECTED                          0x01

// RH_RF95_REG_18_MODEM_STAT                          0x18
#define RH_RF95_RX_CODING_RATE                        0xe0
#define RH_RF95_MODEM_STATUS_CLEAR                    0x10
#define RH_RF95_MODEM_STATUS_HEADER_INFO_VALID        0x08
#define RH_RF95_MODEM_STATUS_RX_ONGOING               0x04
#define RH_RF95_MODEM_STATUS_SIGNAL_SYNCHRONIZED      0x02
#define RH_RF95_MODEM_STATUS_SIGNAL_DETECTED          0x01

// RH_RF95_REG_1C_HOP_CHANNEL                         0x1c
#define RH_RF95_PLL_TIMEOUT                           0x80
#define RH_RF95_RX_PAYLOAD_CRC_IS_ON                  0x40
#define RH_RF95_FHSS_PRESENT_CHANNEL                  0x3f

// RH_RF95_REG_1D_MODEM_CONFIG1                       0x1d
#define RH_RF95_BW                                    0xc0
#define RH_RF95_BW_125KHZ                             0x00
#define RH_RF95_BW_250KHZ                             0x40
#define RH_RF95_BW_500KHZ                             0x80
#define RH_RF95_BW_RESERVED                           0xc0
#define RH_RF95_CODING_RATE                           0x38
#define RH_RF95_CODING_RATE_4_5                       0x00
#define RH_RF95_CODING_RATE_4_6                       0x08
#define RH_RF95_CODING_RATE_4_7                       0x10
#define RH_RF95_CODING_RATE_4_8                       0x18
#define RH_RF95_IMPLICIT_HEADER_MODE_ON               0x04
#define RH_RF95_RX_PAYLOAD_CRC_ON                     0x02
#define RH_RF95_LOW_DATA_RATE_OPTIMIZE                0x01

// RH_RF95_REG_1E_MODEM_CONFIG2                       0x1e
#define RH_RF95_SPREADING_FACTOR                      0xf0
#define RH_RF95_SPREADING_FACTOR_64CPS                0x60
#define RH_RF95_SPREADING_FACTOR_128CPS               0x70
#define RH_RF95_SPREADING_FACTOR_256CPS               0x80
#define RH_RF95_SPREADING_FACTOR_512CPS               0x90
#define RH_RF95_SPREADING_FACTOR_1024CPS              0xa0
#define RH_RF95_SPREADING_FACTOR_2048CPS              0xb0
#define RH_RF95_SPREADING_FACTOR_4096CPS              0xc0
#define RH_RF95_TX_CONTINUOUS_MOE                     0x08
#define RH_RF95_AGC_AUTO_ON                           0x04
#define RH_RF95_SYM_TIMEOUT_MSB                       0x03

// RH_RF95_REG_4D_PA_DAC                              0x4d
#define RH_RF95_PA_DAC_DISABLE                        0x04
#define RH_RF95_PA_DAC_ENABLE                         0x07

/////////////////////////////////////////////////////////////////////
/// \class RH_RF95 RH_RF95.h <RH_RF95.h>
/// \brief Driver to send and receive unaddressed, unreliable datagrams via a LoRa
/// capable radio transceiver.
///
/// For Semtech SX1276/77/78 and HopeRF RFM95/96/97/98 and other similar LoRa capable radios.
/// Based on http://www.hoperf.com/upload/rf/RFM95_96_97_98W.pdf
/// and http://www.hoperf.cn/upload/rfchip/RF96_97_98.pdf
/// and http://www.semtech.com/images/datasheet/LoraDesignGuide_STD.pdf
/// and http://www.semtech.com/images/datasheet/sx1276.pdf
/// FSK/GFSK/OOK modes are not (yet) supported.
///
/// Works with
/// - the excellent MiniWirelessLoRa from Anarduino http://www.anarduino.com/miniwireless
///
/// \par Overview
///
/// This class provides basic functions for sending and receiving unaddressed,
/// unreliable datagrams of arbitrary length to 251 octets per packet.
///
/// Manager classes may use this class to implement reliable, addressed datagrams and streams,
/// mesh routers, repeaters, translators etc.
///
/// Naturally, for any 2 radios to communicate that must be configured to use the same frequency and
/// modulation scheme.
///
/// This Driver provides an object-oriented interface for sending and receiving data messages with Hope-RF
/// RFM95/96/97/98(W) and compatible radio modules in LoRa mode.
///
/// The Hope-RF (http://www.hoperf.com) RFM95/96/97/98(W) is a low-cost ISM transceiver
/// chip. It supports FSK, GFSK, OOK over a wide range of frequencies and
/// programmable data rates, and it also supports the proprietary LoRA (Long Range) mode, which
/// is the only mode supported in this RadioHead driver.
///
/// This Driver provides functions for sending and receiving messages of up
/// to 251 octets on any frequency supported by the radio, in a range of
/// predefined Bandwidths, Spreading Factors and Coding Rates.  Frequency can be set with
/// 61Hz precision to any frequency from 240.0MHz to 960.0MHz. Caution: most modules only support a more limited
/// range of frequencies due to antenna tuning.
///
/// Up to 2 RFM95/96/97/98(W) modules can be connected to an Arduino (3 on a Mega),
/// permitting the construction of translators and frequency changers, etc.
///
/// Support for other features such as transmitter power control etc is
/// also provided.
///
/// Tested on MinWirelessLoRa with arduino-1.0.5
/// on OpenSuSE 13.1
///
/// \par Packet Format
///
/// All messages sent and received by this RH_RF95 Driver conform to this packet format:
///
/// - LoRa mode:
/// - 8 symbol PREAMBLE
/// - Explicit header with header CRC (handled internally by the radio)
/// - 4 octets HEADER: (TO, FROM, ID, FLAGS)
/// - 0 to 251 octets DATA
/// - CRC (handled internally by the radio)
///
/// \par Connecting RFM95/96/97/98 to Arduino
///
/// We tested with Anarduino MiniWirelessLoRA, which is an Arduino Duemilanove compatible with a RFM96W
/// module on-board. Therefore it needs no connections other than the USB
/// programming connection and an antenna to make it work.
///
/// If you have a bare RFM95/96/97/98  that you want to connect to an Arduino, you
/// might use these connections (untested): CAUTION: you must use a 3.3V type
/// Arduino, otherwise you will also need voltage level shifters between the
/// Arduino and the RFM95.  CAUTION, you must also ensure you connect an
/// antenna.
///
/// \code
///                 Arduino      RFM95/96/97/98
///                 GND----------GND   (ground in)
///                 3V3----------3.3V  (3.3V in)
/// interrupt 0 pin D2-----------DIO0  (interrupt request out)
///          SS pin D10----------NSS   (chip select in)
///         SCK pin D13----------SCK   (SPI clock in)
///        MOSI pin D11----------MOSI  (SPI Data in)
///        MISO pin D12----------MISO  (SPI Data out)
/// \endcode
///
/// With these connections, you can then use the default constructor RH_RF95().
/// You can override the default settings for the SS pin and the interrupt in
/// the RH_RF95 constructor if you wish to connect the slave select SS to other
/// than the normal one for your Arduino (D10 for Diecimila, Uno etc and D53
/// for Mega) or the interrupt request to other than pin D2 (Caution,
/// different processors have different constraints as to the pins available
/// for interrupts).
///
/// It is possible to have 2 or more radios connected to one Arduino, provided
/// each radio has its own SS and interrupt line (SCK, SDI and SDO are common
/// to all radios)
///
/// Caution: on some Arduinos such as the Mega 2560, if you set the slave
/// select pin to be other than the usual SS pin (D53 on Mega 2560), you may
/// need to set the usual SS pin to be an output to force the Arduino into SPI
/// master mode.
///
/// Caution: Power supply requirements of the RFM module may be relevant in some circumstances:
/// RFM95/96/97/98 modules are capable of pulling 120mA+ at full power, where Arduino's 3.3V line can
/// give 50mA. You may need to make provision for alternate power supply for
/// the RFM module, especially if you wish to use full transmit power, and/or you have
/// other shields demanding power. Inadequate power for the RFM is likely to cause symptoms such as:
/// - reset's/bootups terminate with "init failed" messages
/// - random termination of communication after 5-30 packets sent/received
/// - "fake ok" state, where initialization passes fluently, but communication doesn't happen
/// - shields hang Arduino boards, especially during the flashing
///
/// \par Interrupts
///
/// The RH_RF95 driver uses interrupts to react to events in the RFM module,
/// such as the reception of a new packet, or the completion of transmission
/// of a packet.  The RH_RF95 driver interrupt service routine reads status from
/// and writes data to the the RFM module via the SPI interface. It is very
/// important therefore, that if you are using the RH_RF95 driver with another
/// SPI based deviced, that you disable interrupts while you transfer data to
/// and from that other device.  Use cli() to disable interrupts and sei() to
/// reenable them.
///
/// \par Memory
///
/// The RH_RF95 driver requires non-trivial amounts of memory. The sample
/// programs all compile to about 8kbytes each, which will fit in the
/// flash proram memory of most Arduinos. However, the RAM requirements are
/// more critical. Therefore, you should be vary sparing with RAM use in
/// programs that use the RH_RF95 driver.
///
/// It is often hard to accurately identify when you are hitting RAM limits on Arduino.
/// The symptoms can include:
/// - Mysterious crashes and restarts
/// - Changes in behaviour when seemingly unrelated changes are made (such as adding print() statements)
/// - Hanging
/// - Output from Serial.print() not appearing
///
/// \par Range
///
/// We have made some simple range tests under the following conditions:
/// - rf95_client base station connected to a VHF discone antenna at 8m height above ground
/// - rf95_server mobile connected to 17.3cm 1/4 wavelength antenna at 1m height, no ground plane.
/// - Both configured for 13dBm, 434MHz, Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range
/// - Minimum reported RSSI seen for successful comms was about -91
/// - Range over flat ground through heavy trees and vegetation approx 2km.
/// - At 20dBm (100mW) otherwise identical conditions approx 3km.
/// - At 20dBm, along salt water flat sandy beach, 3.2km.
///
/// It should be noted that at this data rate, a 12 octet message takes 2 seconds to transmit.
///
/// At 20dBm (100mW) with Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on.
/// (Default medium range) in the conditions described above.
/// - Range over flat ground through heavy trees and vegetation approx 2km.
///
/// \par Transmitter Power
///
/// You can control the transmitter power on the RF transceiver
/// with the RH_RF95::setTxPower() function. The argument can be any of
/// +5 to +23
/// The default is 13. Eg:
/// \code
/// driver.setTxPower(10);
/// \endcode
///
/// We have made some actual power measurements against
/// programmed power for Anarduino MiniWirelessLoRa (which has RFM96W-433Mhz installed)
/// - MiniWirelessLoRa RFM96W-433Mhz, USB power
/// - 30cm RG316 soldered direct to RFM96W module ANT and GND
/// - SMA connector
/// - 12db attenuator
/// - SMA connector
/// - MiniKits AD8307 HF/VHF Power Head (calibrated against Rohde&Schwartz 806.2020 test set)
/// - Tektronix TDS220 scope to measure the Vout from power head
/// \code
/// Program power           Measured Power
///    dBm                         dBm
///      5                           5
///      7                           7
///      9                           8
///     11                          11
///     13                          13
///     15                          15
///     17                          16
///     19                          18
///     20                          20
///     21                          21
///     22                          22
///     23                          23
/// \endcode
/// (Caution: we dont claim laboratory accuracy for these measurements)
/// You would not expect to get anywhere near these powers to air with a simple 1/4 wavelength wire antenna.

#endif
