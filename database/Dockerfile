FROM postgres:15-alpine

# Copy initialization script
COPY init.sql /docker-entrypoint-initdb.d/

# Set environment variables
ENV POSTGRES_DB=pentaledger
ENV POSTGRES_USER=pentaledger
ENV POSTGRES_PASSWORD=pentaledger_dev

# Expose PostgreSQL port
EXPOSE 5432

