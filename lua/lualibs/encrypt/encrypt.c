#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int enc_md5(lua_State *L)
{
    const char *str = luaL_checkstring(L, 1);
    lua_pushstring(L, "xxxxxxxx");
    return 1;
}

static const luaL_Reg encrypt[] = {
    {"md5", enc_md5},
    {NULL, NULL},
};

LUAMOD_API int luaopen_encrypt(lua_State *L)
{
    // 注册
    luaL_newlib(L, encrypt);
    return 1;
}