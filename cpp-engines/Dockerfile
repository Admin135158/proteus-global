# Use a standard Ubuntu image for easy C++ compilation
FROM ubuntu:22.04

# Prevent interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install C++ compiler and curl library
RUN apt-get update && apt-get install -y \
    g++ \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the C++ source code
COPY redis_node.cpp .

# Compile the C++ code
RUN g++ -O2 redis_node.cpp -o redis_node -lcurl -pthread

# Run the compiled binary
CMD ["./redis_node"]
