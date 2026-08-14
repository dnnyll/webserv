#!/usr/bin/env python3
import os
import sys
import json

method = os.environ.get("REQUEST_METHOD", "")

body = ""
if method == "POST":
    try:
        length = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        length = 0
    if length > 0:
        body = sys.stdin.read(length)
    else:
        body = sys.stdin.read()

payload = {
    "env": dict(os.environ),
    "body_bytes_read": len(body),
    "body_preview": body[:500]
}

output = json.dumps(payload, indent=2, sort_keys=True)

sys.stdout.write("Content-Type: application/json\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(output)
sys.stdout.write("\n")
