
# this file autodetected by py/py.mk based on its name
#
#



YAML_MOD_DIR := $(USERMOD_DIR)
YAML_SRC_DIR := $(YAML_MOD_DIR)/libyaml/src
# YAML_SRC_ASM_DIR := $(YAML_MOD_DIR)/libyaml/build

SRC_USERMOD += $(YAML_MOD_DIR)/mod_libyaml.c
CFLAGS_USERMOD += -I $(YAML_MOD_DIR)/libyaml/include
CFLAGS_USERMOD += -I $(YAML_MOD_DIR)/libyaml/src
CFLAGS_USERMOD +=  -D HAVE_CONFIG_H

# CLFAGS_USERMOD += -I$(YAML_SRC_DIR) -I$(YAML_MOD_DIR)/libyaml/bindings -D__YAML_PORTABLE__

# CFLAGS_EXTMOD += -Wno-unused-function
#-Wno-undefined-internal
CFLAGS_EXTRA += -Wno-unused-function -Wno-unused-value
# -Wno-typedef-redefinition
# CFLAGS_EXTRA += -Wno-undefined-internal -Wno-implicit-function-declaration


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



