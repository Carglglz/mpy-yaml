## mpy-yaml 

> [!NOTE]
> This is a proof of concept 

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


#### References 

https://www.wpsoftware.net/andrew/pages/libyaml.html
https://codingchallenges.fyi/challenges/challenge-yaml/
