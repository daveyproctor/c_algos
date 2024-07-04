# Use a lightweight base image
FROM alpine:latest

# Install GCC, GDB, and necessary build tools
RUN apk update &&     apk add --no-cache gcc gdb musl-dev make

# Set the working directory
WORKDIR /workspace

# Set the container to stay alive
CMD ["sleep", "infinity"]
