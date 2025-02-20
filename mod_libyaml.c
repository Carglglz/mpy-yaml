// Include MicroPython API.
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/runtime.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/obj.h"
#include "py/objint.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"
#include "py/gc.h"
#include <py/mpconfig.h>
#include <py/misc.h>
#include "py/objlist.h"
#include "py/objstringio.h"
#include "py/parsenum.h"


#include <stdio.h>
#include <string.h>
#include <errno.h>


#include "libyaml/include/yaml.h"
#include "libyaml/include/config.h"
#include "mod_libyaml.h"


#ifndef MICROPY_DEBUG_YAML
#define MICROPY_DEBUG_YAML (0)
#endif

#if MICROPY_OBJ_IMMEDIATE_OBJS

#define BOOL_VALUE(o) ((o) == mp_const_false ? 0 : 1)

#else

#define BOOL_VALUE(o) (((mp_obj_bool_t *)MP_OBJ_TO_PTR(o))->value)

typedef struct _mp_obj_bool_t {
    mp_obj_base_t base;
    bool value;
} mp_obj_bool_t;
#endif



#define LIST_MIN_ALLOC 4

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

static mp_obj_t list_pop(mp_obj_t list_in) {
    mp_check_self(mp_obj_is_type(list_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(list_in);
    if (self->len == 0) {
        mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("pop from empty list"));
    }
    size_t index = mp_get_index(self->base.type, self->len, MP_OBJ_NEW_SMALL_INT(-1), false);
    mp_obj_t ret = self->items[index];
    self->len -= 1;
    memmove(self->items + index, self->items + index + 1, (self->len - index) * sizeof(mp_obj_t));
    // Clear stale pointer from slot which just got freed to prevent GC issues
    self->items[self->len] = MP_OBJ_NULL;
    if (self->alloc > LIST_MIN_ALLOC && self->alloc > 2 * self->len) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc / 2);
        self->alloc /= 2;
    }
    return ret;
}



static void debug_event(yaml_event_t ev){

    switch(ev.type)
    {

        case YAML_NO_EVENT:

        /** A STREAM-START token. */
        case YAML_STREAM_START_EVENT:

            mp_printf(&mp_plat_print, "STREAM START\n");break; 
        /** A STREAM-END token. */
        case YAML_STREAM_END_EVENT:
            mp_printf(&mp_plat_print, "STREAM END\n");break; 
        /** A DOCUMENT-START token. */
        case YAML_DOCUMENT_START_EVENT:
            mp_printf(&mp_plat_print, "DOCUMENT START\n");break; 
        /** A DOCUMENT-END token. */
        case YAML_DOCUMENT_END_EVENT:
            mp_printf(&mp_plat_print, "DOCUMENT END\n");break; 
        /** A BLOCK-SEQUENCE-START token. */
        case YAML_SEQUENCE_START_EVENT:
            mp_printf(&mp_plat_print, "SEQUENCE START\n");break; 
        /** A BLOCK-MAPPING-START token. */
        case YAML_MAPPING_START_EVENT:
            mp_printf(&mp_plat_print, "MAPPING START\n");break; 
        /** A BLOCK-END token. */
        case YAML_SEQUENCE_END_EVENT:
            mp_printf(&mp_plat_print, "SEQUENCE END\n");break; 
        case YAML_MAPPING_END_EVENT:
            mp_printf(&mp_plat_print, "MAPPING END\n");break; 
        case YAML_ALIAS_EVENT:
            mp_printf(&mp_plat_print, "ALIAS\n");break; 
        /** A SCALAR token. */
        case YAML_SCALAR_EVENT:
            mp_printf(&mp_plat_print, "SCALAR: ");break; }


}


// VERSION

static mp_obj_t version(void) {

    const char *yaml_version = yaml_get_version_string();

    mp_obj_t yv = mp_obj_new_str(yaml_version, strlen(yaml_version)); 

    return yv;
        
}
static MP_DEFINE_CONST_FUN_OBJ_0(version_obj, version);



