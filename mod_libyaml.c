// Include MicroPython API.
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/runtime.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/obj.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"
#include "py/gc.h"
#include <py/mpconfig.h>
#include <py/misc.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>


#include "libyaml/include/yaml.h"
#include "libyaml/include/config.h"
#include "mod_libyaml.h"


static mp_obj_t read_file(mp_obj_t self_in) {
    // file = open(args[0], "rb")
    mp_obj_t f_args[2] = {
        self_in,
        MP_OBJ_NEW_QSTR(MP_QSTR_rb),
    };
    mp_obj_t file = mp_vfs_open(2, &f_args[0], (mp_map_t *)&mp_const_empty_map);

    // data = file.read()
    mp_obj_t dest[2];
    mp_load_method(file, MP_QSTR_read, dest);
    mp_obj_t data = mp_call_method_n_kw(0, 0, dest);

    // file.close()
    mp_stream_close(file);
    return data;
}


static mp_obj_t write_file(mp_obj_t self_in, mp_obj_t data_in) {
    // file = open(args[0], "rb")
    mp_obj_t f_args[2] = {
        self_in,
        MP_OBJ_NEW_QSTR(MP_QSTR_wb),
    };
    mp_obj_t file = mp_vfs_open(2, &f_args[0], (mp_map_t *)&mp_const_empty_map);

    // data = file.read()
    mp_obj_t dest[3];
    mp_load_method(file, MP_QSTR_write, dest);
    dest[2] = data_in;
    mp_obj_t n_bytes = mp_call_method_n_kw(1, 0, dest);

    // file.close()
    mp_stream_close(file);
    return n_bytes;
}


// VERSION

static mp_obj_t version(void) {

    const char *yaml_version = yaml_get_version_string();

    mp_obj_t yv = mp_obj_new_str(yaml_version, strlen(yaml_version)); 

    return yv;
        
}
static MP_DEFINE_CONST_FUN_OBJ_0(version_obj, version);



