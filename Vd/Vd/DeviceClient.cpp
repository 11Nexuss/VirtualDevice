#include "fakekbd.h"

class KeyboardClient {
private:
    HANDLE hDevice;

    bool SendRequest(DWORD requestType, const void* inputData = nullptr, DWORD inputSize = 0, void* outputData = nullptr, DWORD outputSize = 0) {

        std::vector<BYTE> buffer(sizeof(DWORD) + inputSize);
        *(DWORD*)buffer.data() = requestType;

        if (inputData && inputSize > 0) {
            memcpy(buffer.data() + sizeof(DWORD), inputData, inputSize);
        }


        DWORD bytesWritten;
        BOOL success = WriteFile(
            hDevice,
            buffer.data(),
            (DWORD)buffer.size(),
            &bytesWritten,
            NULL
        );

        if (!success) {
            std::cerr << "Failed to send request. Error: " << GetLastError() << std::endl;
            return false;
        }


        if (outputData && outputSize > 0) {
            DWORD bytesRead;
            success = ReadFile(
                hDevice,
                outputData,
                outputSize,
                &bytesRead,
                NULL
            );

            if (!success) {
                std::cerr << "Failed to read response. Error: " << GetLastError() << std::endl;
                return false;
            }
        }

        return true;
    }

public:
    KeyboardClient() : hDevice(INVALID_HANDLE_VALUE) {}

    ~KeyboardClient() {
        Close();
    }

    bool Connect() {
        std::wstring pipeName = L"\\\\.\\pipe\\fakekbd";

        hDevice = CreateFile(
            pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, 
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (hDevice == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            std::cerr << "Failed to connect to virtual keyboard device. Error: " << error << std::endl;
            if (error == ERROR_PIPE_BUSY) {
                std::cerr << "Pipe is busy. Another client might be connected." << std::endl;
            }
            else if (error == ERROR_FILE_NOT_FOUND) {
                std::cerr << "Pipe not found. Make sure the server is running." << std::endl;
            }
            return false;
        }

        std::cout << "Connected to virtual keyboard device successfully." << std::endl;

        // Set pipe to message mode
        DWORD mode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(hDevice, &mode, NULL, NULL)) {
            std::cerr << "Failed to set pipe handle state. Error: " << GetLastError() << std::endl;
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            return false;
        }

        return true;
    }

    void Close() {
        if (hDevice != INVALID_HANDLE_VALUE) {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
        }
    }

    bool SendKeyEvent(BYTE virtualKey, BYTE scanCode, BOOL keyDown) {
        KeyEvent keyEvent;
        keyEvent.virtualKey = virtualKey;
        keyEvent.scanCode = scanCode;
        keyEvent.keyDown = keyDown;
        keyEvent.timestamp = GetTickCount();

        KeyEvent response;
        bool success = SendRequest(IOCTL_FAKEKBD_SEND_KEYS, &keyEvent, sizeof(keyEvent), &response, sizeof(response));

        if (success) {
            std::cout << "Key event sent successfully." << std::endl;
            return true;
        }
        else {
            std::cerr << "Failed to send key event." << std::endl;
            return false;
        }
    }

    bool GetDeviceStatus() {
        FakeKbdStatus status;
        bool success = SendRequest(IOCTL_FAKEKBD_GET_STATUS, nullptr, 0, &status, sizeof(status));

        if (success) {
            std::cout << "=== Device Status ===" << std::endl;
            std::cout << "Keys Processed: " << status.keysProcessed << std::endl;
            std::cout << "Buffer Size: " << status.bufferSize << std::endl;
            std::cout << "Connected: " << (status.isConnected ? "Yes" : "No") << std::endl;
            std::cout << "Last Activity: " << status.lastActivity.wHour << ":"
                << status.lastActivity.wMinute << ":" << status.lastActivity.wSecond << std::endl;
            return true;
        }
        else {
            std::cerr << "Failed to get device status." << std::endl;
            return false;
        }
    }

    bool ClearBuffer() {
        DWORD result;
        bool success = SendRequest(IOCTL_FAKEKBD_CLEAR_BUFFER, nullptr, 0, &result, sizeof(result));

        if (success && result) {
            std::cout << "Buffer cleared successfully." << std::endl;
            return true;
        }
        else {
            std::cerr << "Failed to clear buffer." << std::endl;
            return false;
        }
    }
};


void RunDemo() {
    KeyboardClient client;

    if (!client.Connect()) {
        return;
    }


    std::cout << "\nSending key events..." << std::endl;
    client.SendKeyEvent(0x41, 0x1E, TRUE);  // A key down
    client.SendKeyEvent(0x41, 0x1E, FALSE); // A key up
    client.SendKeyEvent(0x42, 0x30, TRUE);  // B key down
    client.SendKeyEvent(0x42, 0x30, FALSE); // B key up


    std::cout << "\nGetting device status..." << std::endl;
    client.GetDeviceStatus();

    std::cout << "\nClearing buffer..." << std::endl;
    client.ClearBuffer();


    std::cout << "\nGetting device status after clear..." << std::endl;
    client.GetDeviceStatus();

    client.Close();
}


int RunClient() {
    RunDemo();
    return 0;
}