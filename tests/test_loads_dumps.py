import yaml
import json


a = yaml.loads("hello: true\nworld: null\nfoo: 1\nbar: bye")
print(a)
b = json.loads(a)
print(b)
c = yaml.dumps(b)
print(c)
d = yaml.loads(c)
print(d)
assert b == json.loads(d)
