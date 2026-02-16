//
//  TripListView.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import SwiftUI
import SwiftData
import MapKit

struct TripListView: View {
    @Environment(\.modelContext) private var modelContext
    @Query(sort: \MileageTrip.startDate, order: .reverse) private var trips: [MileageTrip]

    @State private var selectedCategory: MileageCategory?

    private var filteredTrips: [MileageTrip] {
        guard let cat = selectedCategory else { return trips }
        return trips.filter { $0.category == cat }
    }

    private var totalMiles: Double {
        filteredTrips.reduce(0) { $0 + $1.distanceMiles }
    }

    private var totalByCategory: (personal: Double, business: Double) {
        let personal = trips.filter { $0.category == .personal }.reduce(0) { $0 + $1.distanceMiles }
        let business = trips.filter { $0.category == .business }.reduce(0) { $0 + $1.distanceMiles }
        return (personal, business)
    }

    var body: some View {
        NavigationStack {
            Group {
                if trips.isEmpty {
                    ContentUnavailableView(
                        "No Trips Yet",
                        systemImage: "car",
                        description: Text("Start tracking with GPS or add a manual entry.")
                    )
                } else {
                    List {
                        Section("Summary") {
                            HStack {
                                Label("Total", systemImage: "road.lanes")
                                Spacer()
                                Text(String(format: "%.2f mi", totalMiles))
                                    .foregroundStyle(.secondary)
                            }
                            HStack {
                                Text("Personal")
                                Spacer()
                                Text(String(format: "%.2f mi", totalByCategory.personal))
                                    .foregroundStyle(.secondary)
                            }
                            HStack {
                                Text("Business")
                                Spacer()
                                Text(String(format: "%.2f mi", totalByCategory.business))
                                    .foregroundStyle(.secondary)
                            }
                        }

                        Section {
                            Picker("Filter", selection: $selectedCategory) {
                                Text("All").tag(nil as MileageCategory?)
                                ForEach(MileageCategory.allCases, id: \.self) { cat in
                                    Text(cat.displayName).tag(cat as MileageCategory?)
                                }
                            }
                            .pickerStyle(.segmented)
                        }

                        Section("Trips") {
                            ForEach(filteredTrips) { trip in
                                NavigationLink {
                                    TripDetailView(trip: trip)
                                } label: {
                                    TripRowView(trip: trip)
                                }
                            }
                            .onDelete(perform: deleteTrips)
                        }
                    }
                }
            }
            .navigationTitle("Trips")
        }
    }

    private func deleteTrips(at offsets: IndexSet) {
        for index in offsets {
            modelContext.delete(filteredTrips[index])
        }
        try? modelContext.save()
    }
}

struct TripRowView: View {
    let trip: MileageTrip

    var body: some View {
        HStack {
            VStack(alignment: .leading, spacing: 4) {
                Text(trip.startDate, style: .date)
                    .font(.headline)
                HStack(spacing: 8) {
                    Text(trip.category.displayName)
                        .font(.caption)
                        .foregroundStyle(.secondary)
                    if trip.isManualEntry {
                        Text("Manual")
                            .font(.caption2)
                            .foregroundStyle(.orange)
                    }
                }
            }
            Spacer()
            Text(trip.displayDistance)
                .font(.headline)
        }
        .padding(.vertical, 4)
    }
}

struct TripDetailView: View {
    let trip: MileageTrip

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                VStack(alignment: .leading, spacing: 8) {
                    Text(trip.displayDistance)
                        .font(.largeTitle)
                        .fontWeight(.bold)
                    Text(trip.startDate, style: .date)
                    if let end = trip.endDate, end != trip.startDate {
                        Text("End: \(end, style: .date)")
                    }
                    if let dur = trip.durationFormatted {
                        Text("Duration: \(dur)")
                    }
                    HStack {
                        Text(trip.category.displayName)
                            .font(.subheadline)
                            .foregroundStyle(.secondary)
                        if trip.isManualEntry {
                            Text("Manual entry")
                                .font(.caption)
                                .foregroundStyle(.orange)
                        }
                    }
                }

                if let purpose = trip.purpose, !purpose.isEmpty {
                    VStack(alignment: .leading, spacing: 4) {
                        Text("Purpose")
                            .font(.headline)
                        Text(purpose)
                    }
                }

                if let coords = trip.routeCoordinates, !coords.isEmpty {
                    TripMapView(coordinates: coords)
                        .frame(height: 200)
                        .clipShape(RoundedRectangle(cornerRadius: 12))
                }
            }
            .frame(maxWidth: .infinity, alignment: .leading)
            .padding()
        }
        .navigationTitle("Trip Details")
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct TripMapView: View {
    let coordinates: [RouteCoordinate]

    var body: some View {
        Map {
            MapPolyline(coordinates: coordinates.map { CLLocationCoordinate2D(latitude: $0.latitude, longitude: $0.longitude) })
                .stroke(.blue, lineWidth: 4)
        }
        .mapStyle(.standard)
    }
}

extension MileageTrip: Identifiable {}
