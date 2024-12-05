import Foundation
import SwiftProtobuf

public class Transport {
    private let socket: UnixSocket

    public init(socket: UnixSocket) {
        self.socket = socket
    }

    public func send(_ message: Synth_SynthMessage) throws {
        let frame = try MessageFrame(message: message)
        try frame.send(using: socket)
    }

    public func receive() throws -> Synth_SynthMessage {
        let sizeData = try socket.receive(size: MemoryLayout<UInt32>.size)
        let size = sizeData.withUnsafeBytes { $0.load(as: UInt32.self) }

        let messageData = try socket.receive(size: Int(size))
        return try Synth_SynthMessage(serializedData: messageData)
    }
}
