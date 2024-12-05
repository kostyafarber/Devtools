import Foundation

struct MessageHeader {
    var magic: UInt32 = 0x5359_4E48  // Same as C++ MAGIC constant
    var size: UInt32 = 0

    init(size: UInt32) {
        self.magic = 0x5359_4E48  // "SYNH" in hex
        self.size = size
    }

    func serialize() -> Data {
        var header = self
        return Data(bytes: &header, count: MemoryLayout<MessageHeader>.size)
    }
}

public class MessageFrame {
    private var header: MessageHeader
    private var payload: Data

    public init(message: Synth_SynthMessage) throws {
        let data = try message.serializedData()
        self.header = MessageHeader(size: UInt32(data.count))
        self.payload = data
    }

    public func send(using socket: UnixSocket) throws {
        let headerData = header.serialize()
        try socket.send(headerData)
        try socket.send(payload)
    }
}
