import os
import yaml


for yf in [f for f in sorted(os.listdir()) if f.endswith(".yaml")]:
    try:
        print(f"\n--- YAML PARSER TEST: {yf} --- \n")
        ys = yaml.loadf(yf)
        print(ys)
        print(f"\n--- YAML: {yf} [ OK ] --- \n")
    except Exception:
        ys = None
        print(f"\n-- YAML: {yf} [ FAIL ] --- \n")
