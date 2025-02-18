## mpy-yaml 

> [!NOTE]
> This is a proof of concept, so it may support only a subset of yaml specs.

MicroPython YAML parser `USER_C_MODULE` using [libyaml](https://github.com/yaml/libyaml)

`yaml.load` parses a YAML file and returns a JSON string which can be loaded
with `json.loads`.

### Example 

```yaml
# public.yaml
title   : Finex 2011
img_url : /finex/html/img/
css_url : /finex/html/style/
js_url  : /finex/html/js/

template_dir: html/templ/

default_act : idx    # used for invalid/missing act=

pages:
  - act   : idx
    title : Welcome
    html  : public/welcome.phtml
  - act   : reg
    title : Register
    html  : public/register.phtml
  - act   : log
    title : Log in
    html  : public/login.phtml
  - act   : out
    title : Log out
    html  : public/logout.phtml

```


```py 
import yaml 

yaml_string = yaml.load("public.yaml")
print(y)

```

```py
{"title": "Finex 2011", "img_url": "/finex/html/img/", "css_url": "/finex/html/style/", "js_url": "/finex/html/js/", "template_dir": "html/templ/", "default_act": "idx", "pages": [ {"act": "idx", "title": "Welcome", "html": "public/welcome.phtml", }, {"act": "reg", "title": "Register", "html": "public/register.phtml", }, {"act": "log", "title": "Log in", "html": "public/login.phtml", }, {"act": "out", "title": "Log out", "html": "public/logout.phtml", }, ], }

```

### Tests 

Tested ports: 
 - unix (macos) 
 - stm32 (pybV1.1)
 - esp32 (ESP32-WROOM-32)

In `./tests/` run `micropython test_parser.py`

```bash
$ micropython test_parser.py
{"title": "Finex 2011", "img_url": "/finex/html/img/", "css_url": "/finex/html/style/", "js_url": "/finex/html/js/", "template_dir": "html/templ/", "default_act": "idx", "pages": [ {"act": "idx", "title": "Welcome", "html": "public/welcome.phtml", }, {"act": "reg", "title": "Register", "html": "public/register.phtml", }, {"act": "log", "title": "Log in", "html": "public/login.phtml", }, {"act": "out", "title": "Log out", "html": "public/logout.phtml", }, ], }
JSON PARSER TEST:
{'js_url': '/finex/html/js/', 'pages': [{'html': 'public/welcome.phtml', 'title': 'Welcome', 'act': 'idx'}, {'html': 'public/register.phtml', 'title': 'Register', 'act': 'reg'}, {'html': 'public/login.phtml', 'title': 'Log in', 'act': 'log'}, {'html': 'public/logout.phtml', 'title': 'Log out', 'act': 'out'}], 'img_url': '/finex/html/img/', 'css_url': '/finex/html/style/', 'template_dir': 'html/templ/', 'title': 'Finex 2011', 'default_act': 'idx'}
JSON: public.yaml [ OK ]

{"name": "Coding Challenges", "challenge": {"name": "YAML Parser", }, "key": "value", "array": [ 1, 2, ], "arrayinline": [ 1, 2, ], "john": {"writes": "Coding Challenges", "loves": "dogs", "hates": "meetings", }, }
JSON PARSER TEST:
{'key': 'value', 'name': 'Coding Challenges', 'challenge': {'name': 'YAML Parser'}, 'john': {'hates': 'meetings', 'loves': 'dogs', 'writes': 'Coding Challenges'}, 'array': [1, 2], 'arrayinline': [1, 2]}
JSON: base.yaml [ OK ]


```

#### References 

https://www.wpsoftware.net/andrew/pages/libyaml.html

https://codingchallenges.fyi/challenges/challenge-yaml/
