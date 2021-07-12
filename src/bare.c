#include <lua.h>
#include <lauxlib.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "fzf.h"

static int32_t l_parsepattern(lua_State *L) {
  const char *pat = luaL_checkstring(L, 1);
  char *copy = (char *)malloc(sizeof(char) * strlen(pat) + 1);
  strcpy(copy, pat);
  lua_pushlightuserdata(L, fzf_parse_pattern(0, false, copy, true));
  return 1;
}

static int32_t l_freepattern(lua_State *L) {
  fzf_pattern_t *pat = lua_touserdata(L, 1);
  fzf_free_pattern(pat);
  return 1;
}

static int32_t l_getscore(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  fzf_pattern_t *pat = lua_touserdata(L, 2);
  fzf_slab_t *slab = lua_touserdata(L, 3);
  int32_t res = fzf_get_score(text, pat, slab);
  lua_pushinteger(L, res);
  return 1;
}

static int32_t l_getpos(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  fzf_pattern_t *pat = lua_touserdata(L, 2);
  fzf_slab_t *slab = lua_touserdata(L, 3);
  fzf_position_t *pos = fzf_get_positions(text, pat, slab);
  lua_newtable(L);
  for (size_t i = 0; i < pos->size; ++i) {
    lua_pushnumber(L, pos->data[i]);
    lua_rawseti(L, -2, i + 1);
  }
  fzf_free_positions(pos);

  return 1;
}

static int32_t l_allocateslab(lua_State *L) {
  lua_pushlightuserdata(L, fzf_make_default_slab());
  return 1;
}

static int32_t l_freeslab(lua_State *L) {
  fzf_free_slab(lua_touserdata(L, 1));
  return 1;
}

static const struct luaL_Reg fzf_funtions[] = {
    {"allocate_slab", l_allocateslab},
    {"free_slab", l_freeslab},
    {"parse_pattern", l_parsepattern},
    {"free_pattern", l_freepattern},
    {"get_score", l_getscore},
    {"get_pos", l_getpos},
    {NULL, NULL}};

int luaopen_libfzf(lua_State *L) {
  luaL_openlib(L, "fzflib", fzf_funtions, 0);
  return 1;
}
