# C++ Order Matching Engine

A fast and lightweight multithreaded order matching engine written in modern C++.  
Implements common order types like **GTC (Good Till Cancelled)**, **IOC (Immediate Or Cancel)**, and **FOK (Fill Or Kill)**.  
Designed for deterministic price-time priority matching and efficient replay from logs.

---

## Features
- Deterministic price-time matching logic
- Cancel/replace support
- Write-ahead logging (WAL) for durability
- Replay capability to rebuild state
- Unit tests for core matching logic

---

## Project Structure
```
cpp-order-matching-engine/
├── include/           # Public headers
├── src/               # Source files
├── tests/             # Unit tests
├── docs/              # Design docs & diagrams
├── CMakeLists.txt     # Build configuration
└── README.md          # Project overview
```

---

## ️ Getting Started

### Prerequisites
- C++20 or newer
- CMake 3.20+
- A compiler (g++/clang/MSVC)

---

## Roadmap
- [ ] Add multithreaded event loop
- [ ] Implement TCP/JSON API for external clients
- [ ] Add performance benchmarks
- [ ] Expand order types (market, stop, stop-limit)

---

## License
This project is licensed under the MIT License.
