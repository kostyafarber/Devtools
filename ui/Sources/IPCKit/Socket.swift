import Foundation
import SwiftProtobuf

public enum SocketError: Error, Equatable {
    case socketCreationFailed
    case connectionFailed(String)
    case sendFailed
    case receiveFailed
}

public class UnixSocket {
    private var socketHandle: Int32 = -1
    private let path: String

    public init(path: String) {
        self.path = path
    }

    deinit {
        if socketHandle >= 0 {
            close(socketHandle)
        }
    }

    private func checkSocketPath() -> Bool {
        return FileManager.default.fileExists(atPath: path)
    }

    private func createSocket() throws {
        let fd = socket(AF_UNIX, SOCK_STREAM, 0)
        if fd < 0 {
            throw SocketError.socketCreationFailed
        }

        self.socketHandle = fd
    }

    private func connectSocket() throws {
        var addr = sockaddr_un()
        addr.sun_family = sa_family_t(AF_UNIX)

        let pathSize = MemoryLayout.size(ofValue: addr.sun_path)
        var sunPath = addr.sun_path

        let _ = path.withCString { cString in
            withUnsafeMutablePointer(to: &sunPath) { ptr in
                strncpy(ptr, cString, pathSize)  // Fixed typo
            }
        }

        addr.sun_path = sunPath

        let result = withUnsafePointer(to: &addr) { ptr in
            ptr.withMemoryRebound(to: sockaddr.self, capacity: 1) { sockaddrPtr in
                Darwin.connect(socketHandle, sockaddrPtr, socklen_t(MemoryLayout<sockaddr_un>.size))
            }
        }

        if result < 0 {
            throw SocketError.connectionFailed(String(cString: strerror(errno)))
        }
    }

    public func connect() throws {
        guard checkSocketPath() else {
            throw SocketError.connectionFailed("socket path does not exist")
        }

        try createSocket()
        try connectSocket()
    }

    public func send(_ data: Data) throws {
        var totalBytesSent = 0
        let size = data.count

        while totalBytesSent < size {
            let result = data.withUnsafeBytes { ptr in
                Darwin.send(
                    socketHandle,
                    ptr.baseAddress?.advanced(by: totalBytesSent),
                    size - totalBytesSent,
                    0
                )
            }

            if result < 0 {
                throw SocketError.sendFailed
            }

            totalBytesSent += result
        }
    }

    public func receive(size: Int) throws -> Data {
        var buffer = [UInt8](repeating: 0, count: size)
        var totalBytesRead = 0

        while totalBytesRead < size {
            let result = buffer.withUnsafeMutableBytes { ptr in
                Darwin.recv(
                    socketHandle,
                    ptr.baseAddress?.advanced(by: totalBytesRead),
                    size - totalBytesRead,
                    0
                )
            }

            if result < 0 {
                throw SocketError.receiveFailed
            }

            if result == 0 {
                throw SocketError.receiveFailed  // Connection closed by peer
            }

            totalBytesRead += result
        }

        return Data(buffer)
    }
}
