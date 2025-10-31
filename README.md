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

```bash
vd server
vd client