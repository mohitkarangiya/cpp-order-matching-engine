# C++ Multithreaded Order Matching Engine

A high-performance, multithreaded order matching engine built in modern C++20. Implements deterministic price-time priority matching with concurrent order processing across multiple symbols using lock-free data structures.

## 🚀 Key Features

### Core Trading Operations
- **Order Types**: GTC (Good Till Cancelled) - IOC and FOK support planned
- **Operations**: Add, Cancel, and Modify orders with full thread safety
- **Matching Logic**: Deterministic price-time priority with partial fill support
- **Multi-Symbol**: Concurrent processing of different trading symbols

### High-Performance Architecture
- **Lock-Free Queues**: Custom SPSC (Single Producer Single Consumer) ring buffers
- **Message-Based Design**: Type-safe variant system using `std::variant` for order operations
- **Thread-per-Symbol**: Dedicated worker threads for each symbol to maximize throughput
- **Zero-Copy Messaging**: Efficient inter-thread communication without locks

### Modern C++ Design
- **C++20 Features**: `std::jthread`, `std::variant`, structured bindings
- **Type Safety**: Compile-time message type verification
- **RAII**: Automatic resource management and clean shutdown
- **Template-Based**: Generic, reusable components

## 🏗️ Architecture

```
[Orders] → [Router] → [Symbol Queues] → [Symbol Workers] → [OrderBooks]
                           ↓
              [Lock-Free SPSC Ring Buffer]
```

### Components

- **Router**: Routes order messages to appropriate symbol queues based on symbol ID
- **SymbolWorker**: Processes orders for a specific symbol in dedicated thread
- **OrderBook**: Maintains bid/ask price levels with FIFO order queues
- **Message System**: Variant-based messaging supporting Add/Cancel/Modify operations

## 📊 Performance Characteristics

- **Concurrency**: Multiple symbols processed simultaneously
- **Memory Efficient**: Lock-free data structures minimize cache contention
- **Scalable**: Linear performance scaling with number of symbol worker threads
- **Low Latency**: Direct memory access patterns, no dynamic allocation in hot paths

## 🛠️ Technical Implementation

### Message Types
```cpp
using OrderMessage = std::variant<AddOrderMessage, ModifyOrderMessage, CancelOrderMessage>;
```

### Lock-Free Queue
Custom implementation using atomic operations with memory ordering guarantees for thread safety without blocking.

### Thread Management
Automatic lifecycle management using `std::jthread` with clean shutdown coordination.

## 📁 Project Structure
```
cpp-order-matching-engine/
├── include/
│   ├── core/             # Core trading engine headers
│   │   ├── Order.hpp     # Order data structures
│   │   ├── OrderBook.hpp # Price-time priority matching
│   │   ├── OrderMessage.hpp # Message variant system
│   │   ├── Router.hpp    # Message routing logic
│   │   └── SymbolWorker.hpp # Per-symbol worker threads
│   └── utils/            # Utility components
│       └── spsc_ring_buffer.hpp # Lock-free SPSC queue
├── src/
│   ├── core/             # Implementation files
│   └── main.cpp          # Test program
└── CMakeLists.txt        # Build configuration
```

## 🚀 Getting Started

### Prerequisites
- C++20 compatible compiler (GCC 11+, Clang 12+, MSVC 2022+)
- CMake 3.20+

### Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Run
```bash
./MainProgram
```

## 🔬 Testing

The system includes comprehensive logging to verify:
- Message routing correctness
- Order matching accuracy
- Trade execution details
- Thread coordination

## 🎯 Design Principles

- **Single Responsibility**: Each component has a focused purpose
- **Lock-Free Programming**: Minimize thread contention for maximum performance
- **Type Safety**: Compile-time verification of message handling
- **Extensibility**: Easy to add new order types and operations

## 🚧 Future Enhancements

- [ ] TCP/WebSocket API for external clients
- [ ] Performance benchmarking suite
- [ ] Persistent order storage with replay capability
- [ ] Advanced order types (Stop, Stop-Limit, Iceberg)
- [ ] Risk management and position limits
- [ ] Market data publication system

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Built with modern C++ for maximum performance and maintainability.**