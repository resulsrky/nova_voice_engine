# 🎙️ NovaEngine Voice Engine

NovaEngine Voice Engine is a high-performance, real-time audio transmission module designed for peer-to-peer (P2P) communication with ultra-low latency and minimal packet loss. Built as part of the NovaEngine media stack, this voice subsystem enables efficient, encrypted, and adaptive audio streaming over unstable or NAT-restricted networks.

---

## ✨ Features

- 🔄 **Full-Duplex Audio Streaming** – Real-time bi-directional audio communication
- 🎛️ **Adaptive Jitter Buffer** – Handles network jitter and delays with dynamic buffering
- 🔊 **Opus / PCM Audio Codecs** – Choose between compressed (Opus) or raw (PCM) audio
- 🌐 **P2P UDP Transmission** – Bypasses centralized servers using UDP hole punching
- 🧩 **Packet Slicing and Reassembly** – Supports MTU-friendly audio fragmentation
- ♻️ **Multipath Redundancy** – Simultaneously sends voice packets over multiple tunnels
- 🔐 **Optional Encryption** – AES-256 or custom symmetric encryption for secure transport
- 📦 **Loss Recovery** – Optional Forward Error Correction (FEC) support

---

## 📦 Architecture Overview

```text
+---------------------------+
|  Microphone / Audio Input|
+---------------------------+
             ↓
     +----------------+
     |  Audio Encoder |
     +----------------+
             ↓
  +------------------------+
  |  Slicer & Packetizer  |
  +------------------------+
             ↓
  +------------------------+
  |  UDP Sender (Multi-Tunnel) |
  +------------------------+

      <<< NETWORK TRANSFER >>>

  +------------------------+
  |  UDP Receiver (Smart Collector) |
  +------------------------+
             ↓
    +--------------------+
    |  Jitter Buffer     |
    +--------------------+
             ↓
     +----------------+
     | Audio Decoder  |
     +----------------+
             ↓
     +----------------+
     |  Speaker Out   |
     +----------------+

⚙️ Build Instructions
Prerequisites
C++17 or later

CMake 3.15+

OpenCV (for timing/logging/debug UI)

PortAudio (for cross-platform audio I/O)

libopus (optional for Opus support)

Build

git clone https://github.com/abkarada/NovaEngine.git
cd NovaEngine
mkdir build && cd build
cmake ..
make -j$(nproc)
🔧 Runtime Parameters
Example:
./novaengine_voice 192.168.1.5 5000 5001 5002
192.168.1.5 → target IP

5000, 5001, 5002 → multiple UDP tunnels (redundant delivery)

🧠 Design Philosophy
NovaEngine Voice Engine was built with the following principles:

Low latency beats everything.

Loss tolerance must be adaptive.

No server should stand between peers.

Performance should scale to unstable networks.

Encryption and jitter control are first-class citizens.

📁 Project Structure
voice_engine/
├── audio_input.cpp / .hpp       # Captures microphone input
├── audio_output.cpp / .hpp      # Handles speaker output
├── encoder.cpp / decoder.cpp    # Opus/PCM codec implementation
├── udp_sender.cpp / receiver.cpp# Custom UDP burst transmission
├── slicer.cpp                   # Frame slicing for MTU alignment
├── smart_collector.cpp         # Packet reordering & loss masking
├── jitter_buffer.cpp           # Adaptive jitter control
└── main_voice.cpp              # Main entry point for sender/receiver
📜 License
MIT License. See LICENSE for details.

🤝 Credits
Developed as part of the SafeRoom Project and NovaEngine by Abdurrahman Karadağ and contributors.

💡 Coming Soon
VAD (Voice Activity Detection)

Live encryption key rotation

💡 New Features
Echo cancellation (AEC)

Noise suppression module


Let me know if you want this in lightweight markdown format, or if you'd like a split into separate `README`, `INSTALL.md`, and `ARCHITECTURE.md` files for documentation clarity.

