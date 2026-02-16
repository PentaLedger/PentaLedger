//
//  ActiveTripView.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import SwiftUI
import SwiftData

struct ActiveTripView: View {
    @Environment(\.modelContext) private var modelContext
    @State private var locationService = LocationService()
    @State private var selectedCategory: MileageCategory = .business
    @State private var purpose = ""
    @State private var isTracking = false
    @State private var showPermissionAlert = false
    @State private var showZeroDistanceAlert = false

    var body: some View {
        NavigationStack {
            VStack(spacing: 24) {
                if locationService.isTracking {
                    VStack(spacing: 16) {
                        Image(systemName: "location.fill")
                            .font(.system(size: 48))
                            .foregroundStyle(.red)
                        Text("Tracking Trip")
                            .font(.title2)
                            .fontWeight(.semibold)
                        Text(String(format: "%.2f miles", locationService.currentDistanceMiles))
                            .font(.system(size: 36, weight: .bold, design: .rounded))
                            .contentTransition(.numericText())
                        Text("Trip continues in background")
                            .font(.caption)
                            .foregroundStyle(.secondary)
                    }
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 40)

                    Button(role: .destructive) {
                        endTrip()
                    } label: {
                        Label("End Trip", systemImage: "stop.fill")
                            .frame(maxWidth: .infinity)
                            .padding()
                    }
                    .buttonStyle(.borderedProminent)
                    .tint(.red)
                    .padding(.horizontal, 24)
                } else {
                    VStack(spacing: 16) {
                        Image(systemName: "car.fill")
                            .font(.system(size: 48))
                            .foregroundStyle(.secondary)
                        Text("Start a trip to track mileage automatically using GPS")
                            .font(.body)
                            .foregroundStyle(.secondary)
                            .multilineTextAlignment(.center)
                            .padding(.horizontal)

                        Form {
                            Section("Trip Details") {
                                Picker("Category", selection: $selectedCategory) {
                                    ForEach(MileageCategory.allCases, id: \.self) { cat in
                                        Text(cat.displayName).tag(cat)
                                    }
                                }
                                .pickerStyle(.segmented)
                                TextField("Purpose (optional)", text: $purpose, axis: .vertical)
                                    .lineLimit(2...4)
                            }
                        }
                        .scrollContentBackground(.hidden)

                        if let error = locationService.errorMessage {
                            Text(error)
                                .font(.caption)
                                .foregroundStyle(.red)
                                .padding(.horizontal)
                        }

                        Button {
                            startTrip()
                        } label: {
                            Label("Start Trip", systemImage: "location.fill")
                                .frame(maxWidth: .infinity)
                                .padding()
                        }
                        .buttonStyle(.borderedProminent)
                        .disabled(!locationService.hasLocationPermission)
                        .padding(.horizontal, 24)
                    }
                }
            }
            .navigationTitle("Track")
            .onAppear {
                isTracking = locationService.isTracking
                if !locationService.hasLocationPermission {
                    locationService.requestPermission()
                }
            }
            .onChange(of: locationService.isTracking) { _, _ in
                isTracking = locationService.isTracking
            }
            .alert("Trip Too Short", isPresented: $showZeroDistanceAlert) {
                Button("OK", role: .cancel) {}
            } message: {
                Text("No distance was recorded. The trip may have been too short or GPS signal was insufficient.")
            }
            .alert("Location Permission Required", isPresented: $showPermissionAlert) {
                Button("Open Settings") {
                    if let url = URL(string: UIApplication.openSettingsURLString) {
                        UIApplication.shared.open(url)
                    }
                }
                Button("Cancel", role: .cancel) {}
            } message: {
                Text("Enable location access to track trips. For background tracking, select \"Always\" in Settings.")
            }
        }
    }

    private func startTrip() {
        guard locationService.hasLocationPermission else {
            showPermissionAlert = true
            return
        }
        locationService.startTracking(category: selectedCategory)
    }

    private func endTrip() {
        let (startDate, endDate, distanceMiles, coordinates) = locationService.stopTracking()

        guard distanceMiles > 0 else {
            showZeroDistanceAlert = true
            return
        }

        let trip = MileageTrip(
            startDate: startDate,
            endDate: endDate,
            distanceMiles: distanceMiles,
            category: selectedCategory,
            purpose: purpose.isEmpty ? nil : purpose,
            isManualEntry: false,
            routeCoordinates: coordinates.isEmpty ? nil : coordinates
        )

        modelContext.insert(trip)
        try? modelContext.save()

        purpose = ""
    }
}

#Preview {
    ActiveTripView()
        .modelContainer(for: MileageTrip.self, inMemory: true)
}
