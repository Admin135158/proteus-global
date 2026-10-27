import os
from fastapi import FastAPI
from pydantic import BaseModel
import requests
import json
import asyncio
from datetime import datetime

app = FastAPI(title="Proteus Global Command Center")

# --- UPSTASH CONFIGURATION ---
UPSTASH_URL = "https://causal-marmoset-240640.upstash.io"
# PASTE YOUR NEW ROTATED TOKEN HERE
UPSTASH_TOKEN = "gQAAAAAAA6wAAAIgcDI1Y2QxYjc2MTJiMGU0OGM1YjlkMGU1MjU3NzU4ZDEyYg	" 

HEADERS = {"Authorization": f"Bearer {UPSTASH_TOKEN}"}

class CommandRequest(BaseModel):
    prompt: str

async def mock_ai_decision(prompt: str):
    await asyncio.sleep(1) 
    return {"golden_text": f"Executing directive based on: {prompt}", "mean_confidence": 87.5}

@app.post("/api/v1/command")
async def issue_command(request: CommandRequest):
    print(f"[BRAIN] Received command: {request.prompt}")
    
    ai_decision = await mock_ai_decision(request.prompt)
    
    command_payload = {
        "action": "EXECUTE",
        "directive": ai_decision["golden_text"],
        "confidence": ai_decision["mean_confidence"],
        "timestamp": datetime.utcnow().isoformat()
    }
    
    # Send to Upstash via REST API
    try:
        res = requests.post(
            f"{UPSTASH_URL}/set/proteus:command", 
            headers=HEADERS, 
            data=json.dumps(command_payload)
        )
        print(f"[BRAIN] Upstash Response: {res.json()}")
    except Exception as e:
        print(f"[BRAIN] Failed to connect to Upstash: {e}")
        
    return {"status": "Command dispatched to global bus", "decision": command_payload}

@app.get("/api/v1/swarm_status")
async def get_status():
    # Read from Upstash REST API
    try:
        res = requests.get(f"{UPSTASH_URL}/get/proteus:heartbeat", headers=HEADERS)
        data = res.json()
        if data.get("result"):
            return json.loads(data["result"])
        return {"status": "No active nodes detected"}
    except Exception as e:
        return {"error": str(e)}
