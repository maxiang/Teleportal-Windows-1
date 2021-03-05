/****************************************************************************
** Meta object code from reading C++ file 'ping-message-all.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../teleportalQT/dependencies/ping-cpp/src/message/ping-message-all.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ping-message-all.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PingEnumNamespace_t {
    QByteArrayData data[66];
    char stringdata0[1363];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PingEnumNamespace_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PingEnumNamespace_t qt_meta_stringdata_PingEnumNamespace = {
    {
QT_MOC_LITERAL(0, 0, 17), // "PingEnumNamespace"
QT_MOC_LITERAL(1, 18, 14), // "PingDeviceType"
QT_MOC_LITERAL(2, 33, 7), // "UNKNOWN"
QT_MOC_LITERAL(3, 41, 6), // "PING1D"
QT_MOC_LITERAL(4, 48, 7), // "PING360"
QT_MOC_LITERAL(5, 56, 6), // "PINGHF"
QT_MOC_LITERAL(6, 63, 13), // "PingMessageId"
QT_MOC_LITERAL(7, 77, 21), // "PING1D_SPEED_OF_SOUND"
QT_MOC_LITERAL(8, 99, 16), // "PING1D_MODE_AUTO"
QT_MOC_LITERAL(9, 116, 14), // "PING1D_PROFILE"
QT_MOC_LITERAL(10, 131, 22), // "PING1D_DISTANCE_SIMPLE"
QT_MOC_LITERAL(11, 154, 19), // "PING1D_GENERAL_INFO"
QT_MOC_LITERAL(12, 174, 16), // "PING1D_VOLTAGE_5"
QT_MOC_LITERAL(13, 191, 20), // "PING1D_PING_INTERVAL"
QT_MOC_LITERAL(14, 212, 28), // "PING1D_PROCESSOR_TEMPERATURE"
QT_MOC_LITERAL(15, 241, 22), // "PING1D_PCB_TEMPERATURE"
QT_MOC_LITERAL(16, 264, 23), // "PING1D_FIRMWARE_VERSION"
QT_MOC_LITERAL(17, 288, 16), // "PING1D_DEVICE_ID"
QT_MOC_LITERAL(18, 305, 15), // "PING1D_DISTANCE"
QT_MOC_LITERAL(19, 321, 19), // "PING1D_GAIN_SETTING"
QT_MOC_LITERAL(20, 341, 24), // "PING1D_TRANSMIT_DURATION"
QT_MOC_LITERAL(21, 366, 18), // "PING1D_PING_ENABLE"
QT_MOC_LITERAL(22, 385, 12), // "PING1D_RANGE"
QT_MOC_LITERAL(23, 398, 20), // "PING1D_SET_MODE_AUTO"
QT_MOC_LITERAL(24, 419, 22), // "PING1D_SET_PING_ENABLE"
QT_MOC_LITERAL(25, 442, 23), // "PING1D_SET_GAIN_SETTING"
QT_MOC_LITERAL(26, 466, 25), // "PING1D_SET_SPEED_OF_SOUND"
QT_MOC_LITERAL(27, 492, 20), // "PING1D_SET_DEVICE_ID"
QT_MOC_LITERAL(28, 513, 24), // "PING1D_SET_PING_INTERVAL"
QT_MOC_LITERAL(29, 538, 16), // "PING1D_SET_RANGE"
QT_MOC_LITERAL(30, 555, 23), // "PING1D_CONTINUOUS_START"
QT_MOC_LITERAL(31, 579, 22), // "PING1D_GOTO_BOOTLOADER"
QT_MOC_LITERAL(32, 602, 22), // "PING1D_CONTINUOUS_STOP"
QT_MOC_LITERAL(33, 625, 25), // "COMMON_DEVICE_INFORMATION"
QT_MOC_LITERAL(34, 651, 23), // "COMMON_PROTOCOL_VERSION"
QT_MOC_LITERAL(35, 675, 10), // "COMMON_ACK"
QT_MOC_LITERAL(36, 686, 22), // "COMMON_GENERAL_REQUEST"
QT_MOC_LITERAL(37, 709, 17), // "COMMON_ASCII_TEXT"
QT_MOC_LITERAL(38, 727, 11), // "COMMON_NACK"
QT_MOC_LITERAL(39, 739, 24), // "BLUEBPS_CELL_VOLTAGE_MIN"
QT_MOC_LITERAL(40, 764, 23), // "BLUEBPS_TEMPERATURE_MAX"
QT_MOC_LITERAL(41, 788, 20), // "BLUEBPS_CELL_TIMEOUT"
QT_MOC_LITERAL(42, 809, 23), // "BLUEBPS_CURRENT_TIMEOUT"
QT_MOC_LITERAL(43, 833, 14), // "BLUEBPS_EVENTS"
QT_MOC_LITERAL(44, 848, 19), // "BLUEBPS_CURRENT_MAX"
QT_MOC_LITERAL(45, 868, 27), // "BLUEBPS_TEMPERATURE_TIMEOUT"
QT_MOC_LITERAL(46, 896, 13), // "BLUEBPS_STATE"
QT_MOC_LITERAL(47, 910, 31), // "BLUEBPS_SET_TEMPERATURE_TIMEOUT"
QT_MOC_LITERAL(48, 942, 27), // "BLUEBPS_SET_TEMPERATURE_MAX"
QT_MOC_LITERAL(49, 970, 23), // "BLUEBPS_SET_CURRENT_MAX"
QT_MOC_LITERAL(50, 994, 27), // "BLUEBPS_SET_CURRENT_TIMEOUT"
QT_MOC_LITERAL(51, 1022, 32), // "BLUEBPS_SET_CELL_VOLTAGE_MINIMUM"
QT_MOC_LITERAL(52, 1055, 32), // "BLUEBPS_SET_CELL_VOLTAGE_TIMEOUT"
QT_MOC_LITERAL(53, 1088, 23), // "BLUEBPS_SET_LPF_SETTING"
QT_MOC_LITERAL(54, 1112, 23), // "BLUEBPS_SET_STREAM_RATE"
QT_MOC_LITERAL(55, 1136, 32), // "BLUEBPS_SET_LPF_SAMPLE_FREQUENCY"
QT_MOC_LITERAL(56, 1169, 22), // "BLUEBPS_RESET_DEFAULTS"
QT_MOC_LITERAL(57, 1192, 14), // "BLUEBPS_REBOOT"
QT_MOC_LITERAL(58, 1207, 19), // "BLUEBPS_ERASE_FLASH"
QT_MOC_LITERAL(59, 1227, 19), // "PING360_DEVICE_DATA"
QT_MOC_LITERAL(60, 1247, 24), // "PING360_AUTO_DEVICE_DATA"
QT_MOC_LITERAL(61, 1272, 17), // "PING360_DEVICE_ID"
QT_MOC_LITERAL(62, 1290, 13), // "PING360_RESET"
QT_MOC_LITERAL(63, 1304, 17), // "PING360_MOTOR_OFF"
QT_MOC_LITERAL(64, 1322, 21), // "PING360_AUTO_TRANSMIT"
QT_MOC_LITERAL(65, 1344, 18) // "PING360_TRANSDUCER"

    },
    "PingEnumNamespace\0PingDeviceType\0"
    "UNKNOWN\0PING1D\0PING360\0PINGHF\0"
    "PingMessageId\0PING1D_SPEED_OF_SOUND\0"
    "PING1D_MODE_AUTO\0PING1D_PROFILE\0"
    "PING1D_DISTANCE_SIMPLE\0PING1D_GENERAL_INFO\0"
    "PING1D_VOLTAGE_5\0PING1D_PING_INTERVAL\0"
    "PING1D_PROCESSOR_TEMPERATURE\0"
    "PING1D_PCB_TEMPERATURE\0PING1D_FIRMWARE_VERSION\0"
    "PING1D_DEVICE_ID\0PING1D_DISTANCE\0"
    "PING1D_GAIN_SETTING\0PING1D_TRANSMIT_DURATION\0"
    "PING1D_PING_ENABLE\0PING1D_RANGE\0"
    "PING1D_SET_MODE_AUTO\0PING1D_SET_PING_ENABLE\0"
    "PING1D_SET_GAIN_SETTING\0"
    "PING1D_SET_SPEED_OF_SOUND\0"
    "PING1D_SET_DEVICE_ID\0PING1D_SET_PING_INTERVAL\0"
    "PING1D_SET_RANGE\0PING1D_CONTINUOUS_START\0"
    "PING1D_GOTO_BOOTLOADER\0PING1D_CONTINUOUS_STOP\0"
    "COMMON_DEVICE_INFORMATION\0"
    "COMMON_PROTOCOL_VERSION\0COMMON_ACK\0"
    "COMMON_GENERAL_REQUEST\0COMMON_ASCII_TEXT\0"
    "COMMON_NACK\0BLUEBPS_CELL_VOLTAGE_MIN\0"
    "BLUEBPS_TEMPERATURE_MAX\0BLUEBPS_CELL_TIMEOUT\0"
    "BLUEBPS_CURRENT_TIMEOUT\0BLUEBPS_EVENTS\0"
    "BLUEBPS_CURRENT_MAX\0BLUEBPS_TEMPERATURE_TIMEOUT\0"
    "BLUEBPS_STATE\0BLUEBPS_SET_TEMPERATURE_TIMEOUT\0"
    "BLUEBPS_SET_TEMPERATURE_MAX\0"
    "BLUEBPS_SET_CURRENT_MAX\0"
    "BLUEBPS_SET_CURRENT_TIMEOUT\0"
    "BLUEBPS_SET_CELL_VOLTAGE_MINIMUM\0"
    "BLUEBPS_SET_CELL_VOLTAGE_TIMEOUT\0"
    "BLUEBPS_SET_LPF_SETTING\0BLUEBPS_SET_STREAM_RATE\0"
    "BLUEBPS_SET_LPF_SAMPLE_FREQUENCY\0"
    "BLUEBPS_RESET_DEFAULTS\0BLUEBPS_REBOOT\0"
    "BLUEBPS_ERASE_FLASH\0PING360_DEVICE_DATA\0"
    "PING360_AUTO_DEVICE_DATA\0PING360_DEVICE_ID\0"
    "PING360_RESET\0PING360_MOTOR_OFF\0"
    "PING360_AUTO_TRANSMIT\0PING360_TRANSDUCER"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PingEnumNamespace[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       2,   14, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x2,    4,   24,
       6,    6, 0x2,   59,   32,

 // enum data: key, value
       2, uint(PingEnumNamespace::PingDeviceType::UNKNOWN),
       3, uint(PingEnumNamespace::PingDeviceType::PING1D),
       4, uint(PingEnumNamespace::PingDeviceType::PING360),
       5, uint(PingEnumNamespace::PingDeviceType::PINGHF),
       7, uint(PingEnumNamespace::PingMessageId::PING1D_SPEED_OF_SOUND),
       8, uint(PingEnumNamespace::PingMessageId::PING1D_MODE_AUTO),
       9, uint(PingEnumNamespace::PingMessageId::PING1D_PROFILE),
      10, uint(PingEnumNamespace::PingMessageId::PING1D_DISTANCE_SIMPLE),
      11, uint(PingEnumNamespace::PingMessageId::PING1D_GENERAL_INFO),
      12, uint(PingEnumNamespace::PingMessageId::PING1D_VOLTAGE_5),
      13, uint(PingEnumNamespace::PingMessageId::PING1D_PING_INTERVAL),
      14, uint(PingEnumNamespace::PingMessageId::PING1D_PROCESSOR_TEMPERATURE),
      15, uint(PingEnumNamespace::PingMessageId::PING1D_PCB_TEMPERATURE),
      16, uint(PingEnumNamespace::PingMessageId::PING1D_FIRMWARE_VERSION),
      17, uint(PingEnumNamespace::PingMessageId::PING1D_DEVICE_ID),
      18, uint(PingEnumNamespace::PingMessageId::PING1D_DISTANCE),
      19, uint(PingEnumNamespace::PingMessageId::PING1D_GAIN_SETTING),
      20, uint(PingEnumNamespace::PingMessageId::PING1D_TRANSMIT_DURATION),
      21, uint(PingEnumNamespace::PingMessageId::PING1D_PING_ENABLE),
      22, uint(PingEnumNamespace::PingMessageId::PING1D_RANGE),
      23, uint(PingEnumNamespace::PingMessageId::PING1D_SET_MODE_AUTO),
      24, uint(PingEnumNamespace::PingMessageId::PING1D_SET_PING_ENABLE),
      25, uint(PingEnumNamespace::PingMessageId::PING1D_SET_GAIN_SETTING),
      26, uint(PingEnumNamespace::PingMessageId::PING1D_SET_SPEED_OF_SOUND),
      27, uint(PingEnumNamespace::PingMessageId::PING1D_SET_DEVICE_ID),
      28, uint(PingEnumNamespace::PingMessageId::PING1D_SET_PING_INTERVAL),
      29, uint(PingEnumNamespace::PingMessageId::PING1D_SET_RANGE),
      30, uint(PingEnumNamespace::PingMessageId::PING1D_CONTINUOUS_START),
      31, uint(PingEnumNamespace::PingMessageId::PING1D_GOTO_BOOTLOADER),
      32, uint(PingEnumNamespace::PingMessageId::PING1D_CONTINUOUS_STOP),
      33, uint(PingEnumNamespace::PingMessageId::COMMON_DEVICE_INFORMATION),
      34, uint(PingEnumNamespace::PingMessageId::COMMON_PROTOCOL_VERSION),
      35, uint(PingEnumNamespace::PingMessageId::COMMON_ACK),
      36, uint(PingEnumNamespace::PingMessageId::COMMON_GENERAL_REQUEST),
      37, uint(PingEnumNamespace::PingMessageId::COMMON_ASCII_TEXT),
      38, uint(PingEnumNamespace::PingMessageId::COMMON_NACK),
      39, uint(PingEnumNamespace::PingMessageId::BLUEBPS_CELL_VOLTAGE_MIN),
      40, uint(PingEnumNamespace::PingMessageId::BLUEBPS_TEMPERATURE_MAX),
      41, uint(PingEnumNamespace::PingMessageId::BLUEBPS_CELL_TIMEOUT),
      42, uint(PingEnumNamespace::PingMessageId::BLUEBPS_CURRENT_TIMEOUT),
      43, uint(PingEnumNamespace::PingMessageId::BLUEBPS_EVENTS),
      44, uint(PingEnumNamespace::PingMessageId::BLUEBPS_CURRENT_MAX),
      45, uint(PingEnumNamespace::PingMessageId::BLUEBPS_TEMPERATURE_TIMEOUT),
      46, uint(PingEnumNamespace::PingMessageId::BLUEBPS_STATE),
      47, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_TEMPERATURE_TIMEOUT),
      48, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_TEMPERATURE_MAX),
      49, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_CURRENT_MAX),
      50, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_CURRENT_TIMEOUT),
      51, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_CELL_VOLTAGE_MINIMUM),
      52, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_CELL_VOLTAGE_TIMEOUT),
      53, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_LPF_SETTING),
      54, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_STREAM_RATE),
      55, uint(PingEnumNamespace::PingMessageId::BLUEBPS_SET_LPF_SAMPLE_FREQUENCY),
      56, uint(PingEnumNamespace::PingMessageId::BLUEBPS_RESET_DEFAULTS),
      57, uint(PingEnumNamespace::PingMessageId::BLUEBPS_REBOOT),
      58, uint(PingEnumNamespace::PingMessageId::BLUEBPS_ERASE_FLASH),
      59, uint(PingEnumNamespace::PingMessageId::PING360_DEVICE_DATA),
      60, uint(PingEnumNamespace::PingMessageId::PING360_AUTO_DEVICE_DATA),
      61, uint(PingEnumNamespace::PingMessageId::PING360_DEVICE_ID),
      62, uint(PingEnumNamespace::PingMessageId::PING360_RESET),
      63, uint(PingEnumNamespace::PingMessageId::PING360_MOTOR_OFF),
      64, uint(PingEnumNamespace::PingMessageId::PING360_AUTO_TRANSMIT),
      65, uint(PingEnumNamespace::PingMessageId::PING360_TRANSDUCER),

       0        // eod
};

QT_INIT_METAOBJECT const QMetaObject PingEnumNamespace::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_PingEnumNamespace.data,
    qt_meta_data_PingEnumNamespace,
    nullptr,
    nullptr,
    nullptr
} };

QT_WARNING_POP
QT_END_MOC_NAMESPACE
