#!/bin/bash
echo "Starting Proteus Global Stack..."

# Start Redis in the background
redis-server --daemonize yes
echo "[OK] Redis Bus Started"

# Start Python API in the background
cd ~/proteus-global/python-ai
nohup uvicorn main:app --host 127.0.0.1 --port 8000 > api.log 2>&1 &
echo "[OK] Python Brain Started"

# Start C++ Node in the background
cd ~/proteus-global/cpp-engines
nohup ./redis_node > node.log 2>&1 &
echo "[OK] C++ Swarm Node Started"

echo "All systems online. Use 'pkill -f redis-server' etc to stop them."
