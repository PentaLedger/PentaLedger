---
name: Accounting Application Architecture
overview: Plan for a full-stack accounting application with PostgreSQL database, C++ API server (using Drogon framework for JWT support), SvelteKit web frontend, and Docker containerization for all components.
todos: []
---

# Accounting Application Implementation P

lan

## Architecture Overview

The application will consist of three main components:

1. **PostgreSQL Database** - Stores all accounting data (accounts, transactions, journal entries)
2. **C++ API Server** - RESTful API using Drogon framework (excellent JWT support via jwt-cpp)
3. **SvelteKit Web Application** - Frontend for user interaction

All components will be Dockerized for easy testing and deployment.

## Technology Stack

- **Database**: PostgreSQL 15+
- **API Server**: C++ with Drogon framework + jwt-cpp library
- **Frontend**: SvelteKit (TypeScript)
- **Containerization**: Docker & Docker Compose
- **Build System**: CMake for C++ server

## Project Structure

```javascript
PentaLedger/
├── database/
│   ├── Dockerfile
│   ├── init.sql (schema initialization)
│   └── migrations/ (future database migrations)
├── api/
│   ├── Dockerfile
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── main.cpp
│   │   ├── models/ (data models)
│   │   ├── controllers/ (API endpoints)
│   │   ├── middleware/ (JWT auth middleware)
│   │   ├── database/ (PostgreSQL connection handling)
│   │   └── utils/ (helper functions)
│   └── include/
├── web/
│   ├── Dockerfile
│   ├── package.json
│   ├── svelte.config.js
│   ├── src/
│   │   ├── routes/ (SvelteKit routes)
│   │   ├── lib/ (components, stores, utilities)
│   │   └── app.html
│   └── static/
├── docker-compose.yml
└── README.md
```



## Database Schema

Core tables for double-entry bookkeeping:

- **accounts**:** Chart of accounts (id, name, type, parent_id, code)
- **journal_entries**: Transaction records (id, date, description, created_at)
- **journal_entry_lines**: Debit/credit lines (id, journal_entry_id, account_id, debit, credit, description)
- **users**: User authentication (id, username, email, password_hash)
- **sessions**: JWT token tracking (optional, for token revocation)

## Implementation Components

### 1. Database Layer

- PostgreSQL schema with proper constraints
- Foreign key relationships for referential integrity
- Indexes on frequently queried columns
- Docker container with initialization scripts

### 2. C++ API Server (Drogon)

- RESTful endpoints for:
- Authentication: `/api/auth/login`, `/api/auth/register`, `/api/auth/refresh`
- Accounts: CRUD operations for chart of accounts
- Journal Entries: Create, read, list journal entries
- Reports: Balance sheet, income statement endpoints
- JWT middleware for protected routes
- PostgreSQL connection pool using Drogon's ORM
- Error handling and validation
- CORS configuration for SvelteKit frontend

### 3. SvelteKit Frontend

- Authentication pages (login, register)
- Dashboard
- Chart of accounts management
- Journal entry creation and listing
- Financial reports display
- JWT token management (storage, refresh)
- API client with interceptors

### 4. Docker Configuration

- Separate Dockerfiles for each component
- Docker Compose orchestration
- Environment variable configuration
- Network setup for inter-service communication
- Volume mounts for development

## Key Files to Create

1. `docker-compose.yml` - Orchestrates all services
2. `database/Dockerfile` - PostgreSQL container
3. `database/init.sql` - Database schema
4. `api/Dockerfile` - C++ API server container
5. `api/CMakeLists.txt` - Build configuration
6. `api/src/main.cpp` - Drogon application entry point
7. `api/src/middleware/JwtAuth.cpp` - JWT authentication middleware
8. `web/Dockerfile` - SvelteKit application container
9. `web/package.json` - Frontend dependencies
10. `web/svelte.config.js` - SvelteKit configuration

## Dependencies

**C++ API (Drogon):**

- Drogon framework (HTTP server, ORM)
- jwt-cpp (JWT token handling)
- libpq (PostgreSQL client)
- jsoncpp (JSON parsing)

**SvelteKit:**

- @sveltejs/kit
- @sveltejs/adapter-node (for Docker)
- axios or fetch for API calls
- JWT decode library

## Security Considerations

- JWT tokens with expiration
- Password hashing (bcrypt/argon2)
- HTTPS in production (configured in Docker)
- SQL injection prevention (parameterized queries via Drogon ORM)
- CORS configuration
- Input validation on both frontend and backend

## Development Workflow

1. Start all services: `docker-compose up`
2. Database initializes with schema
3. API server connects to database