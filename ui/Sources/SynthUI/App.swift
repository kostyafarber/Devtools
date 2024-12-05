import IPCKit
import SwiftUI

@main
struct SynthUIApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

struct ContentView: View {
    @State private var isPlaying = false
    @State private var frequency: Double = 440.0
    @State private var volume: Double = 0.7
    @State private var dutyCycle: Double = 0.5
    @State private var transport: Transport?

    var body: some View {
        VStack {
            Text("Square Wave Synth")
                .font(.system(.largeTitle, design: .rounded))
                .padding()

            SynthControls(
                isPlaying: $isPlaying,
                frequency: $frequency,
                volume: $volume,
                dutyCycle: $dutyCycle,
                transport: $transport
            )
            .padding()
            .background(
                RoundedRectangle(cornerRadius: 16)
                    .fill(Color(nsColor: .windowBackgroundColor))
                    .shadow(radius: 8)
            )
        }
        .padding()
        .background(Color(nsColor: .controlBackgroundColor))
        .onAppear {
            setupTransport()
        }
    }

    private func setupTransport() {
        startBackendProcess()

        do {
            let socket = UnixSocket(path: "/tmp/socket.server")
            try socket.connect()
            transport = Transport(socket: socket)
        } catch {
            print("Error setting up transport: \(error)")
        }
    }

    private func startBackendProcess() {
        let process = Process()
        process.executableURL = URL(
            fileURLWithPath: Bundle.main.bundlePath + "/Contents/MacOS/SynthBackend")

        do {
            try process.run()
        } catch {
            print("Failed to start backend process: \(error)")
        }
    }
}

struct SynthControls: View {
    @Binding var isPlaying: Bool
    @Binding var frequency: Double
    @Binding var volume: Double
    @Binding var dutyCycle: Double
    @Binding var transport: Transport?

    var body: some View {
        VStack(spacing: 20) {
            PlayButton(isPlaying: $isPlaying, transport: $transport)
            ParameterSlider(
                value: $frequency, range: 20...2000, title: "Frequency", unit: "Hz",
                transport: $transport)
            ParameterSlider(
                value: $volume, range: 0...1, title: "Volume", unit: "%", transport: $transport)
            ParameterSlider(
                value: $dutyCycle, range: 0...1, title: "Duty Cycle", unit: "%",
                transport: $transport)
        }
        .padding()
    }
}

struct PlayButton: View {
    @Binding var isPlaying: Bool
    @Binding var transport: Transport?

    var body: some View {
        Button(action: {
            isPlaying.toggle()
            sendCommand(isPlaying ? .start : .stop)
        }) {
            Image(systemName: isPlaying ? "stop.circle.fill" : "play.circle.fill")
                .resizable()
                .frame(width: 60, height: 60)
                .foregroundColor(isPlaying ? .red : .green)
        }
        .buttonStyle(.plain)
    }

    private func sendCommand(_ command: Synth_SynthMessage.Command) {
        guard let transport = transport else { return }
        var message = Synth_SynthMessage()
        message.command = command
        do {
            try transport.send(message)
        } catch {
            print("Error sending command: \(error)")
        }
    }
}

struct ParameterSlider: View {
    @Binding var value: Double
    let range: ClosedRange<Double>
    let title: String
    let unit: String
    @Binding var transport: Transport?

    var body: some View {
        VStack(alignment: .leading) {
            HStack {
                Text(title)
                    .font(.headline)
                Spacer()
                Text("\(value, specifier: "%.1f")\(unit)")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }

            Slider(value: $value, in: range)
                .onChange(of: value) { _ in
                    sendParameter()
                }
                .tint(.blue)
        }
    }

    private func sendParameter() {
        guard let transport = transport else { return }
        var message = Synth_SynthMessage()
        switch title {
        case "Frequency":
            message.command = .setFrequency
            message.frequency = Float(value)
        case "Volume":
            message.command = .setVolume  // New command
            message.volume = Float(value)
        case "Duty Cycle":
            message.command = .setDutyCycle
            message.dutyCycle = Float(value)
        default:
            return
        }
        do {
            try transport.send(message)
        } catch {
            print("Error sending parameter: \(error)")
        }
    }
}
