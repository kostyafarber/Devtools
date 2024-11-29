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
    targets: [
        .target(
            name: "IPCKit"
        ),
        // Targets are the basic building blocks of a package, defining a module or a test suite.
        // Targets can depend on other targets in this package and products from dependencies.
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