// PARSER
//
static void parse_value(const mp_print_t *print, unsigned char* value){

    mp_obj_t pval = mp_obj_new_str((const char *)value, strlen((const char *)value));
    const char *bf = "false";
    const char *bt = "true";
    const char *bn = "null";
    mp_obj_t dest[2];
    mp_load_method(pval, MP_QSTR_isdigit, dest);
    mp_obj_t isnum = mp_call_method_n_kw(0, 0, dest);

    if (isnum == mp_const_true) {
        mp_print_str(print, (const char*)value);

    }
    else {
        //DEBUG
        /* mp_printf(&mp_plat_print, "VALUE:%s \n", value); */ 
        if (strcmp((const char *)value, bt) == 0 || strcmp((const char *)value, bf) == 0 || strcmp((const char *)value, bn) == 0){

            mp_print_str(print, (const char*)value);

        } 
        else{
        mp_print_str(print, "\"");
        mp_print_str(print, (const char*)value);
        mp_print_str(print, "\"");

        }
    }
    
}

static mp_obj_t mod_yaml_loads(const mp_obj_t str_in){

    mp_check_self(mp_obj_is_str(str_in));
    const char *yfdata = mp_obj_str_get_str(str_in);

    yaml_parser_t parser;
    yaml_event_t  event;   

    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 8, &print);

    mp_obj_t states = mp_obj_new_list(0, NULL);
    mp_obj_t map_state = mp_obj_new_str((const char *)"map", strlen((const char*)"map"));
    mp_obj_t seq_state = mp_obj_new_str((const char *)"seq", strlen((const char*)"seq"));
    mp_obj_t curr_state = mp_obj_new_str((const char *)"map", strlen((const char*)"map"));

    mp_obj_list_t *lstates;

    bool is_key = true;
    bool is_map_value = false;
    bool is_seq = false;
    bool is_first_key = true;
    bool is_first_value_seq = true;

    const char *stream_start = "";
    const char *stream_end = "";
    const char *key_start = "\"";
    const char *seq_start = "[";
    const char *map_start = "{";
    const char *map_end = "}";
    const char *seq_end = "]";
    const char *key_end = "\": ";
    const char *value_next = ", ";



    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (unsigned char *)yfdata, strlen(yfdata));
    do {

        if (!yaml_parser_parse(&parser, &event)){

            goto parser_error;

        }

        // DEBUG:
        //
        #if MICROPY_DEBUG_YAML
        debug_event(event);
        #endif

        switch(event.type)
        {

        case YAML_STREAM_START_EVENT: 
                mp_print_str(&print, stream_start);
                break;
        case YAML_STREAM_END_EVENT:   

                mp_print_str(&print, stream_end);
                break;


        case YAML_SEQUENCE_START_EVENT: 
                if (is_seq && !is_first_value_seq){

                mp_print_str(&print, value_next);

                }
                is_seq = true;
                is_first_value_seq = true;

                mp_obj_list_append(states, seq_state);
                curr_state = seq_state;

                mp_print_str(&print, seq_start);

                break;

        case YAML_SEQUENCE_END_EVENT: 
                is_seq = false;

                curr_state = list_pop(states);

                lstates = MP_OBJ_TO_PTR(states);
                if (lstates->len > 0) {
                    curr_state = list_pop(states); 
                    mp_obj_list_append(states, curr_state);}

                mp_print_str(&print, seq_end);

                // check previous state if empty sequence 
                if (mp_obj_str_equal(curr_state, map_state)){
                    is_first_key = false;
                    is_key = true;
                }
                else if(mp_obj_str_equal(curr_state, seq_state)){

                    is_first_value_seq = false;

                }

                break;

        case YAML_MAPPING_START_EVENT:  
                if (is_seq && !is_first_value_seq){

                mp_print_str(&print, value_next);

                }

                mp_print_str(&print, map_start);

                is_first_key = true;
                is_key = true;

                
                mp_obj_list_append(states, map_state);
                curr_state = map_state;
                break;

        case YAML_MAPPING_END_EVENT:  

                curr_state = list_pop(states);

                lstates = MP_OBJ_TO_PTR(states);
                if (lstates->len > 0) {
                    curr_state = list_pop(states); 
                    mp_obj_list_append(states, curr_state);}

                mp_print_str(&print, map_end);

                // check previous state if empty map 
                if (mp_obj_str_equal(curr_state, map_state)){
                    is_first_key = false;
                    is_key = true;
                }
                else if(mp_obj_str_equal(curr_state, seq_state)){

                    is_first_value_seq = false;

                }

                break;
        case YAML_SCALAR_EVENT:  

                // CURRENT STATE MAP:
                // DEBUG
                //
                if (mp_obj_str_equal(curr_state, map_state)){
                    if (is_key){
                        if(!is_first_key){

                            mp_print_str(&print, value_next);

                        }
                        is_first_key = false;
                        is_map_value = true;
                        is_key = false;

                        mp_print_str(&print, key_start);

                        mp_print_str(&print, (const char*)event.data.scalar.value);

                        mp_print_str(&print, key_end);
                    }
                    else if(is_map_value){

                        parse_value(&print, event.data.scalar.value);
                        is_map_value = false;
                        is_key = true;
                        
                        }
                    if (is_seq){
                        is_first_value_seq = false;
                    }
                }
                else if(mp_obj_str_equal(curr_state, seq_state)){

                    if (!is_first_value_seq){

                        mp_print_str(&print, value_next);
                    }

                    
                    parse_value(&print, event.data.scalar.value);
                    is_first_value_seq = false;

                    is_key = true;

                }

                // DEBUG:
                #if MICROPY_DEBUG_YAML
                mp_printf(&mp_plat_print, " %s \n", event.data.scalar.value); 
                mp_printf(&mp_plat_print, "STATE: %s \n", mp_obj_str_get_str(curr_state));
                #endif
                break;

        default:
            break;

        }

        if(event.type != YAML_STREAM_END_EVENT)
          yaml_event_delete(&event);


      } while(event.type != YAML_STREAM_END_EVENT);
      yaml_event_delete(&event);
      /* END new code */

      /* Cleanup */
      yaml_parser_delete(&parser);

    /* return mod_json_loads(yout); --> requires non static mod_json_loads */ 
    /* return yout; */

    return mp_obj_new_str_from_utf8_vstr(&vstr);

