//
//  MileageTrip.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import Foundation
import SwiftData
import CoreLocation

/// Represents a coordinate point for GPS route storage
struct RouteCoordinate: Codable, Sendable {
    var latitude: Double
    var longitude: Double
    var timestamp: Date
}

@Model
final class MileageTrip {
    var id: UUID
    var startDate: Date
    var endDate: Date?
    var distanceMiles: Double
    var categoryRaw: String
    var purpose: String?
    var isManualEntry: Bool
    var routeCoordinatesData: Data?

    var category: MileageCategory {
        get { MileageCategory(rawValue: categoryRaw) ?? .personal }
        set { categoryRaw = newValue.rawValue }
    }

    var routeCoordinates: [RouteCoordinate]? {
        get {
            guard let data = routeCoordinatesData else { return nil }
            return try? JSONDecoder().decode([RouteCoordinate].self, from: data)
        }
        set {
            routeCoordinatesData = try? JSONEncoder().encode(newValue)
        }
    }

    init(
        id: UUID = UUID(),
        startDate: Date,
        endDate: Date? = nil,
        distanceMiles: Double,
        category: MileageCategory,
        purpose: String? = nil,
        isManualEntry: Bool,
        routeCoordinates: [RouteCoordinate]? = nil
    ) {
        self.id = id
        self.startDate = startDate
        self.endDate = endDate
        self.distanceMiles = distanceMiles
        self.categoryRaw = category.rawValue
        self.purpose = purpose
        self.isManualEntry = isManualEntry
        self.routeCoordinates = routeCoordinates
    }

    var displayDistance: String {
        String(format: "%.2f mi", distanceMiles)
    }

    var duration: TimeInterval? {
        guard let end = endDate else { return nil }
        return end.timeIntervalSince(startDate)
    }

    var durationFormatted: String? {
        guard let d = duration, d > 0 else { return nil }
        let hours = Int(d) / 3600
        let minutes = (Int(d) % 3600) / 60
        if hours > 0 {
            return "\(hours)h \(minutes)m"
        } else {
            return "\(minutes)m"
        }
    }
}
