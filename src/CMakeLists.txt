SET(SOURCES ui_Main.c
    ui_Setup.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c)

add_library(ui ${SOURCES})
