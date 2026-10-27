FROM python:3.11-slim
WORKDIR /app
# Install dependencies
RUN pip install fastapi uvicorn requests pydantic
# Copy the API code
COPY . .
# Cloud Run injects a $PORT variable. We default to 8080.
CMD exec uvicorn main:app --host 0.0.0.0 --port ${PORT:-8080}
