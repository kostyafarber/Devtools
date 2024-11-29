@frozen
public struct SynthMessage {
    public enum Command: UInt8 {
        case setFrequency = 0
        case setDutyCycle = 1
        case start = 2
        case stop = 3
        case increaseVolume = 4
        case decreaseVolume = 5
    }

    @frozen
    public struct MessageData {
        private var storage: (UInt64, UInt64)  // 16 bytes of storage

        public init() {
            storage = (0, 0)
        }

        public var frequency: Float {
            get { withUnsafeBytes(of: storage) { $0.load(as: Float.self) } }
            set {
                withUnsafeMutableBytes(of: &storage) { $0.storeBytes(of: newValue, as: Float.self) }
            }
        }

        public var volume: Float {
            get { withUnsafeBytes(of: storage) { $0.load(as: Float.self) } }
            set {
                withUnsafeMutableBytes(of: &storage) { $0.storeBytes(of: newValue, as: Float.self) }
            }
        }

        public var dutyCycle: Float {
            get { withUnsafeBytes(of: storage) { $0.load(as: Float.self) } }
            set {
                withUnsafeMutableBytes(of: &storage) { $0.storeBytes(of: newValue, as: Float.self) }
            }
        }
    }

    public let command: Command
    public let data: MessageData

    public init(command: Command, data: MessageData) {
        self.command = command
        self.data = data
    }

}
