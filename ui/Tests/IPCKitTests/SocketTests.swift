import XCTest

@testable import IPCKit

final class SocketTests: XCTestCase {
    let testPath = "/tmp/test.sock"
    override func setUp() {

        try? FileManager.default.removeItem(atPath: testPath)

        FileManager.default.createFile(atPath: testPath, contents: nil)
        super.setUp()
    }

    override func tearDown() {
        // Clean up any test sockets
        try? FileManager.default.removeItem(atPath: testPath)
        super.tearDown()
    }

    func testSocketInit() throws {
        let socket = UnixSocket(path: testPath)
        XCTAssertNotNil(socket)
    }

    func testFailedConnection() {
        let socket = UnixSocket(path: "/tmp/nonexistent.socket")

        XCTAssertThrowsError(try socket.connect()) { error in
            XCTAssertEqual(error as? SocketError, .connectionFailed("socket path does not exist"))
        }
    }

}