parser_error:

    /* Display a parser error message. */

    switch (parser.error)
    {
        case YAML_MEMORY_ERROR:
            mp_printf(&mp_plat_print, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                mp_printf(&mp_plat_print, "Reader error: %s: #%X at %ld\n", parser.problem,
                        parser.problem_value, (long)parser.problem_offset);
            }
            else {
                mp_printf(&mp_plat_print, "Reader error: %s at %ld\n", parser.problem,
                        (long)parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                mp_printf(&mp_plat_print, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        (int)parser.context_mark.line+1, (int)parser.context_mark.column+1,
                        parser.problem, (int)parser.problem_mark.line+1,
                        (int)parser.problem_mark.column+1);
            }
            else {
                mp_printf(&mp_plat_print, "Scanner error: %s at line %d, column %d\n",
                        parser.problem, (int)parser.problem_mark.line+1,
                        (int)parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                mp_printf(&mp_plat_print, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        (int)parser.context_mark.line+1, (int)parser.context_mark.column+1,
                        parser.problem, (int)parser.problem_mark.line+1,
                        (int)parser.problem_mark.column+1);
            }
            else {
                mp_printf(&mp_plat_print, "Parser error: %s at line %d, column %d\n",
                        parser.problem, (int)parser.problem_mark.line+1,
                        (int)parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            mp_printf(&mp_plat_print, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);

    return mp_const_none;

}

static MP_DEFINE_CONST_FUN_OBJ_1(mod_yaml_loads_obj, mod_yaml_loads); 


static mp_obj_t mod_yaml_load(const mp_obj_t f_in){

    mp_check_self(mp_obj_is_str(f_in));
    return mod_yaml_loads(read_file(f_in));
}

static MP_DEFINE_CONST_FUN_OBJ_1(mod_yaml_load_obj, mod_yaml_load); 


// EMITTER


// This is a helper function to iterate through a dictionary.  The state of
// the iteration is held in *cur and should be initialised with zero for the
// first call.  Will return NULL when no more elements are available.
static mp_map_elem_t *dict_iter_next(mp_obj_dict_t *dict, size_t *cur) {
    size_t max = dict->map.alloc;
    mp_map_t *map = &dict->map;

    size_t i = *cur;
    for (; i < max; i++) {
        if (mp_map_slot_is_filled(map, i)) {
            *cur = i + 1;
            return &(map->table[i]);
        }
    }

    assert(map->used == 0 || i == max);
    return NULL;
}


static int list_print_yaml(yaml_emitter_t *emitter, yaml_event_t *event, mp_obj_t self_in){


    const char * _str = "str";
    const char * _list = "list";
    const char * _dict = "dict";
    const char * _int = "int";
    const char * _bool = "bool";
    const char * _none = "NoneType";
    mp_obj_list_t *o = MP_OBJ_TO_PTR(self_in);


    yaml_sequence_start_event_initialize(event, NULL, NULL, 1, YAML_BLOCK_SEQUENCE_STYLE);
    if (!yaml_emitter_emit(emitter, event)) {
    
        mp_printf(&mp_plat_print, "ERROR SEQ START: "); 
        return 0;}

    
    for (size_t i = 0; i < o->len; i++) {


        // VALUE
        const char *value_type = mp_obj_get_type_str(o->items[i]);
        // DEBUG
        #if MICROPY_DEBUG_YAML
        mp_printf(&mp_plat_print, "VALUE TYPE: %s \n", value_type); 
        #endif
        if (strcmp(value_type, _list) == 0){

            if (!list_print_yaml(emitter, event, o->items[i])){
                return 0;
            }
        }
        if (strcmp(value_type, _dict) == 0){
            if (!dict_print_yaml(emitter, event, o->items[i])){
                return 0;
            }
        }
        if (strcmp(value_type, _str) == 0){

        #if MICROPY_DEBUG_YAML
        mp_printf(&mp_plat_print, "VALUE : %s \n", mp_obj_str_get_str(o->items[i])); 
        #endif

        yaml_scalar_event_initialize(event, NULL, NULL,
            (yaml_char_t *)mp_obj_str_get_str(o->items[i]), strlen(mp_obj_str_get_str(o->items[i])), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(emitter, event)) {
        
            mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
            return 0;}

        }
        if (strcmp(value_type, _int) == 0){

            char stack_buf[sizeof(mp_int_t) * 4];
            char *buf = stack_buf;
            size_t buf_size = sizeof(stack_buf);
            size_t fmt_size;

            char *int_str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size, o->items[i], 10, NULL, '\0', '\0');

            #if MICROPY_DEBUG_YAML
            mp_printf(&mp_plat_print, "VALUE : %s \n", int_str); 
            #endif
            yaml_scalar_event_initialize(event, NULL, NULL,
                                         (yaml_char_t *)int_str, strlen(int_str), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(emitter, event)) {

                mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
                return 0;}


            if (buf != stack_buf) {
                m_del(char, buf, buf_size);
            }

        }
        if (strcmp(value_type, _bool) == 0){

            bool value = BOOL_VALUE(o->items[i]);
            char *val;
            if (value){
               val = "true"; 

            }
            else{
                val = "false";
            }

            #if MICROPY_DEBUG_YAML
            mp_printf(&mp_plat_print, "VALUE : %s \n", val); 
            #endif
            yaml_scalar_event_initialize(event, NULL, NULL,
                                         (yaml_char_t *)val, strlen(val), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(emitter, event)) {

                mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
                return 0;}

        }
        if (strcmp(value_type, _none) == 0){

            #if MICROPY_DEBUG_YAML
            mp_printf(&mp_plat_print, "VALUE : null \n"); 
            #endif
            yaml_scalar_event_initialize(event, NULL, NULL,
                                         (yaml_char_t *)"null", strlen("null"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(emitter, event)) {

                mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
                return 0;}

        }
    }



    yaml_sequence_end_event_initialize(event);
    if (!yaml_emitter_emit(emitter, event)) {

        mp_printf(&mp_plat_print, "ERROR SEQ END: "); 
        return 0;}

    return 1;


}

static int dict_print_yaml(yaml_emitter_t *emitter, yaml_event_t *event, mp_obj_t self_in){


    const char * _str = "str";
    const char * _list = "list";
    const char * _dict = "dict";
    const char * _int = "int";
    const char * _bool = "bool";
    const char * _none = "NoneType";
    mp_obj_dict_t *self = MP_OBJ_TO_PTR(self_in);

    yaml_mapping_start_event_initialize(event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE);
    if (!yaml_emitter_emit(emitter, event)) {
    
        mp_printf(&mp_plat_print, "ERROR MAP START: "); 
        return 0;}

    size_t cur = 0;
    mp_map_elem_t *next = NULL;
    while ((next = dict_iter_next(self, &cur)) != NULL) {


        //  KEY
        #if MICROPY_DEBUG_YAML
        mp_printf(&mp_plat_print, "KEY: %s \n", mp_obj_str_get_str(next->key)); 
        #endif

        yaml_scalar_event_initialize(event, NULL, NULL,
            (yaml_char_t *)mp_obj_str_get_str(next->key), strlen(mp_obj_str_get_str(next->key)), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(emitter, event)) {
        
            mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
            return 0;}



        // VALUE
        const char *value_type = mp_obj_get_type_str(next->value);

        // DEBUG
        #if MICROPY_DEBUG_YAML
        mp_printf(&mp_plat_print, "VALUE TYPE: %s \n", value_type); 
        #endif
        if (strcmp(value_type, _list) == 0){

            if (!list_print_yaml(emitter, event, next->value)){
                return 0;
            }
        }
        if (strcmp(value_type, _dict) == 0){
            if (!dict_print_yaml(emitter, event, next->value)){
                return 0;
            }
        }
        if (strcmp(value_type, _str) == 0){

        #if MICROPY_DEBUG_YAML
        mp_printf(&mp_plat_print, "VALUE : %s \n", mp_obj_str_get_str(next->value)); 
        #endif

        yaml_scalar_event_initialize(event, NULL, NULL,
            (yaml_char_t *)mp_obj_str_get_str(next->value), strlen(mp_obj_str_get_str(next->value)), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(emitter, event)) {
        
            mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
            return 0;}

        }
        if (strcmp(value_type, _int) == 0){

            char stack_buf[sizeof(mp_int_t) * 4];
            char *buf = stack_buf;
            size_t buf_size = sizeof(stack_buf);
            size_t fmt_size;

            char *int_str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size, next->value, 10, NULL, '\0', '\0');

            #if MICROPY_DEBUG_YAML
            mp_printf(&mp_plat_print, "VALUE : %s \n", int_str); 
            #endif
            yaml_scalar_event_initialize(event, NULL, NULL,
                                         (yaml_char_t *)int_str, strlen(int_str), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(emitter, event)) {

                mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
                return 0;}


            if (buf != stack_buf) {
                m_del(char, buf, buf_size);
            }

        }
        if (strcmp(value_type, _bool) == 0){

            bool value = BOOL_VALUE(next->value);
            char *val;
            if (value){
               val = "true"; 

            }
            else{
                val = "false";
            }

            #if MICROPY_DEBUG_YAML
            mp_printf(&mp_plat_print, "VALUE : %s \n", val); 
            #endif
            yaml_scalar_event_initialize(event, NULL, NULL,
                                         (yaml_char_t *)val, strlen(val), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(emitter, event)) {

                mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
                return 0;}

        }
        if (strcmp(value_type, _none) == 0){
            #if MICROPY_DEBUG_YAML
            mp_printf(&mp_plat_print, "VALUE : null \n"); 
            #endif

            yaml_scalar_event_initialize(event, NULL, NULL,
                                         (yaml_char_t *)"null", strlen("null"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(emitter, event)) {

                mp_printf(&mp_plat_print, "ERROR SCALAR: "); 
                return 0;}

        }
    }



    yaml_mapping_end_event_initialize(event);
    if (!yaml_emitter_emit(emitter, event)) {

        mp_printf(&mp_plat_print, "ERROR MAP END: "); 
        return 0;}

    return 1;


}


/*
 * Write handler.
 */

static int
yaml_vstr_write_handler(void *data, unsigned char *buffer, size_t size)
{

    const mp_print_t *print = (const mp_print_t*)data;
    mp_print_str(print, (const char*)buffer);

    // DEBUG
    #if MICROPY_DEBUG_YAML
    mp_printf(&mp_plat_print, "%s \n", buffer); 
    #endif

    return 1;
}

YAML_DECLARE(void)
yaml_emitter_set_output_print(yaml_emitter_t *emitter, void *data)
{
    assert(emitter);    /* Non-NULL emitter object expected. */
    assert(!emitter->write_handler);    /* You can set the output only once. */
    assert(data);       /* Non-NULL file object expected. */

    emitter->write_handler = yaml_vstr_write_handler;
    emitter->write_handler_data = data;
}

static mp_obj_t mod_yaml_dumps(mp_obj_t obj) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 8, &print);

    yaml_emitter_t emitter;
    yaml_event_t event; 


    memset(&emitter, 0, sizeof(emitter));
    memset(&event, 0, sizeof(event));

    if (!yaml_emitter_initialize(&emitter))
        goto emitter_error;
    yaml_emitter_set_unicode(&emitter, 1);

    yaml_emitter_set_output_print(&emitter, &print);


    yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
    if (!yaml_emitter_emit(&emitter, &event)) goto emitter_error;


    yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 0);
    if (!yaml_emitter_emit(&emitter, &event)) {
    
        mp_printf(&mp_plat_print, "ERROR DOC START: "); 
        goto emitter_error;}

    if (!dict_print_yaml(&emitter, &event, obj)){
        goto emitter_error;
    }
    yaml_document_end_event_initialize(&event, 0);
    if (!yaml_emitter_emit(&emitter, &event)) {
    
        mp_printf(&mp_plat_print, "ERROR DOC END: "); 
        goto emitter_error;}

    yaml_stream_end_event_initialize(&event);
    if (!yaml_emitter_emit(&emitter, &event)) goto emitter_error;


    yaml_emitter_delete(&emitter);

    return mp_obj_new_str_from_utf8_vstr(&vstr);

emitter_error:
    switch (emitter.error)
    {
        case YAML_MEMORY_ERROR:
            mp_printf(&mp_plat_print, "Memory error: Not enough memory for emitting\n");
            break;

        case YAML_WRITER_ERROR:
            mp_printf(&mp_plat_print, "Writer error: %s\n", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            mp_printf(&mp_plat_print, "Emitter error: %s\n", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            mp_printf(&mp_plat_print, "Internal error\n");
            break;
    }


    yaml_emitter_delete(&emitter);
    return mp_const_none;

}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_yaml_dumps_obj, mod_yaml_dumps);



static mp_obj_t mod_yaml_dump(mp_obj_t obj, const mp_obj_t f_in){

    mp_check_self(mp_obj_is_str(f_in));

    return write_file(f_in, mod_yaml_dumps(obj)); 
}

static MP_DEFINE_CONST_FUN_OBJ_2(mod_yaml_dump_obj, mod_yaml_dump); 
// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t mp_module_yaml_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_yaml) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&version_obj) },
    { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&mod_yaml_load_obj) },
    { MP_ROM_QSTR(MP_QSTR_loads), MP_ROM_PTR(&mod_yaml_loads_obj) },
    { MP_ROM_QSTR(MP_QSTR_dump), MP_ROM_PTR(&mod_yaml_dump_obj) },
    { MP_ROM_QSTR(MP_QSTR_dumps), MP_ROM_PTR(&mod_yaml_dumps_obj) },
};
static MP_DEFINE_CONST_DICT(mp_module_yaml_globals, mp_module_yaml_globals_table);

// Define module object.
const mp_obj_module_t mp_module_yaml = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_yaml_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_yaml, mp_module_yaml);
