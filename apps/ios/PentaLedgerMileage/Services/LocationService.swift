//
//  LocationService.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//

import Foundation
import CoreLocation

/// Accuracy threshold in meters - discard points with worse accuracy
private let maxHorizontalAccuracy: CLLocationAccuracy = 50

/// Minimum speed in m/s to consider moving (filter stationary noise)
private let minMovingSpeedMps: Double = 1.0

@Observable
final class LocationService: NSObject {
    private let locationManager = CLLocationManager()
    private let mileageCalculator = MileageCalculator.self

    var authorizationStatus: CLAuthorizationStatus = .notDetermined
    var isTracking = false
    var tripStartDate: Date?
    var currentDistanceMiles: Double = 0
    var routeCoordinates: [RouteCoordinate] = []
    var lastLocation: CLLocation?
    var errorMessage: String?

    var hasLocationPermission: Bool {
        switch authorizationStatus {
        case .authorizedAlways, .authorizedWhenInUse:
            return true
        default:
            return false
        }
    }

    var needsAlwaysPermission: Bool {
        authorizationStatus == .authorizedWhenInUse
    }

    override init() {
        super.init()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.distanceFilter = 10
        locationManager.allowsBackgroundLocationUpdates = true
        locationManager.pausesLocationUpdatesAutomatically = false
        locationManager.showsBackgroundLocationIndicator = true
        authorizationStatus = locationManager.authorizationStatus
    }

    func requestPermission() {
        locationManager.requestWhenInUseAuthorization()
    }

    func requestAlwaysPermission() {
        locationManager.requestAlwaysAuthorization()
    }

    func startTracking(category: MileageCategory) {
        guard hasLocationPermission else {
            errorMessage = "Location permission required"
            return
        }

        if authorizationStatus == .authorizedWhenInUse {
            requestAlwaysPermission()
        }

        routeCoordinates = []
        currentDistanceMiles = 0
        lastLocation = nil
        errorMessage = nil
        tripStartDate = Date()
        isTracking = true

        locationManager.startUpdatingLocation()
    }

    func stopTracking() -> (startDate: Date, endDate: Date, distanceMiles: Double, coordinates: [RouteCoordinate]) {
        isTracking = false
        locationManager.stopUpdatingLocation()

        let start = tripStartDate ?? Date()
        let end = Date()
        let distance = currentDistanceMiles
        let coords = routeCoordinates

        routeCoordinates = []
        currentDistanceMiles = 0
        lastLocation = nil
        tripStartDate = nil

        return (start, end, distance, coords)
    }

    private func processLocation(_ location: CLLocation) {
        guard isTracking else { return }

        if location.horizontalAccuracy > maxHorizontalAccuracy || location.horizontalAccuracy < 0 {
            return
        }

        if let last = lastLocation {
            let speed = location.speed
            if speed >= 0 && speed < minMovingSpeedMps {
                return
            }

            let from = last.coordinate
            let to = location.coordinate
            let segmentMiles = mileageCalculator.distanceMiles(from: from, to: to)
            currentDistanceMiles += segmentMiles
        }

        lastLocation = location
        routeCoordinates.append(RouteCoordinate(
            latitude: location.coordinate.latitude,
            longitude: location.coordinate.longitude,
            timestamp: location.timestamp
        ))
    }
}

extension LocationService: CLLocationManagerDelegate {
    nonisolated func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last else { return }
        Task { @MainActor in
            processLocation(location)
        }
    }

    nonisolated func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        Task { @MainActor in
            errorMessage = error.localizedDescription
        }
    }

    nonisolated func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        Task { @MainActor in
            authorizationStatus = manager.authorizationStatus
        }
    }
}
