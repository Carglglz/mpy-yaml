import os
import yaml
import json

for yf in [f for f in os.listdir() if f.endswith(".yaml")]:
    ys = yaml.load(yf)
    print(ys)

    try:
        print("JSON PARSER TEST:")
        print(json.loads(ys))
        print(f"JSON: {yf} [ OK ]")
    except Exception:
        print(f"JSON: {yf} [ FAIL ]")
