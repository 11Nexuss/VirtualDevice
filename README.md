# 🧠 VirtualDevice

A **virtual keyboard device emulator** written in C++.  
Simulates a keyboard input driver using Windows **Named Pipes**, enabling interaction between a “device server” and “client” application.

---

## ⚙️ Features
- Simulates a virtual keyboard device entirely in user mode  
- Uses a simple named pipe protocol (`\\.\pipe\fakekbd`)  
- Supports:
  - Sending virtual key events
  - Querying device status
  - Clearing key buffers  
- Demonstrates low-level IPC (inter-process communication) and emulated IOCTL request handling

---

## 🧩 Project Structure

| File | Description |
|------|--------------|
| `main.cpp` | Entry point — chooses between server/client mode |
| `fakekbd.h` | Shared header defining device interface, IOCTL codes, and data structures |
| `server.cpp` | Implements the virtual keyboard device emulator (Named Pipe server) |
| `client.cpp` | Implements client to communicate with the virtual device |

---

## 🛠️ Installation

### Option 1 — Build from source
1. Open **Visual Studio 2022** (or later).
2. Create a new **C++ Console Application** project.
3. Add the provided source files:
   - `main.cpp`
   - `server.cpp`
   - `client.cpp`
   - `fakekbd.h`
4. Make sure your project is using:
   - **Windows SDK** (10.0 or newer)
   - **x64 configuration**
5. Build the project.

### Option 2 — Use precompiled binary
Download the precompiled `vd.exe` (if provided) and place it in your working directory.

---

## ▶️ Usage

Once built, open **Command Prompt** in the executable directory and run:

vd server

This starts the **virtual keyboard device server**, which acts as the device backend.  
You’ll see output like:

=== Virtual Keyboard Device Emulator ===  
Virtual Keyboard Device started at \\.\pipe\fakekbd  
Waiting for client connection...

Then, in a **separate terminal**, run:

vd client

This starts the **client**, which connects to the virtual device and performs the following:
- Sends several simulated key events (`A` and `B`)
- Requests and prints the device status
- Clears the key buffer
- Displays updated status

You’ll see output similar to:

Connected to virtual keyboard device successfully.

Sending key events...  
Key event sent successfully.

=== Device Status ===  
Keys Processed: 4  
Buffer Size: 4  
Connected: Yes  
Last Activity: 14:52:33  

Clearing buffer...  
Buffer cleared successfully.  

Getting device status after clear...  
Keys Processed: 4  
Buffer Size: 0  
Connected: Yes  
Last Activity: 14:52:34  

To stop the **server**, simply press **Enter** in its terminal window.

---

## 🧠 How It Works

This project emulates a **virtual keyboard device** using Windows **Named Pipes** instead of kernel-mode drivers.

### Server (Device Emulator)
- Creates a named pipe at `\\.\pipe\fakekbd`
- Waits for client connections
- Handles incoming IOCTL-style requests:
  - `IOCTL_FAKEKBD_SEND_KEYS`
  - `IOCTL_FAKEKBD_GET_STATUS`
  - `IOCTL_FAKEKBD_CLEAR_BUFFER`
- Maintains device state (`FakeKbdStatus`), including:
  - Keys processed
  - Buffer size
  - Connection status
  - Last activity time

### Client
- Connects to the pipe as if it were a device handle  
- Sends and receives structured requests/responses  
- Demonstrates how IOCTL-like communication works between a user-mode client and virtual driver

---

## ⚠️ Notes

- **Windows only** — requires WinAPI functions and Named Pipes  
- This is **not a real keyboard driver** — it runs fully in user mode for demonstration  
- Safe to use; does **not inject or send actual keypresses** to the OS  
- Great for learning about:
  - Device-driver-like IPC
  - IOCTL command handling
  - User-mode driver simulation

---

## 🧑‍💻 Developer Tips

- Extend the IOCTL system to handle new commands (e.g., simulate macros or custom packets).
- Implement a logging layer for better debugging of requests/responses.
- Replace the named pipe transport with TCP/IP for remote testing.
- Wrap the logic in a DLL for integration into larger projects.

---

## 📄 License

This project is distributed under the **MIT License**.  
You may freely modify, distribute, and use it for research, educational, or legitimate automation purposes.
s