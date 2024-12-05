// swift-tools-version: 6.0
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "SynthUI",
    platforms: [
        .macOS(.v13)
    ],
    products: [
        .executable(name: "SynthUI", targets: ["SynthUI"])
    ],
    dependencies: [
        .package(url: "https://github.com/apple/swift-protobuf.git", from: "1.28.0")
    ],
    targets: [
        .target(
            name: "IPCKit",
            dependencies: [.product(name: "SwiftProtobuf", package: "swift-protobuf")]
        ),
        .executableTarget(
            name: "SynthUI",
            dependencies: ["IPCKit"]
        ),
        .testTarget(  // Add this
            name: "IPCKitTests",
            dependencies: ["IPCKit"]
        ),
    ]
)
    