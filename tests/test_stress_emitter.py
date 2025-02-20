import yaml
import gc

t = {"foo": 1, "bar": "bye", "world": None, "hello": True}

print("EMITTER STRESS:")
for i in range(1000):
    print(i, yaml.dumps(t))
    gc.collect()
