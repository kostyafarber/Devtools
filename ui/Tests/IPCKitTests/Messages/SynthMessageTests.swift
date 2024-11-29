import XCTest

@testable import IPCKit

final class MessageTests: XCTestCase {
    func testMessageLayout() {
        print("Command size:", MemoryLayout<SynthMessage.Command>.size)  // 1
        print("MessageData size:", MemoryLayout<SynthMessage.MessageData>.size)  // 16
        print("Total size:", MemoryLayout<SynthMessage>.size)  // 24
        print("Stride:", MemoryLayout<SynthMessage>.stride)  // 24
        print("Alignment:", MemoryLayout<SynthMessage>.alignment)

        XCTAssertEqual(MemoryLayout<SynthMessage>.size, 17)
    }

}
