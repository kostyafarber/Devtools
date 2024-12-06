# Audio Synthesiser

A modern C++20 audio synthesizer with a SwiftUI frontend, demonstrating various software engineering concepts and best practices.

## Features

- **Cross-Platform Audio Engine**: Built with Core Audio for high-performance sound generation
- **Modern SwiftUI Interface**: Clean and responsive user interface for controlling synthesizer parameters
- **IPC Communication**: Protobuf-based communication between the C++ backend and Swift frontend
- **Comprehensive Testing**: Unit tests and integration tests using Google Test framework
- **CMake Build System**: Modern CMake configuration with proper dependency management
- **Development Tools**:
  - Address Sanitizer integration for memory safety
  - Compile commands export for IDE integration
  - Ninja build system support for faster builds

## Educational Aspects

This project serves as a learning platform for several advanced programming concepts:

1. **Audio Programming**

   - Digital signal processing
   - Real-time audio generation
   - Core Audio API usage

2. **Modern C++ Features**

   - C++20 standard
   - RAII principles
   - Smart pointers and memory management

3. **System Design**

   - IPC (Inter-Process Communication)
   - Protocol Buffers
   - Ring buffer implementation

4. **Testing Practices**

   - Unit testing with Google Test
   - Integration testing
   - Test filtering and organization

5. **Build Systems**
   - Modern CMake practices
   - Swift Package Manager integration
   - macOS bundle creation

## Prerequisites

- CMake 3.26 or higher
- Ninja build system
- C++20 compatible compiler
- Swift 6.0 or higher (for UI)
- macOS 13.0 or higher
- Protocol Buffers compiler

To install these, you can use Homebrew:

```bash
brew install cmake ninja protobuf googletest
```

### Xcode Requirements

- Xcode 15.0 or later (for Swift 6.0 support)
- Xcode Command Line Tools

To install:

```bash
xcode-select --install
```

## Building

1. Clone the repository:

<<<<<<< Updated upstream

```bash
git clone https://github.com/yourusername/audio-synthesizer.git
cd audio-synthesizer
```

2. Build the project:

```
./meta.sh
```

### Build Options

The build system supports several flags:

- `--debug-audio`: Enable audio debugging
- `--no-audio`: Disable audio component build
- `--no-ipc`: Disable IPC component build
- `--no-tests`: Disable test builds

## Running

To run the application:

```bash
./meta.sh run
```

## Testing

Run all tests:

```bash
./meta.sh test
```

Run specific tests:

```bash
./meta.sh test integration # Run integration tests
./meta.sh test audio # Run audio-related tests
```

## Project Structure

- `src/core/` - Audio engine and core functionality
- `src/ipc/` - IPC communication layer
- `ui/` - SwiftUI frontend application
- `tests/` - Unit and integration tests
- `protos/` - Protocol Buffer definitions
