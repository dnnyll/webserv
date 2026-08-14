#!/usr/bin/env python3

import os
import sys

mode_str = os.environ.get("QUERY_STRING", "")

mode = 0o644
if "000" in mode_str:
    mode = 0
elif "644" in mode_str:
    mode = 0o644

target = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "files", "hello.txt")

try:
    os.chmod(target, mode)
    sys.stdout.write("Content-Type: text/plain\r\n\r\n")
    sys.stdout.write("chmod %s %s\n" % (oct(mode), target))
except Exception as e:
    sys.stdout.write("Status: 500 Internal Server Error\r\n")
    sys.stdout.write("Content-Type: text/plain\r\n\r\n")
    sys.stdout.write("chmod failed: %s\n" % (e,))
