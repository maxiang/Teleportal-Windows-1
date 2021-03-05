/****************************************************************************
** Meta object code from reading C++ file 'linkconfiguration.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../teleportalQT/dependencies/link/linkconfiguration.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'linkconfiguration.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LinkConfiguration_t {
    QByteArrayData data[29];
    char stringdata0[391];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LinkConfiguration_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LinkConfiguration_t qt_meta_stringdata_LinkConfiguration = {
    {
QT_MOC_LITERAL(0, 0, 17), // "LinkConfiguration"
QT_MOC_LITERAL(1, 18, 20), // "configurationChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 11), // "argsAsConst"
QT_MOC_LITERAL(4, 52, 10), // "deviceType"
QT_MOC_LITERAL(5, 63, 33), // "PingEnumNamespace::PingDevice..."
QT_MOC_LITERAL(6, 97, 16), // "createConfString"
QT_MOC_LITERAL(7, 114, 5), // "error"
QT_MOC_LITERAL(8, 120, 24), // "LinkConfiguration::Error"
QT_MOC_LITERAL(9, 145, 13), // "errorToString"
QT_MOC_LITERAL(10, 159, 7), // "isValid"
QT_MOC_LITERAL(11, 167, 4), // "name"
QT_MOC_LITERAL(12, 172, 14), // "serialBaudrate"
QT_MOC_LITERAL(13, 187, 4), // "type"
QT_MOC_LITERAL(14, 192, 31), // "AbstractLinkNamespace::LinkType"
QT_MOC_LITERAL(15, 224, 12), // "isSimulation"
QT_MOC_LITERAL(16, 237, 12), // "typeToString"
QT_MOC_LITERAL(17, 250, 15), // "isCompanionPort"
QT_MOC_LITERAL(18, 266, 6), // "string"
QT_MOC_LITERAL(19, 273, 5), // "Error"
QT_MOC_LITERAL(20, 279, 20), // "MissingConfiguration"
QT_MOC_LITERAL(21, 300, 8), // "NoErrors"
QT_MOC_LITERAL(22, 309, 6), // "NoType"
QT_MOC_LITERAL(23, 316, 11), // "InvalidType"
QT_MOC_LITERAL(24, 328, 6), // "NoArgs"
QT_MOC_LITERAL(25, 335, 17), // "InvalidArgsNumber"
QT_MOC_LITERAL(26, 353, 12), // "ArgsAreEmpty"
QT_MOC_LITERAL(27, 366, 10), // "InvalidUrl"
QT_MOC_LITERAL(28, 377, 13) // "InvalidSubnet"

    },
    "LinkConfiguration\0configurationChanged\0"
    "\0argsAsConst\0deviceType\0"
    "PingEnumNamespace::PingDeviceType\0"
    "createConfString\0error\0LinkConfiguration::Error\0"
    "errorToString\0isValid\0name\0serialBaudrate\0"
    "type\0AbstractLinkNamespace::LinkType\0"
    "isSimulation\0typeToString\0isCompanionPort\0"
    "string\0Error\0MissingConfiguration\0"
    "NoErrors\0NoType\0InvalidType\0NoArgs\0"
    "InvalidArgsNumber\0ArgsAreEmpty\0"
    "InvalidUrl\0InvalidSubnet"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LinkConfiguration[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       1,   92, // properties
       1,   96, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       3,    0,   80,    2, 0x02 /* Public */,
       4,    0,   81,    2, 0x02 /* Public */,
       6,    0,   82,    2, 0x02 /* Public */,
       7,    0,   83,    2, 0x02 /* Public */,
       9,    0,   84,    2, 0x02 /* Public */,
      10,    0,   85,    2, 0x02 /* Public */,
      11,    0,   86,    2, 0x02 /* Public */,
      12,    0,   87,    2, 0x02 /* Public */,
      13,    0,   88,    2, 0x02 /* Public */,
      15,    0,   89,    2, 0x02 /* Public */,
      16,    0,   90,    2, 0x02 /* Public */,
      17,    0,   91,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::QStringList,
    0x80000000 | 5,
    QMetaType::QString,
    0x80000000 | 8,
    QMetaType::QString,
    QMetaType::Bool,
    QMetaType::QString,
    QMetaType::Int,
    0x80000000 | 14,
    QMetaType::Bool,
    QMetaType::QString,
    QMetaType::Bool,

 // properties: name, type, flags
      18, QMetaType::QString, 0x00495001,

 // properties: notify_signal_id
       0,

 // enums: name, alias, flags, count, data
      19,   19, 0x0,    9,  101,

 // enum data: key, value
      20, uint(LinkConfiguration::MissingConfiguration),
      21, uint(LinkConfiguration::NoErrors),
      22, uint(LinkConfiguration::NoType),
      23, uint(LinkConfiguration::InvalidType),
      24, uint(LinkConfiguration::NoArgs),
      25, uint(LinkConfiguration::InvalidArgsNumber),
      26, uint(LinkConfiguration::ArgsAreEmpty),
      27, uint(LinkConfiguration::InvalidUrl),
      28, uint(LinkConfiguration::InvalidSubnet),

       0        // eod
};

void LinkConfiguration::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LinkConfiguration *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->configurationChanged(); break;
        case 1: { QStringList _r = _t->argsAsConst();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 2: { PingEnumNamespace::PingDeviceType _r = _t->deviceType();
            if (_a[0]) *reinterpret_cast< PingEnumNamespace::PingDeviceType*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->createConfString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { LinkConfiguration::Error _r = _t->error();
            if (_a[0]) *reinterpret_cast< LinkConfiguration::Error*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->errorToString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->isValid();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { QString _r = _t->name();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 8: { int _r = _t->serialBaudrate();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 9: { AbstractLinkNamespace::LinkType _r = _t->type();
            if (_a[0]) *reinterpret_cast< AbstractLinkNamespace::LinkType*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->isSimulation();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { QString _r = _t->typeToString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->isCompanionPort();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LinkConfiguration::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LinkConfiguration::configurationChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<LinkConfiguration *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->createFullConfString(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject LinkConfiguration::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LinkConfiguration.data,
    qt_meta_data_LinkConfiguration,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LinkConfiguration::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LinkConfiguration::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LinkConfiguration.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LinkConfiguration::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void LinkConfiguration::configurationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
