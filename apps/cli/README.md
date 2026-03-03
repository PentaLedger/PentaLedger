# PentaLedger CLI

Command-line utility for connecting to the PentaLedger Neon PostgreSQL database.

## Prerequisites

- **libpq** (PostgreSQL client library)
  - macOS: `brew install libpq`
  - Ubuntu: `apt install libpq-dev`
  - Fedora: `dnp install libpq-devel`

## Build

From the project root:

```bash
mkdir -p build && cd build
cmake -DPostgreSQL_ROOT=/opt/homebrew/opt/libpq ..   # macOS Homebrew libpq (keg-only)
cmake --build . --target pentaledger_cli
```

On systems where libpq is in the default path, omit the `-DPostgreSQL_ROOT` option.

## Usage

Set the `PENTALEDGER_DATABASE_URL` environment variable to your Neon PostgreSQL connection string:

```bash
export PENTALEDGER_DATABASE_URL="postgresql://user:password@host/neondb?sslmode=require"
./build/apps/cli/pentaledger_cli
```

Get the connection string from the [Neon Console](https://console.neon.tech) or use the Neon MCP `get_connection_string` tool.
