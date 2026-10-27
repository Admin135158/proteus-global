# Use a lightweight Python base image
FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Install dependencies
RUN pip install --no-cache-dir fastapi uvicorn requests pydantic

# Copy the API code
COPY main.py .

# Cloud Run sets the PORT env variable. We default to 8080.
ENV PORT=8080

# Expose the port
EXPOSE 8080

# Run the API
CMD exec uvicorn main:app --host 0.0.0.0 --port ${PORT}
