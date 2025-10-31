#include "fakekbd.h"

class VirtualKeyboardEmulator {
private:
    HANDLE hNamedPipe;
    std::atomic<bool> running;
    std::thread clientThread;
    std::vector<KeyEvent> keyBuffer;
    FakeKbdStatus status;

    void InitializeStatus() {
        status.keysProcessed = 0;
        status.bufferSize = 0;
        status.isConnected = FALSE;
        GetSystemTime(&status.lastActivity);
    }

public:
    VirtualKeyboardEmulator() : hNamedPipe(INVALID_HANDLE_VALUE), running(false) {
        InitializeStatus();
    }

    ~VirtualKeyboardEmulator() {
        Stop();
    }

    bool Start() {

        std::wstring pipeName = L"\\\\.\\pipe\\fakekbd";

        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL; 
        sa.bInheritHandle = FALSE;

        // Create named pipe (simulating device)
        hNamedPipe = CreateNamedPipe(
            pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            4096,  
            4096,  
            0,      
            &sa    
        );

        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create named pipe. Error: " << GetLastError() << std::endl;
            return false;
        }

        std::cout << "Virtual Keyboard Device started at \\\\.\\pipe\\fakekbd" << std::endl;
        running = true;
        clientThread = std::thread(&VirtualKeyboardEmulator::ClientHandler, this);
        return true;
    }

    void Stop() {
        running = false;

        if (hNamedPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hNamedPipe);
            hNamedPipe = INVALID_HANDLE_VALUE;
        }

        if (clientThread.joinable()) {
            clientThread.join();
        }

        std::cout << "Virtual Keyboard Device stopped." << std::endl;
    }

private:
    void ClientHandler() {
        while (running) {
            std::cout << "Waiting for client connection..." << std::endl;

            BOOL connected = ConnectNamedPipe(hNamedPipe, NULL);
            if (!connected) {
                DWORD error = GetLastError();
                if (error == ERROR_PIPE_CONNECTED) {
                    connected = TRUE;
                }
                else {
                    std::cout << "ConnectNamedPipe failed, retrying... Error: " << error << std::endl;
                    Sleep(1000);
                    continue;
                }
            }

            if (connected) {
                std::cout << "Client connected to virtual keyboard device." << std::endl;
                status.isConnected = TRUE;
                HandleClientCommunication();
                DisconnectNamedPipe(hNamedPipe);
                status.isConnected = FALSE;
                std::cout << "Client disconnected." << std::endl;
            }
        }
    }

    void HandleClientCommunication() {
        BYTE buffer[1024];
        DWORD bytesRead;

        while (running) {
            BOOL success = ReadFile(
                hNamedPipe,
                buffer,
                sizeof(buffer),
                &bytesRead,
                NULL
            );

            if (!success) {
                DWORD error = GetLastError();
                if (error == ERROR_BROKEN_PIPE) {
                    std::cout << "Client disconnected." << std::endl;
                    break;
                }
                else {
                    std::cout << "ReadFile failed. Error: " << error << std::endl;
                    break;
                }
            }

            if (bytesRead == 0) {
                continue; 
            }

            ProcessRequest(buffer, bytesRead);
        }
    }

    void ProcessRequest(BYTE* inputBuffer, DWORD inputSize) {
        if (inputSize < sizeof(DWORD)) {
            std::cout << "Received insufficient data for request." << std::endl;
            return;
        }

        DWORD requestType = *(DWORD*)inputBuffer;
        BYTE* data = inputBuffer + sizeof(DWORD);
        DWORD dataSize = inputSize - sizeof(DWORD);

        std::cout << "Processing request: 0x" << std::hex << requestType << std::endl;

        switch (requestType) {
        case IOCTL_FAKEKBD_SEND_KEYS:
            HandleSendKeys(data, dataSize);
            break;

        case IOCTL_FAKEKBD_GET_STATUS:
            HandleGetStatus();
            break;

        case IOCTL_FAKEKBD_CLEAR_BUFFER:
            HandleClearBuffer();
            break;

        default:
            std::cout << "Unknown request: 0x" << std::hex << requestType << std::endl;
            break;
        }

        GetSystemTime(&status.lastActivity);
    }

    void HandleSendKeys(BYTE* data, DWORD dataSize) {
        if (dataSize < sizeof(KeyEvent)) {
            std::cout << "Insufficient data for key event." << std::endl;
            return;
        }

        KeyEvent* keyEvent = (KeyEvent*)data;

        std::string action = keyEvent->keyDown ? "DOWN" : "UP";
        std::cout << "Virtual Key Event: VK=0x" << std::hex << (int)keyEvent->virtualKey
            << " ScanCode=0x" << (int)keyEvent->scanCode
            << " Action=" << action << std::endl;


        keyBuffer.push_back(*keyEvent);
        status.keysProcessed++;
        status.bufferSize = (DWORD)keyBuffer.size();


        DWORD bytesWritten;
        WriteFile(hNamedPipe, keyEvent, sizeof(KeyEvent), &bytesWritten, NULL);
    }

    void HandleGetStatus() {
        DWORD bytesWritten;
        WriteFile(hNamedPipe, &status, sizeof(status), &bytesWritten, NULL);
        std::cout << "Sent device status to client." << std::endl;
    }

    void HandleClearBuffer() {
        keyBuffer.clear();
        status.bufferSize = 0;
        std::cout << "Key buffer cleared." << std::endl;

        DWORD result = 1; 
        DWORD bytesWritten;
        WriteFile(hNamedPipe, &result, sizeof(result), &bytesWritten, NULL);
    }
};

int RunServer() {
    VirtualKeyboardEmulator device;

    if (!device.Start()) {
        return 1;
    }

    std::cout << "Device server running. Press Enter to stop..." << std::endl;
    std::cin.get();

    device.Stop();
    return 0;
}