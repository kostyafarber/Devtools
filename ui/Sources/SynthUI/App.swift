import SwiftUI

@main
struct SynthUIApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .windowStyle(.titleBar)
        .windowResizability(.contentSize)
    }
}

struct ContentView: View {
    @State private var isPlaying = false  // Track play state

    var body: some View {
        VStack {
            Text("Synth UI")
                .font(.largeTitle)
                .padding()

            Spacer()  // Push content to center

            Button(action: {
                isPlaying.toggle()  // Toggle play state
                print(isPlaying ? "Playing" : "Stopped")
            }) {
                Text(isPlaying ? "Stop" : "Play")
                    .font(.title2)
                    .padding()
                    .frame(width: 120)
                    .background(isPlaying ? .red : .green)
                    .foregroundColor(.white)
                    .cornerRadius(10)
            }

            Spacer()  // Push content to center
        }
        .frame(minWidth: 600, minHeight: 400)  // Set minimum window size
        .padding()
    }
}
