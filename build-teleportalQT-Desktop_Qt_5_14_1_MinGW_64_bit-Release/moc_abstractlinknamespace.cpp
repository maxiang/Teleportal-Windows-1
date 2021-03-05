/****************************************************************************
** Meta object code from reading C++ file 'abstractlinknamespace.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../teleportalQT/dependencies/link/abstractlinknamespace.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstractlinknamespace.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AbstractLinkNamespace_t {
    QByteArrayData data[10];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AbstractLinkNamespace_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AbstractLinkNamespace_t qt_meta_stringdata_AbstractLinkNamespace = {
    {
QT_MOC_LITERAL(0, 0, 21), // "AbstractLinkNamespace"
QT_MOC_LITERAL(1, 22, 8), // "LinkType"
QT_MOC_LITERAL(2, 31, 4), // "None"
QT_MOC_LITERAL(3, 36, 4), // "File"
QT_MOC_LITERAL(4, 41, 6), // "Serial"
QT_MOC_LITERAL(5, 48, 3), // "Udp"
QT_MOC_LITERAL(6, 52, 3), // "Tcp"
QT_MOC_LITERAL(7, 56, 16), // "Ping1DSimulation"
QT_MOC_LITERAL(8, 73, 17), // "Ping360Simulation"
QT_MOC_LITERAL(9, 91, 4) // "Last"

    },
    "AbstractLinkNamespace\0LinkType\0None\0"
    "File\0Serial\0Udp\0Tcp\0Ping1DSimulation\0"
    "Ping360Simulation\0Last"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AbstractLinkNamespace[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x0,    8,   19,

 // enum data: key, value
       2, uint(AbstractLinkNamespace::None),
       3, uint(AbstractLinkNamespace::File),
       4, uint(AbstractLinkNamespace::Serial),
       5, uint(AbstractLinkNamespace::Udp),
       6, uint(AbstractLinkNamespace::Tcp),
       7, uint(AbstractLinkNamespace::Ping1DSimulation),
       8, uint(AbstractLinkNamespace::Ping360Simulation),
       9, uint(AbstractLinkNamespace::Last),

       0        // eod
};

QT_INIT_METAOBJECT const QMetaObject AbstractLinkNamespace::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_AbstractLinkNamespace.data,
    qt_meta_data_AbstractLinkNamespace,
    nullptr,
    nullptr,
    nullptr
} };

QT_WARNING_POP
QT_END_MOC_NAMESPACE
