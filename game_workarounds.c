#include "game_workarounds.h"
#include "main.h"
#include "ge_constants.h"

#include <string.h>

static const char * get_game_code(void)
{
    static const char *(*system_game_code_getter)() = NULL;
    if (system_game_code_getter == NULL){
        system_game_code_getter = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemForKernel", 0xEF29061C);
    }
    return system_game_code_getter() + 0x44;
}

static int is_mhfu(){
    static int cache = -1;
    if (cache != -1){
        return cache;
    }
    const char *mhfu_codes[] = {
        // there should only be the EU, US and JP code, but just in case
        "ULES01213",
        "ULUS10391",
        "NPHH00259",
        "NPJH50244",
        "ULAS42149",
        "ULJM05500",
        "ULJM08019",
        "ULJM08025",
        "ULKS46177"
    };
    const char *game_id = get_game_code();
    for(int i = 0;i < sizeof(mhfu_codes) / sizeof(mhfu_codes[0]);i++){
        if (strcmp(game_id, mhfu_codes[i]) == 0){
            cache = 1;
            return 1;
        }
    }
    cache = 0;
    return 0;
}

enum GAME_WORKAROUND_ACTION fix_mhfu_color_filter_prim(int vertex_count, int prim){
    if (vertex_count != 4){
        return GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED;
    }
    if (state.vertex_type != 0x80011c /* through, abgr8888, s16 pos */){
        return GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED;
    }
    if (prim != GE_PRIM_TRIANGLE_STRIP){
        return GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED;
    }

    #if 0
    // this check works on the EU version, not sure if it works on other versions
    // one of the issue is that it's really similar to other color boxes, might have to check the vertices if it conflicts with other things
    if (state.base != 0x09000000){
        return GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED;
    }
    #endif

    u8 vertex_size = 0;
    u8 pos_off = 0;
    u8 visit_off = 0;
    int pos_size = 0;
    getVertexInfo(state.vertex_type, &vertex_size, &pos_off, &visit_off, &pos_size);

    static const s16 expectations[] = {
        0, 0,
        480, 0,
        0, 272,
        480, 272
    };

    static const s16 patches[] = {
        0, 0,
        960, 0,
        0, 544,
        960, 544
    };

    int offset = 0;
    u32 vertex_addr = state.vertex_addr;
    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 2;j++){
            u32 pos_addr = vertex_addr + pos_off + pos_size * j;
            if (*(s16 *)pos_addr != expectations[offset] && *(s16 *)pos_addr != patches[offset]){
                return GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED;
            }
            offset++;
        }
        vertex_addr += vertex_size;
    }

    #if 1
    // not sure why it'll only draw up to half the screen in height... perhaps a limit to through mode color vertex triangle strips?
    // in dimmed areas games doesn't look good without the filter, but having half the screen in night mode is even worse
    return GAME_WORKAROUND_ACTION_NOP;
    #else
    offset = 0;
    vertex_addr = state.vertex_addr;
    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 2;j++){
            u32 pos_addr = vertex_addr + pos_off + pos_size * j;
            *(s16 *)pos_addr = patches[offset];
            offset++;
        }
        vertex_addr += vertex_size;
    }
    #endif
    return GAME_WORKAROUND_ACTION_VERTICE_PROCESSED;
}

enum GAME_WORKAROUND_ACTION fix_prim(int vertex_count, int prim){
    if (is_mhfu()){
        #define FIX(func) { \
            int fix_result = func(vertex_count, prim); \
            if (fix_result != GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED) { \
                return fix_result; \
            } \
        }
        FIX(fix_mhfu_color_filter_prim);
        #undef FIX
    }
    return GAME_WORKAROUND_ACTION_VERTICE_NOT_PROCESSED;
}
