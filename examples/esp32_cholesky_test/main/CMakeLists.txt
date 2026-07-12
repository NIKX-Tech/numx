idf_component_register(
    SRCS
        "main.c"
        "../../../src/linalg.c"
    INCLUDE_DIRS "../../../include"
    PRIV_REQUIRES esp_timer
)
target_compile_options(${COMPONENT_LIB} PRIVATE -O2 -std=c99 -Wno-unused-parameter)
