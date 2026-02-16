//
//  MileageCategory.swift
//  PentaLedgerMileage
//
//  Copyright (C) 2025 Joe Turner
//
//  Aligned with PentaLedger E_MilageCategory (E_MILEAGE_PERSONAL=0, E_MILEAGE_BUSINESS=1)
//

import Foundation

enum MileageCategory: String, Codable, CaseIterable {
    case personal = "personal"
    case business = "business"

    var displayName: String {
        switch self {
        case .personal: return "Personal"
        case .business: return "Business"
        }
    }

    /// Raw value matching PentaLedger E_MilageCategory for export compatibility
    var pentaledgerValue: Int {
        switch self {
        case .personal: return 0
        case .business: return 1
        }
    }

    static func fromPentaledger(_ value: Int) -> MileageCategory {
        switch value {
        case 1: return .business
        default: return .personal
        }
    }
}
