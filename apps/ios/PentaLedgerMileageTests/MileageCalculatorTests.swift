//
//  MileageCalculatorTests.swift
//  PentaLedgerMileageTests
//
//  Copyright (C) 2025 Joe Turner
//

import XCTest
import CoreLocation
@testable import PentaLedgerMileage

final class MileageCalculatorTests: XCTestCase {

    func testDistanceBetweenSamePointIsZero() {
        let coord = CLLocationCoordinate2D(latitude: 37.7749, longitude: -122.4194)
        let distance = MileageCalculator.distanceMiles(from: coord, to: coord)
        XCTAssertEqual(distance, 0, accuracy: 0.0001)
    }

    func testDistanceBetweenKnownPoints() {
        // San Francisco to Los Angeles is approximately 347 miles
        let sf = CLLocationCoordinate2D(latitude: 37.7749, longitude: -122.4194)
        let la = CLLocationCoordinate2D(latitude: 34.0522, longitude: -118.2437)
        let distance = MileageCalculator.distanceMiles(from: sf, to: la)
        XCTAssertGreaterThan(distance, 340)
        XCTAssertLessThan(distance, 360)
    }

    func testTotalDistanceEmptyArray() {
        let coords: [CLLocationCoordinate2D] = []
        let total = MileageCalculator.totalDistanceMiles(coordinates: coords)
        XCTAssertEqual(total, 0)
    }

    func testTotalDistanceSinglePoint() {
        let coords = [CLLocationCoordinate2D(latitude: 37.7749, longitude: -122.4194)]
        let total = MileageCalculator.totalDistanceMiles(coordinates: coords)
        XCTAssertEqual(total, 0)
    }

    func testTotalDistanceTwoPoints() {
        let sf = CLLocationCoordinate2D(latitude: 37.7749, longitude: -122.4194)
        let oakland = CLLocationCoordinate2D(latitude: 37.8044, longitude: -122.2712)
        let coords = [sf, oakland]
        let total = MileageCalculator.totalDistanceMiles(coordinates: coords)
        XCTAssertGreaterThan(total, 8)
        XCTAssertLessThan(total, 15)
    }

    func testTotalDistanceFromRouteCoordinates() {
        let route = [
            RouteCoordinate(latitude: 37.7749, longitude: -122.4194, timestamp: Date()),
            RouteCoordinate(latitude: 37.7849, longitude: -122.4094, timestamp: Date())
        ]
        let total = MileageCalculator.totalDistanceMiles(from: route)
        XCTAssertGreaterThan(total, 0)
        XCTAssertLessThan(total, 2)
    }
}
