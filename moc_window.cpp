/****************************************************************************
** Meta object code from reading C++ file 'window.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "window.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Window_t {
    QByteArrayData data[14];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Window_t qt_meta_stringdata_Window = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Window"
QT_MOC_LITERAL(1, 7, 9), // "setXYMode"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 5), // "UNIT*"
QT_MOC_LITERAL(4, 24, 10), // "setXYZMode"
QT_MOC_LITERAL(5, 35, 13), // "setNormalMode"
QT_MOC_LITERAL(6, 49, 4), // "data"
QT_MOC_LITERAL(7, 54, 15), // "QVector<double>"
QT_MOC_LITERAL(8, 70, 10), // "changeAxis"
QT_MOC_LITERAL(9, 81, 9), // "resetPlot"
QT_MOC_LITERAL(10, 91, 11), // "resetCanvas"
QT_MOC_LITERAL(11, 103, 9), // "setPeriod"
QT_MOC_LITERAL(12, 113, 18), // "setResetCycleStart"
QT_MOC_LITERAL(13, 132, 13) // "setResolution"

    },
    "Window\0setXYMode\0\0UNIT*\0setXYZMode\0"
    "setNormalMode\0data\0QVector<double>\0"
    "changeAxis\0resetPlot\0resetCanvas\0"
    "setPeriod\0setResetCycleStart\0setResolution"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Window[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x0a /* Public */,
       4,    1,   82,    2, 0x0a /* Public */,
       5,    1,   85,    2, 0x0a /* Public */,
       6,    3,   88,    2, 0x0a /* Public */,
       6,    2,   95,    2, 0x0a /* Public */,
       6,    3,  100,    2, 0x0a /* Public */,
       6,    2,  107,    2, 0x0a /* Public */,
       8,    1,  112,    2, 0x0a /* Public */,
       9,    1,  115,    2, 0x0a /* Public */,
      10,    0,  118,    2, 0x0a /* Public */,
      11,    1,  119,    2, 0x0a /* Public */,
      12,    0,  122,    2, 0x0a /* Public */,
      13,    1,  123,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double,    2,    2,    2,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    2,    2,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 7, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 7,    2,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void Window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setXYMode((*reinterpret_cast< UNIT*(*)>(_a[1]))); break;
        case 1: _t->setXYZMode((*reinterpret_cast< UNIT*(*)>(_a[1]))); break;
        case 2: _t->setNormalMode((*reinterpret_cast< UNIT*(*)>(_a[1]))); break;
        case 3: _t->data((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 4: _t->data((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: _t->data((*reinterpret_cast< QVector<double>(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: _t->data((*reinterpret_cast< QVector<double>(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 7: _t->changeAxis((*reinterpret_cast< UNIT*(*)>(_a[1]))); break;
        case 8: _t->resetPlot((*reinterpret_cast< UNIT*(*)>(_a[1]))); break;
        case 9: _t->resetCanvas(); break;
        case 10: _t->setPeriod((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->setResetCycleStart(); break;
        case 12: _t->setResolution((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Window::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_Window.data,
    qt_meta_data_Window,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Window.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
