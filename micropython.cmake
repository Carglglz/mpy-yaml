
add_library(usermod_yaml INTERFACE)

set(YAML_MOD_DIR ${CMAKE_CURRENT_LIST_DIR})
set(YAML_SRC_DIR "${YAML_MOD_DIR}/libyaml/src")

idf_build_set_property(COMPILE_OPTIONS "-Wno-unused-function" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-unused-value" APPEND)
target_include_directories(usermod_yaml INTERFACE
    ${YAML_MOD_DIR}/libyaml/include
    ${YAML_MOD_DIR}/libyaml/src

)


target_sources(usermod_yaml INTERFACE
    ${YAML_MOD_DIR}/mod_libyaml.c
    ${YAML_MOD_DIR}/api.c
    ${YAML_SRC_DIR}/dumper.c
    ${YAML_SRC_DIR}/emitter.c
    ${YAML_SRC_DIR}/loader.c
    ${YAML_SRC_DIR}/parser.c
    ${YAML_SRC_DIR}/reader.c
    ${YAML_SRC_DIR}/scanner.c
    ${YAML_SRC_DIR}/writer.c
)

target_compile_options(usermod INTERFACE -DHAVE_CONFIG_H)

target_link_libraries(usermod INTERFACE usermod_yaml)
