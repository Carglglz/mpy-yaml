import yaml


a = yaml.loads("hello: true\nworld: null\nfoo: 1\nbar: bye")
print(a)
b = yaml.dumps(a)
print(b)
c = yaml.loads(b)
print(c)
assert a == c
