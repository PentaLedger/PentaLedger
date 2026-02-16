//
//  PentaLedgerMileageApp.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import SwiftUI
import SwiftData

@main
struct PentaLedgerMileageApp: App {
    var sharedModelContainer: ModelContainer = {
        let schema = Schema([
            MileageTrip.self,
        ])
        let modelConfiguration = ModelConfiguration(schema: schema, isStoredInMemoryOnly: false)

        do {
            return try ModelContainer(for: schema, configurations: [modelConfiguration])
        } catch {
            fatalError("Could not create ModelContainer: \(error)")
        }
    }()

    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .modelContainer(sharedModelContainer)
    }
}
