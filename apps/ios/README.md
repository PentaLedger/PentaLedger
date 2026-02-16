# PentaLedger Mileage

iPhone application for tracking vehicle mileage with automatic GPS tracking and manual entry support.

## Features

- **Automatic GPS tracking**: Records trips in the background using device location
- **Manual entry**: Add mileage records with date, distance, category, and purpose
- **Mileage categories**: Personal and Business (aligned with PentaLedger)

## Requirements

- iOS 17.0+
- Xcode 15+
- Swift 5.9+

## Building

1. Open `PentaLedgerMileage.xcodeproj` in Xcode (from the `ios/` directory)
2. Select your target device or simulator
3. Build and run (Cmd+R)

## Testing

Run the unit tests with Cmd+U or Product > Test. The `MileageCalculatorTests` target verifies the Haversine distance calculations.

## Location Permissions

The app requires "Always" location permission for background trip tracking. On first launch, you'll be prompted for "When In Use" access. To enable background tracking, go to Settings > PentaLedger Mileage > Location and select "Always".

## License

GPL-2.0 - See parent project for details.
