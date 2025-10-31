#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <vector>

#define FAKEKBD_DEVICE_NAME L"\\\\.\\fakekbd"

// IOCTL codes for the vd (Virtual device)
#define IOCTL_FAKEKBD_SEND_KEYS     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FAKEKBD_GET_STATUS    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FAKEKBD_CLEAR_BUFFER  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct FakeKbdStatus {
    DWORD keysProcessed;
    DWORD bufferSize;
    BOOL  isConnected;
    SYSTEMTIME lastActivity;
};

struct KeyEvent {
    BYTE virtualKey;
    BYTE scanCode;
    BOOL keyDown;
    DWORD timestamp;
};


class VirtualKeyboardEmulator;
class KeyboardClient;


void RunDemo();