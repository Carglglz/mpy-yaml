## mpy-yaml 

> [!NOTE]
> This is a proof of concept, so it may support only a subset of yaml specs.

MicroPython YAML parser/emitter `USER_C_MODULE` using [libyaml](https://github.com/yaml/libyaml)


### API

- `yaml.loads("yaml_string")`: parses a YAML string and returns a dictionary (same as `json.loads`)

- `yaml.load(stream)`: parses a stream YAML file and returns a dictionary (same as `json.load`) 

- `yaml.loadf(filename)`: parses a YAML file and returns a dictionary. 

- `yaml.dumps(dict)`: dump a dict object into a YAML string.

- `yaml.dump(dict, stream)`: dump a dict object into a YAML string and writes it to a file stream (same as `json.dump`)

- `yaml.dumpf(dict, filename)`: dump a dict object into a YAML string and saves it in `filename`



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

json_string = yaml.load("public.yaml")
print(json_string)

```

```py
{"title": "Finex 2011", "img_url": "/finex/html/img/", "css_url": "/finex/html/style/", "js_url": "/finex/html/js/", "template_dir": "html/templ/", "default_act": "idx", "pages": [{"act": "idx", "title": "Welcome", "html": "public/welcome.phtml"}, {"act": "reg", "title": "Register", "html": "public/register.phtml"}, {"act": "log", "title": "Log in", "html": "public/login.phtml"}, {"act": "out", "title": "Log out", "html": "public/logout.phtml"}]}
```

### Tests 

Tested ports: 
 - unix (macos) 
 - stm32 (pybV1.1)
 - esp32 (ESP32-WROOM-32)

In `./tests/` run `micropython test_parser.py`

```bash
$ micropython test_parser.py


--- YAML PARSER TEST: base.yaml ---

{'john': {'hates': 'meetings', 'loves': 'dogs', 'writes': 'Coding Challenges'}, 'size': [{'hi': 'hello', 'chao': 'bye'}, 10], 'hello': 'world', 'key': 'value', 'name': 'Coding Challenges', 'enable': True, 'challenge': {'name': 'YAML Parser'}, 'array': [1, 2], 'arrayinline': [1, 2]}

--- YAML: base.yaml [ OK ] ---


--- YAML PARSER TEST: public.yaml ---

{'js_url': '/finex/html/js/', 'pages': [{'html': 'public/welcome.phtml', 'title': 'Welcome', 'act': 'idx'}, {'html': 'public/register.phtml', 'title': 'Register', 'act': 'reg'}, {'html': 'public/login.phtml', 'title': 'Log in', 'act': 'log'}, {'html': 'public/logout.phtml', 'title': 'Log out', 'act': 'out'}], 'img_url': '/finex/html/img/', 'css_url': '/finex/html/style/', 'template_dir': 'html/templ/', 'title': 'Finex 2011', 'default_act': 'idx'}

--- YAML: public.yaml [ OK ] ---


--- YAML PARSER TEST: services.yaml ---

{'aiomqtt_sensor_bme280': {'enabled': False, 'kwargs': {'hostname': '${SERVER}'}}, 'hello': {'enabled': True, 'args': [3, 5], 'kwargs': {'service_logger': True, 'loglevel': 'INFO'}}, 'unittest_core': {'enabled': True, 'kwargs': {'debug': False, 'root': '', 'require': [], 'save_report': True, 'modules': ['/home/${USERNAME}/.micropython/lib', 'mymodule.py']}}, 'network': {'enabled': True, 'kwargs': {'rsyslog': False, 'webrepl_on': False}}, 'ping': {'enabled': False, 'kwargs': {'host': '192.168.1.1'}}, 'mip': {'enabled': True, 'kwargs': {'packages': {}, 'schedule': {'repeat': 60, 'start_in': 30}, 'restart': False, 'autoupdate': True}}, 'watcher': {'enabled': True, 'kwargs': {'wdfeed': 60000, 'save_report': True, 'max_errors': 0}}, 'powermg': {'enabled': False}, 'aiomqtt': {'enabled': True, 'kwargs': {'server': '${SERVER}', 'ssl': True, 'ssl_params': {'cert': 'broker.crt', 'key': 'broker-key.pem', 'ca': 'ca.crt'}, 'hostname': '${SERVER}', 'port': 8883, 'keepalive': 300, 'stats': True, 'topics': ['device/gkesp32/resp', 'homeassistant/sensor/especg/state', 'homeassistant/sensor/gkesp32/state']}}, 'gui_lvgl': {'enabled': True, 'kwargs': {'width': 320, 'banner': True, 'lib': 'display_driver', 'driver': 'get_display', 'app': 'asyncmd', 'height': 480, 'app_config': 'configs/asyncmd/gui_lvgl.config', 'loglevel': 'INFO'}}, 'microdot_ws': {'enabled': False}, 'devop': {'enabled': False}, 'microdot': {'enabled': False}, 'webfile': {'enabled': True, 'kwargs': {'ssl': False, 'cert': 'broker.crt', 'hostname': '${SERVER}', 'key': 'broker-key.pem'}}, 'stats': {'enabled': True, 'kwargs': {'port': 8234}}, 'unittest': {'enabled': False, 'kwargs': {'debug': False, 'save_report': True, 'root': '', 'schedule': {'repeat': 30, 'start_in': 2}, 'modules': ['/home/cgg/.micropython/lib', 'mymodule.py']}}, 'certbot': {'enabled': True, 'kwargs': {'certificate': 'broker.crt', 'schedule': {'repeat': 300, 'start_in': 60}, 'loglevel': 'DEBUG'}}, 'aiomqtt_sensor_ina219': {'enabled': False, 'kwargs': {'hostname': '${SERVER}'}}}

--- YAML: services.yaml [ OK ] ---
```

or run MicroPython test suite e.g. :

```bash
$ ./tests/run-tests.py ../user_modules/mpy-yaml/tests/*.py
platform=darwin arch=x64
pass  ../user_modules/mpy-yaml/tests/test_stress_emitter.py
pass  ../user_modules/mpy-yaml/tests/test_stress_parser.py
pass  ../user_modules/mpy-yaml/tests/test_parser.py
pass  ../user_modules/mpy-yaml/tests/test_loads_dumps.py
4 tests performed (8032 individual testcases)
4 tests passed
```

### Debug

Enable with `#define MICROPY_DEBUG_YAML (1)` in `mpconfigboard.h` or `mpconfigvariant.h`

### Memory Usage

The yaml parser and emitter requires two buffers each. Defaults are:

```make
MICROPY_YAML_INPUT_BUFFER  512
MICROPY_YAML_OUTPUT_BUFFER  2048

```
See `mod_libyaml.h` for more information and change defaults settings in `mpconfigboard.h` or `mpconfigvariant.h` if needed.


#### References 

https://www.wpsoftware.net/andrew/pages/libyaml.html

https://codingchallenges.fyi/challenges/challenge-yaml/
