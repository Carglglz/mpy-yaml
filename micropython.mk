
# this file autodetected by py/py.mk based on its name
#
#



YAML_MOD_DIR := $(USERMOD_DIR)
YAML_SRC_DIR := $(YAML_MOD_DIR)/libyaml/src

SRC_USERMOD += $(YAML_MOD_DIR)/mod_libyaml.c
CFLAGS_USERMOD += -I $(YAML_MOD_DIR)/libyaml/include
CFLAGS_USERMOD += -I $(YAML_MOD_DIR)/libyaml/src
CFLAGS_USERMOD +=  -D HAVE_CONFIG_H

CFLAGS_EXTRA += -Wno-unused-function -Wno-unused-value


SRC_USERMOD_LIB_C += $(addprefix $(YAML_SRC_DIR)/, \
					 dumper.c \
					 emitter.c \
					 loader.c \
					 parser.c \
					 reader.c \
					 scanner.c \
					 writer.c \
					)

SRC_USERMOD_LIB_C += $(addprefix $(YAML_MOD_DIR)/, \
					api.c \
					)



