import os
import yaml
import json


for yf in [f for f in sorted(os.listdir()) if f.endswith(".yaml")]:
    try:
        print(f"\n--- YAML PARSER TEST: {yf} --- \n")
        ys = yaml.load(yf)
        print(ys)
        print(f"\n--- YAML: {yf} [ OK ] --- \n")
    except Exception:
        ys = None
        print(f"\n-- YAML: {yf} [ FAIL ] --- \n")

    try:
        print(f"\n--- JSON PARSER TEST: {yf} --- \n")
        print(json.loads(ys))
        print(f"\n--- JSON: {yf} [ OK ] --- \n")
    except Exception:
        print(f"\n-- JSON: {yf} [ FAIL ] --- \n")
