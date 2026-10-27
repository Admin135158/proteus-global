FROM alpine:latest
# Install C++ compiler and libcurl
RUN apk add --no-cache g++ curl-dev
WORKDIR /app
# Copy source code
COPY redis_node.cpp .
# Compile it for Linux (Not Android!)
RUN g++ -O2 redis_node.cpp -o redis_node -lcurl -pthread
CMD ["./redis_node"]
