/****************************************************************************
** Meta object code from reading C++ file 'pingsensor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../teleportalQT/src/inc/pingsensor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pingsensor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PingSensor_t {
    QByteArrayData data[11];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PingSensor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PingSensor_t qt_meta_stringdata_PingSensor = {
    {
QT_MOC_LITERAL(0, 0, 10), // "PingSensor"
QT_MOC_LITERAL(1, 11, 19), // "parserErrorsChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 15), // "connectionClose"
QT_MOC_LITERAL(4, 48, 14), // "connectionOpen"
QT_MOC_LITERAL(5, 63, 17), // "connectionChanged"
QT_MOC_LITERAL(6, 81, 11), // "linkChanged"
QT_MOC_LITERAL(7, 93, 25), // "distanceConfidenceChanged"
QT_MOC_LITERAL(8, 119, 20), // "handleMessagePrivate"
QT_MOC_LITERAL(9, 140, 12), // "ping_message"
QT_MOC_LITERAL(10, 153, 3) // "msg"

    },
    "PingSensor\0parserErrorsChanged\0\0"
    "connectionClose\0connectionOpen\0"
    "connectionChanged\0linkChanged\0"
    "distanceConfidenceChanged\0"
    "handleMessagePrivate\0ping_message\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PingSensor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,
       6,    0,   53,    2, 0x06 /* Public */,
       7,    0,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void PingSensor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PingSensor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->parserErrorsChanged(); break;
        case 1: _t->connectionClose(); break;
        case 2: _t->connectionOpen(); break;
        case 3: _t->connectionChanged(); break;
        case 4: _t->linkChanged(); break;
        case 5: _t->distanceConfidenceChanged(); break;
        case 6: _t->handleMessagePrivate((*reinterpret_cast< const ping_message(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PingSensor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PingSensor::parserErrorsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PingSensor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PingSensor::connectionClose)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PingSensor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PingSensor::connectionOpen)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PingSensor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PingSensor::connectionChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PingSensor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PingSensor::linkChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PingSensor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PingSensor::distanceConfidenceChanged)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PingSensor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PingSensor.data,
    qt_meta_data_PingSensor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PingSensor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PingSensor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PingSensor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PingSensor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void PingSensor::parserErrorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PingSensor::connectionClose()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PingSensor::connectionOpen()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PingSensor::connectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PingSensor::linkChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PingSensor::distanceConfidenceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
