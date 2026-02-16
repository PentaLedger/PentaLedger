//
//  SettingsView.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import SwiftUI
import CoreLocation

struct SettingsView: View {
    @State private var locationService = LocationService()
    @AppStorage("useMiles") private var useMiles = true

    private var permissionStatusText: String {
        switch locationService.authorizationStatus {
        case .notDetermined:
            return "Not determined"
        case .restricted:
            return "Restricted"
        case .denied:
            return "Denied"
        case .authorizedAlways:
            return "Always (background tracking enabled)"
        case .authorizedWhenInUse:
            return "When In Use (upgrade for background)"
        @unknown default:
            return "Unknown"
        }
    }

    private var permissionIcon: String {
        switch locationService.authorizationStatus {
        case .authorizedAlways:
            return "checkmark.circle.fill"
        case .authorizedWhenInUse:
            return "exclamationmark.circle.fill"
        case .denied, .restricted:
            return "xmark.circle.fill"
        default:
            return "questionmark.circle.fill"
        }
    }

    private var permissionColor: Color {
        switch locationService.authorizationStatus {
        case .authorizedAlways:
            return .green
        case .authorizedWhenInUse:
            return .orange
        case .denied, .restricted:
            return .red
        default:
            return .gray
        }
    }

    var body: some View {
        NavigationStack {
            Form {
                Section("Location Permission") {
                    HStack {
                        Image(systemName: permissionIcon)
                            .foregroundStyle(permissionColor)
                            .font(.title2)
                        VStack(alignment: .leading, spacing: 4) {
                            Text(permissionStatusText)
                                .font(.subheadline)
                            if locationService.needsAlwaysPermission {
                                Text("Enable \"Always\" for background trip tracking")
                                    .font(.caption)
                                    .foregroundStyle(.secondary)
                            }
                        }
                        Spacer()
                    }

                    if locationService.authorizationStatus != .authorizedAlways {
                        Button("Open Settings") {
                            if let url = URL(string: UIApplication.openSettingsURLString) {
                                UIApplication.shared.open(url)
                            }
                        }
                    }
                }

                Section("Units") {
                    Toggle("Use miles", isOn: $useMiles)
                    Text("Display distances in miles. When off, kilometers are used.")
                        .font(.caption)
                        .foregroundStyle(.secondary)
                }

                Section {
                    HStack {
                        Text("Version")
                        Spacer()
                        Text("1.0.0")
                            .foregroundStyle(.secondary)
                    }
                }
            }
            .navigationTitle("Settings")
        }
    }
}

#Preview {
    SettingsView()
}
