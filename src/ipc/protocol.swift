// like variant, enums
enum AudioServiceError: Error {
    case connectionLost
    case invalidFrequency(Float)
    case serviceNotReady
}

@objc protocol AudioServiceProtocol {
    func setFrequency(_ frequency: Float) async throws  // make functions async and report errors
    func setVolume(_ volume: Float)
    func startAudio()
    func stopAudio()
}
