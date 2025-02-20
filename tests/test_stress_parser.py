import yaml


print("PARSER STRESS:")
for i in range(1000):
    a = yaml.loads("---\nfoo: 1\nbar: bye\nworld: null\nhello: true\n...\n")
    print(i, a)