// PARSER
static mp_obj_t mod_yaml_load(const mp_obj_t f_in){



    mp_check_self(mp_obj_is_str(f_in));
    const char *yfdata = mp_obj_str_get_str(read_file(f_in));

    yaml_parser_t parser;
    yaml_token_t  token;   
    bool is_key = true;
    bool is_map = true;
    bool is_seq = false;
    const char *buf = "";
    const char *stream_start = "\n";
    const char *stream_end = "}";
    const char *key_start = "\"";
    const char *seq_start = "[ ";
    const char *block_start = "{";
    const char *block_end = "}, ";
    const char *seq_end = "], ";
    const char *key_end = "\": ";
    const char *value_end = "\", ";


    mp_obj_t yout = mp_obj_new_str(buf, strlen(buf)); 
    /* mp_obj_t ybuf; */ 
    // mp_obj_str_binary_op

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (unsigned char *)yfdata, strlen(yfdata));
    do {
        yaml_parser_scan(&parser, &token);
        switch(token.type)
        {
        /* Stream start/end */
        case YAML_STREAM_START_TOKEN: 
                /* mp_printf(&mp_plat_print, "STREAM START\n"); */ 
                /* mp_printf(&mp_plat_print, "{"); */ 
                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(stream_start, strlen(stream_start)));
                break;
        case YAML_STREAM_END_TOKEN:   
                /* mp_printf(&mp_plat_print, "STREAM END\n"); */   
                /* mp_printf(&mp_plat_print, "}\n"); */   
                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(stream_end, strlen(stream_end)));
                break;
        /* Token types (read before actual token) */
        case YAML_KEY_TOKEN:   
                is_key = true;
                /* mp_printf(&mp_plat_print, "(Key token)   "); */ 
                /* mp_printf(&mp_plat_print, "\""); */ 

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(key_start, strlen(key_start)));
                break;
        case YAML_VALUE_TOKEN: 
                is_key = false;

                /* yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(key_start, strlen(key_start))); */
                /* mp_printf(&mp_plat_print, "(Value token) "); */ 
                /* mp_printf(&mp_plat_print, "(Value token) "); */ 
                break;


    /** An ALIAS token. */
        /*case YAML_ALIAS_TOKEN: */
        /*    break; */
    /*/1** An ANCHOR token. *1/ */
        /*case YAML_ANCHOR_TOKEN: */
        /*    break; */
    /*/1** A TAG token. *1/ */
        /*case YAML_TAG_TOKEN: */
        /*    break; */
        /* Flow delimiters --> (INLINE) */ 

        case YAML_FLOW_SEQUENCE_START_TOKEN:

                is_seq = true;
                is_map = false;

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(seq_start, strlen(seq_start)));

                break;
    /** A FLOW-SEQUENCE-END token. */
        case YAML_FLOW_SEQUENCE_END_TOKEN:

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(seq_end, strlen(seq_end)));
                is_seq = false;
                break;
        /** A FLOW-MAPPING-START token. */
        case YAML_FLOW_MAPPING_START_TOKEN:

                is_seq = false;
                is_map = false;

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(block_start, strlen(block_start)));

                break;
        /** A FLOW-MAPPING-END token. */
        case YAML_FLOW_MAPPING_END_TOKEN:

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(block_end, strlen(block_end)));
                is_map = false;
                break;



        case YAML_FLOW_ENTRY_TOKEN:

                /* mp_printf(&mp_plat_print, "Start Block (Entry)\n"); */ 
                break;


        /* Block delimeters --> (INDENTED) */
        case YAML_BLOCK_SEQUENCE_START_TOKEN: 
                /* mp_printf(&mp_plat_print, "Start Block (Sequence)\n"); */ 
                /* mp_printf(&mp_plat_print, "[ "); */ 
                is_seq = true;
                is_map = false;

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(seq_start, strlen(seq_start)));

                break;
        case YAML_BLOCK_ENTRY_TOKEN:          

                /* mp_printf(&mp_plat_print, "Start Block (Entry)\n"); */ 
                /* is_map = true; */
                /* mp_printf(&mp_plat_print, " {"); */    

                /* yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(block_start, strlen(block_start))); */

                break;
        case YAML_BLOCK_END_TOKEN:            
                if (is_map){

                /* mp_printf(&mp_plat_print, "}, "); */              

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(block_end, strlen(block_end)));
                is_map = false;

                }
                else{
                if (is_seq){

                    /* mp_printf(&mp_plat_print, "], "); */              

                    yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(seq_end, strlen(seq_end)));
                    is_seq = false;
                    }

                }

                /* mp_printf(&mp_plat_print, "\tEnd block\n"); */              

                break;
        /* Data */
        case YAML_BLOCK_MAPPING_START_TOKEN:  
                is_map = true;
                /* mp_printf(&mp_plat_print, "\t[Block mapping]\n"); */            
                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(block_start, strlen(block_start)));
                break;
        case YAML_SCALAR_TOKEN:  

/*                 yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str((const char *)token.data.scalar.value, strlen((const char *)token.data.scalar.value))); */

                if (is_key){

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str((const char *)token.data.scalar.value, strlen((const char *)token.data.scalar.value)));

                /* mp_printf(&mp_plat_print, "KEY: %s \n", token.data.scalar.value); */ 
                /* mp_printf(&mp_plat_print, "%s\": ", token.data.scalar.value); */ 


                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(key_end, strlen(key_end)));

                }
                else{


                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(key_start, strlen(key_start)));
                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str((const char *)token.data.scalar.value, strlen((const char *)token.data.scalar.value)));

                /* mp_printf(&mp_plat_print, "\"%s\", ", token.data.scalar.value); */ 

                yout = mp_obj_str_binary_op(MP_BINARY_OP_ADD, yout, mp_obj_new_str(value_end, strlen(value_end)));

                /* mp_printf(&mp_plat_print, "VALUE: %s \n", token.data.scalar.value); */ 
                }
                break;
        /* Others */
        default:
          mp_printf(&mp_plat_print, "Got token of type %d\n", token.type);
        }

        if(token.type != YAML_STREAM_END_TOKEN)
          yaml_token_delete(&token);


      } while(token.type != YAML_STREAM_END_TOKEN);
      yaml_token_delete(&token);
      /* END new code */

      /* Cleanup */
      yaml_parser_delete(&parser);

    /* return mod_json_loads(yout); --> requires non static mod_json_loads */ 
    return yout;

}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_yaml_load_obj, mod_yaml_load); 
// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t mp_module_yaml_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_yaml) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&version_obj) },
    { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&mod_yaml_load_obj) },
};
static MP_DEFINE_CONST_DICT(mp_module_yaml_globals, mp_module_yaml_globals_table);

// Define module object.
const mp_obj_module_t mp_module_yaml = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_yaml_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_yaml, mp_module_yaml);
