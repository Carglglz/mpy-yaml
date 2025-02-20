#include "py/obj.h"

/* mp_obj_t mod_json_loads(mp_obj_t obj); */

/* MP_DECLARE_CONST_FUN_OBJ_1(mod_json_loads_obj); */

static int dict_print_yaml(yaml_emitter_t *emitter, yaml_event_t *event, mp_obj_t self_in);

static int list_print_yaml(yaml_emitter_t *emitter, yaml_event_t *event, mp_obj_t self_in);


// OVERRIDE BUFFERS SIZE
#undef INPUT_RAW_BUFFER_SIZE
#undef OUTPUT_BUFFER_SIZE

// MODIFY this if needed depending on FILE SIZE and MEMORY AVAILABLE

// PARSER NEEDS TWO INPUT BUFFERS:
// INPUT_BUFFER and RAW_INPUT_BUFFER where INPUT_BUFFER = 3 * RAW_INPUT_BUFFER

#ifdef MICROPY_YAML_INPUT_BUFFER

#define INPUT_RAW_BUFFER_SIZE   MICROPY_YAML_INPUT_BUFFER 
#else
#define INPUT_RAW_BUFFER_SIZE   512 
#endif


// EMITTER NEEDS TWO OUTPUT BUFFERS:
// OUTPUT_BUFFER, RAW_OUTPUT_BUFFER where RAW_OUTPUT_BUFFER = (2 * OUTPUT_BUFFER) + 2

#ifdef MICROPY_YAML_OUTPUT_BUFFER

#define OUTPUT_BUFFER_SIZE      MICROPY_YAML_OUTPUT_BUFFER 
#else
#define OUTPUT_BUFFER_SIZE      2048
#endif
//
