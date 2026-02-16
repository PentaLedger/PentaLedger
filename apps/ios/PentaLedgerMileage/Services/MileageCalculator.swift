//
//  MileageCalculator.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import Foundation
import CoreLocation

/// Calculates distance between coordinates using the Haversine formula
enum MileageCalculator {

    /// Earth radius in miles
    private static let earthRadiusMiles = 3959.0

    /// Calculate distance in miles between two coordinates using Haversine formula
    static func distanceMiles(from: CLLocationCoordinate2D, to: CLLocationCoordinate2D) -> Double {
        let lat1 = from.latitude.radians
        let lat2 = to.latitude.radians
        let deltaLat = (to.latitude - from.latitude).radians
        let deltaLon = (to.longitude - from.longitude).radians

        let a = sin(deltaLat / 2) * sin(deltaLat / 2) +
                cos(lat1) * cos(lat2) * sin(deltaLon / 2) * sin(deltaLon / 2)
        let c = 2 * atan2(sqrt(a), sqrt(1 - a))

        return earthRadiusMiles * c
    }

    /// Calculate total distance in miles for a sequence of coordinates
    static func totalDistanceMiles(coordinates: [CLLocationCoordinate2D]) -> Double {
        guard coordinates.count >= 2 else { return 0 }
        var total: Double = 0
        for i in 1..<coordinates.count {
            total += distanceMiles(from: coordinates[i - 1], to: coordinates[i])
        }
        return total
    }

    /// Calculate total distance from RouteCoordinate array
    static func totalDistanceMiles(from routeCoordinates: [RouteCoordinate]) -> Double {
        let coords = routeCoordinates.map { CLLocationCoordinate2D(latitude: $0.latitude, longitude: $0.longitude) }
        return totalDistanceMiles(coordinates: coords)
    }
}

private extension Double {
    var radians: Double { self * .pi / 180 }
}
