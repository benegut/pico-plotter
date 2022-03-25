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
struct qt_meta_stringdata_XYZ_PicoChannelWindow_t {
    QByteArrayData data[8];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_XYZ_PicoChannelWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_XYZ_PicoChannelWindow_t qt_meta_stringdata_XYZ_PicoChannelWindow = {
    {
QT_MOC_LITERAL(0, 0, 21), // "XYZ_PicoChannelWindow"
QT_MOC_LITERAL(1, 22, 14), // "update_XY_Axis"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 19), // "start_Stream_Signal"
QT_MOC_LITERAL(4, 58, 18), // "stop_Stream_Signal"
QT_MOC_LITERAL(5, 77, 21), // "send_Unit_Data_Signal"
QT_MOC_LITERAL(6, 99, 4), // "UNIT"
QT_MOC_LITERAL(7, 104, 18) // "Update_Button_Slot"

    },
    "XYZ_PicoChannelWindow\0update_XY_Axis\0"
    "\0start_Stream_Signal\0stop_Stream_Signal\0"
    "send_Unit_Data_Signal\0UNIT\0"
    "Update_Button_Slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_XYZ_PicoChannelWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void XYZ_PicoChannelWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<XYZ_PicoChannelWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->update_XY_Axis(); break;
        case 1: _t->start_Stream_Signal(); break;
        case 2: _t->stop_Stream_Signal(); break;
        case 3: _t->send_Unit_Data_Signal((*reinterpret_cast< UNIT(*)>(_a[1]))); break;
        case 4: _t->Update_Button_Slot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< UNIT >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (XYZ_PicoChannelWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XYZ_PicoChannelWindow::update_XY_Axis)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (XYZ_PicoChannelWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XYZ_PicoChannelWindow::start_Stream_Signal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (XYZ_PicoChannelWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XYZ_PicoChannelWindow::stop_Stream_Signal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (XYZ_PicoChannelWindow::*)(UNIT );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XYZ_PicoChannelWindow::send_Unit_Data_Signal)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject XYZ_PicoChannelWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_XYZ_PicoChannelWindow.data,
    qt_meta_data_XYZ_PicoChannelWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *XYZ_PicoChannelWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *XYZ_PicoChannelWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_XYZ_PicoChannelWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int XYZ_PicoChannelWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void XYZ_PicoChannelWindow::update_XY_Axis()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void XYZ_PicoChannelWindow::start_Stream_Signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void XYZ_PicoChannelWindow::stop_Stream_Signal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void XYZ_PicoChannelWindow::send_Unit_Data_Signal(UNIT _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_Window_t {
    QByteArrayData data[18];
    char stringdata0[282];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Window_t qt_meta_stringdata_Window = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Window"
QT_MOC_LITERAL(1, 7, 21), // "send_Unit_Data_Signal"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "UNIT"
QT_MOC_LITERAL(4, 35, 19), // "start_Stream_Signal"
QT_MOC_LITERAL(5, 55, 18), // "stop_Stream_Signal"
QT_MOC_LITERAL(6, 74, 14), // "update_XY_Axis"
QT_MOC_LITERAL(7, 89, 28), // "show_XYZ_PicoChannelMenuSlot"
QT_MOC_LITERAL(8, 118, 4), // "data"
QT_MOC_LITERAL(9, 123, 17), // "streamButton_Slot"
QT_MOC_LITERAL(10, 141, 15), // "saveButton_Slot"
QT_MOC_LITERAL(11, 157, 16), // "videoButton_Slot"
QT_MOC_LITERAL(12, 174, 16), // "resetButton_Slot"
QT_MOC_LITERAL(13, 191, 13), // "setResolution"
QT_MOC_LITERAL(14, 205, 18), // "set_Z_Scale_Offset"
QT_MOC_LITERAL(15, 224, 21), // "set_Z_Scale_Amplitude"
QT_MOC_LITERAL(16, 246, 18), // "get_Unit_Data_Slot"
QT_MOC_LITERAL(17, 265, 16) // "stop_Stream_Slot"

    },
    "Window\0send_Unit_Data_Signal\0\0UNIT\0"
    "start_Stream_Signal\0stop_Stream_Signal\0"
    "update_XY_Axis\0show_XYZ_PicoChannelMenuSlot\0"
    "data\0streamButton_Slot\0saveButton_Slot\0"
    "videoButton_Slot\0resetButton_Slot\0"
    "setResolution\0set_Z_Scale_Offset\0"
    "set_Z_Scale_Amplitude\0get_Unit_Data_Slot\0"
    "stop_Stream_Slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Window[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06 /* Public */,
       4,    0,   92,    2, 0x06 /* Public */,
       5,    0,   93,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   94,    2, 0x0a /* Public */,
       7,    0,   95,    2, 0x0a /* Public */,
       8,    4,   96,    2, 0x0a /* Public */,
       9,    0,  105,    2, 0x0a /* Public */,
      10,    0,  106,    2, 0x0a /* Public */,
      11,    0,  107,    2, 0x0a /* Public */,
      12,    0,  108,    2, 0x0a /* Public */,
      13,    1,  109,    2, 0x0a /* Public */,
      14,    1,  112,    2, 0x0a /* Public */,
      15,    1,  115,    2, 0x0a /* Public */,
      16,    1,  118,    2, 0x0a /* Public */,
      17,    0,  121,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

       0        // eod
};

void Window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->send_Unit_Data_Signal((*reinterpret_cast< UNIT(*)>(_a[1]))); break;
        case 1: _t->start_Stream_Signal(); break;
        case 2: _t->stop_Stream_Signal(); break;
        case 3: _t->update_XY_Axis(); break;
        case 4: _t->show_XYZ_PicoChannelMenuSlot(); break;
        case 5: _t->data((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 6: _t->streamButton_Slot(); break;
        case 7: _t->saveButton_Slot(); break;
        case 8: _t->videoButton_Slot(); break;
        case 9: _t->resetButton_Slot(); break;
        case 10: _t->setResolution((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->set_Z_Scale_Offset((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->set_Z_Scale_Amplitude((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->get_Unit_Data_Slot((*reinterpret_cast< UNIT(*)>(_a[1]))); break;
        case 14: _t->stop_Stream_Slot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< UNIT >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< UNIT >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Window::*)(UNIT );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Window::send_Unit_Data_Signal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Window::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Window::start_Stream_Signal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Window::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Window::stop_Stream_Signal)) {
                *result = 2;
                return;
            }
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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Window::send_Unit_Data_Signal(UNIT _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Window::start_Stream_Signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Window::stop_Stream_Signal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
