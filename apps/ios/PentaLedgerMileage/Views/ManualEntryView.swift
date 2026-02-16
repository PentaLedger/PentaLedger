//
//  ManualEntryView.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import SwiftUI
import SwiftData

struct ManualEntryView: View {
    @Environment(\.modelContext) private var modelContext

    @State private var date = Date()
    @State private var startOdometer = ""
    @State private var endOdometer = ""
    @State private var category: MileageCategory = .business
    @State private var purpose = ""
    @State private var showSaved = false
    @State private var errorMessage: String?

    private var distanceMiles: Double? {
        guard let start = Double(startOdometer), let end = Double(endOdometer), end > start else {
            return nil
        }
        return end - start
    }

    private var isValid: Bool {
        distanceMiles != nil && (distanceMiles ?? 0) > 0 && (distanceMiles ?? 0) < 10000
    }

    var body: some View {
        NavigationStack {
            Form {
                Section("Trip Date") {
                    DatePicker("Date", selection: $date, displayedComponents: .date)
                }

                Section("Odometer Reading") {
                    TextField("Start (miles)", text: $startOdometer)
                        .keyboardType(.decimalPad)
                    TextField("End (miles)", text: $endOdometer)
                        .keyboardType(.decimalPad)
                }

                if let dist = distanceMiles, dist > 0 {
                    Text("Distance: \(String(format: "%.2f", dist)) miles")
                        .foregroundStyle(.secondary)
                }

                Section("Category") {
                    Picker("Category", selection: $category) {
                        ForEach(MileageCategory.allCases, id: \.self) { cat in
                            Text(cat.displayName).tag(cat)
                        }
                    }
                    .pickerStyle(.segmented)
                }

                Section("Purpose (optional)") {
                    TextField("Notes or purpose", text: $purpose, axis: .vertical)
                        .lineLimit(3...6)
                }

                if let error = errorMessage {
                    Section {
                        Text(error)
                            .foregroundStyle(.red)
                    }
                }
            }
            .navigationTitle("Manual Entry")
            .onChange(of: startOdometer) { _, _ in errorMessage = nil }
            .onChange(of: endOdometer) { _, _ in errorMessage = nil }
            .toolbar {
                ToolbarItem(placement: .primaryAction) {
                    Button("Save") {
                        saveTrip()
                    }
                    .disabled(!isValid)
                }
            }
            .overlay {
                if showSaved {
                    Text("Saved")
                        .font(.headline)
                        .padding()
                        .background(.green.opacity(0.9))
                        .foregroundStyle(.white)
                        .clipShape(RoundedRectangle(cornerRadius: 8))
                        .transition(.scale.combined(with: .opacity))
                }
            }
            .animation(.easeInOut(duration: 0.2), value: showSaved)
            .animation(.easeInOut(duration: 0.2), value: errorMessage)
        }
    }

    private func saveTrip() {
        guard let dist = distanceMiles, dist > 0, dist < 10000 else {
            errorMessage = "End odometer must be greater than start"
            return
        }

        let trip = MileageTrip(
            startDate: date,
            endDate: date,
            distanceMiles: dist,
            category: category,
            purpose: purpose.isEmpty ? nil : purpose,
            isManualEntry: true
        )

        modelContext.insert(trip)
        do {
            try modelContext.save()

            startOdometer = ""
            endOdometer = ""
            purpose = ""
            errorMessage = nil
            showSaved = true

            DispatchQueue.main.asyncAfter(deadline: .now() + 1.5) {
                showSaved = false
            }
        } catch {
            errorMessage = "Failed to save: \(error.localizedDescription)"
        }
    }
}

#Preview {
    ManualEntryView()
        .modelContainer(for: MileageTrip.self, inMemory: true)
}
