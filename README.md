# PentaLedger

A full-stack accounting application with double-entry bookkeeping capabilities.

## Architecture

- **Database**: PostgreSQL 15+ with double-entry bookkeeping schema
- **API Server**: C++ REST API using Drogon framework with JWT authentication
- **Frontend**: SvelteKit web application
- **Containerization**: Docker & Docker Compose for easy deployment

## Features

- User authentication with JWT tokens
- Chart of accounts management
- Journal entry creation and management
- Financial reports (Balance Sheet, Income Statement)
- Double-entry bookkeeping validation

## Prerequisites

- Docker and Docker Compose
- (For local development) Node.js 20+, CMake, C++17 compiler

## Quick Start

1. Clone the repository:
```bash
git clone <repository-url>
cd PentaLedger
```

2. Start all services with Docker Compose:
```bash
docker-compose up --build
```

3. Access the application:
- Web UI: http://localhost:3000
- API: http://localhost:8080
- Database: localhost:5432

## Development

### Database

The database schema is automatically initialized when the database container starts for the first time. The schema includes:
- Users table for authentication
- Accounts table for chart of accounts
- Journal entries and lines for transactions
- Proper indexes and constraints

### API Server

The C++ API server uses:
- Drogon framework for HTTP handling
- jwt-cpp for JWT token management
- PostgreSQL client library (libpq)
- OpenSSL for password hashing

### Frontend

The SvelteKit frontend provides:
- Authentication pages (login/register)
- Dashboard
- Account management
- Journal entry creation
- Financial reports

## Environment Variables

### API Server
- `DB_HOST`: Database host (default: localhost)
- `DB_PORT`: Database port (default: 5432)
- `DB_NAME`: Database name (default: pentaledger)
- `DB_USER`: Database user (default: pentaledger)
- `DB_PASSWORD`: Database password
- `API_PORT`: API server port (default: 8080)
- `JWT_SECRET`: Secret key for JWT tokens (change in production!)

### Web
- `VITE_API_URL`: API server URL (default: http://localhost:8080)
- `PORT`: Web server port (default: 3000)

## API Endpoints

### Authentication
- `POST /api/auth/register` - Register new user
- `POST /api/auth/login` - Login user
- `POST /api/auth/refresh` - Refresh JWT token

### Accounts
- `GET /api/accounts` - List all accounts
- `POST /api/accounts` - Create account
- `GET /api/accounts/{id}` - Get account by ID
- `PUT /api/accounts/{id}` - Update account
- `DELETE /api/accounts/{id}` - Delete account

### Journal Entries
- `GET /api/journal-entries` - List all journal entries
- `POST /api/journal-entries` - Create journal entry
- `GET /api/journal-entries/{id}` - Get journal entry by ID

### Reports
- `GET /api/reports/balance-sheet` - Get balance sheet
- `GET /api/reports/income-statement` - Get income statement

All endpoints except authentication require JWT token in Authorization header: `Bearer <token>`

## License

GPL-2.0
