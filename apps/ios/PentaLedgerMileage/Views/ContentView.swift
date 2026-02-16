//
//  ContentView.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import SwiftUI
import SwiftData

struct ContentView: View {
    @State private var selectedTab = 0

    var body: some View {
        TabView(selection: $selectedTab) {
            TripListView()
                .tabItem {
                    Label("Trips", systemImage: "car.fill")
                }
                .tag(0)

            ActiveTripView()
                .tabItem {
                    Label("Track", systemImage: "location.fill")
                }
                .tag(1)

            ManualEntryView()
                .tabItem {
                    Label("Add", systemImage: "plus.circle.fill")
                }
                .tag(2)

            SettingsView()
                .tabItem {
                    Label("Settings", systemImage: "gearshape.fill")
                }
                .tag(3)
        }
    }
}

#Preview {
    ContentView()
        .modelContainer(for: MileageTrip.self, inMemory: true)
}
