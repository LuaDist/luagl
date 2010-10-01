#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include "luagl_util.h"


#define STATIC_MEMORY_SIZE 1024
static char __luagl_using_static_mem = 0;
static char *static_mem[STATIC_MEMORY_SIZE];

#ifdef __luagl_debug_memleak

static int __memleak_malloc_counter = 0;
static int __memleak_counter = 0;
static int __malloc_counter = 0;
static void *myMalloc(size_t s) { __memleak_malloc_counter++; __memleak_counter++; return malloc(s); }
static void myFree(void *v) { __memleak_counter--; free(v); }

#define malloc myMalloc
#define free myFree

#endif

void *luagl_malloc(size_t s)
{
  if (__luagl_using_static_mem == 0 && s < STATIC_MEMORY_SIZE)
  {
    __luagl_using_static_mem = 0xff;
    return static_mem;
  }

  return malloc(s);
}

void luagl_free(void *v)
{
  if (v != static_mem)
  {
    free(v);
    return;
  }
  __luagl_using_static_mem = 0;
}

/* set field of a lua table with a number */
void luagl_set_field(lua_State *L, unsigned int index, lua_Number value)
{
  lua_pushnumber(L, index);
  lua_pushnumber(L, value);
  lua_settable(L, -3);
}

/* returns an parray with given type and size, this will always call malloc function */
#define LUAGL_NEW_ARRAY_POINTER(type, size) ( (type *)malloc((size) * sizeof(type)) )

/* returns a bi-dimensional parray with given type and size */
#define LUAGL_NEW_ARRAY2_POINTER(type, size1, size2) ( (type *)malloc((size1) * (size2) * sizeof(type)) )

#define LUAGL_INIT_ARRAY(parray, type, size, conversionFunc)           \
{                                                               \
  int i;                                                        \
  for(i = 0; i < (size); i++) {                                 \
  lua_rawgeti(L, index, i+1);                                 \
  (parray)[i] = (type)(conversionFunc)(L, -1);                 \
  lua_remove(L, -1);                                          \
  }                                                             \
}

#define LUAGL_INIT_ARRAY2(parray, type, size1, size2, conversionFunc)  \
{                                                               \
  int i, j;                                                     \
  for(i = 0; i < size1; i++)  {                                 \
  lua_rawgeti(L, index, i+1);                                 \
  if(!lua_istable(L, -1)) return -1;                          \
  for(j = 0; j < *size2; j++) {                               \
  lua_rawgeti(L, -1, j + 1);                                \
  (parray)[i*(*size2) + j] = (type)(conversionFunc)(L, -1);  \
  lua_remove(L, -1);                                        \
  }                                                           \
  lua_remove(L, -1);                                          \
  }                                                             \
}

/* Gets an parray from a lua table, store it in 'parray' and returns the no. of elems of the parray
index refers to where the table is in stack. */
#define DEFINE_GET_ARRAY_FUNC(name, type, conversionFunc) \
  int name(lua_State *L, int index, type **parray)           \
{                                                         \
  int n = luaL_getn(L, index);                           \
  *parray = LUAGL_NEW_ARRAY(type, n);                           \
  LUAGL_INIT_ARRAY(*parray, type, n, conversionFunc);           \
  return n;                                              \
}

/* Same as above, but use use LUAGL_NEW_ARRAY_POINTER instead of LUAGL_NEW_ARRAY */
#define DEFINE_GET_ARRAY_POINTER_FUNC(name, type, conversionFunc) \
  int name(lua_State *L, int index, type **parray)           \
{                                                         \
  int n = luaL_getn(L, index);                           \
  *parray = LUAGL_NEW_ARRAY_POINTER(type, n);                   \
  LUAGL_INIT_ARRAY(*parray, type, n, conversionFunc);           \
  return n;                                              \
}

#define DEFINE_GET_ARRAY2_FUNC(name, type, conversionFunc)   \
  int name(lua_State *L, int index, type **parray, int *size)   \
{                                                            \
  int n = luaL_getn(L, index);                              \
  lua_rawgeti(L, index, 1);                                 \
  if(!lua_istable(L, -1)) { lua_remove(L, -1); return -1; } \
  *size = luaL_getn(L, -1);                                 \
  *parray = LUAGL_NEW_ARRAY2(type, n, *size);                      \
  LUAGL_INIT_ARRAY2(*parray, type, n, size, conversionFunc);       \
  lua_remove(L, -1);                                        \
  return n;                                                 \
}

/* Same as above, but use use LUAGL_NEW_ARRAY2_POINTER instead of LUAGL_NEW_ARRAY2 */
#define DEFINE_GET_ARRAY2_POINTER_FUNC(name, type, conversionFunc)   \
  int name(lua_State *L, int index, type **parray, int *size)   \
{                                                            \
  int n = luaL_getn(L, index);                              \
  lua_rawgeti(L, index, 1);                                 \
  if(!lua_istable(L, -1)) { lua_remove(L, -1); return -1; } \
  *size = luaL_getn(L, -1);                                 \
  *parray = (type *)malloc(*size * n * sizeof(type));        \
  LUAGL_INIT_ARRAY2(*parray, type, n, size, conversionFunc);       \
  lua_remove(L, -1);                                        \
  return n;                                                 \
}

DEFINE_GET_ARRAY_FUNC(luagl_get_arrayb, unsigned char, lua_toboolean)
DEFINE_GET_ARRAY_FUNC(luagl_get_arrayd, double, lua_tonumber)
DEFINE_GET_ARRAY_FUNC(luagl_get_arrayf, float, lua_tonumber)
DEFINE_GET_ARRAY_FUNC(luagl_get_arrayi, int, lua_tointeger)
DEFINE_GET_ARRAY_FUNC(luagl_get_arrayui, unsigned int, lua_tointeger)
DEFINE_GET_ARRAY_FUNC(luagl_get_arrayubyte, unsigned char, lua_tointeger)

DEFINE_GET_ARRAY_POINTER_FUNC(luagl_get_arrayb_pointer, unsigned char, lua_toboolean)
DEFINE_GET_ARRAY_POINTER_FUNC(luagl_get_arrayd_pointer, double, lua_tonumber)

DEFINE_GET_ARRAY2_FUNC(luagl_get_array2ubyte, unsigned char, lua_tointeger)
DEFINE_GET_ARRAY2_FUNC(luagl_get_array2d, double, lua_tonumber)
DEFINE_GET_ARRAY2_FUNC(luagl_get_array2f, float, lua_tonumber)

DEFINE_GET_ARRAY2_POINTER_FUNC(luagl_get_array2d_pointer, double, lua_tonumber)

#undef DEFINE_GET_ARRAY_FUNC
#undef DEFINE_GET_ARRAY_POINTER_FUNC
#undef DEFINE_GET_ARRAY2_FUNC
#undef DEFINE_GET_ARRAY2_POINTER_FUNC

int luagl_str2mask(const char *str)
{
  int i, j;
  int mask = 0;
  int size = (int)strlen(str);
  for(i = 0, j = 0; j < size; i++)
  {
    if(str[i] == '1')
    {
      mask |= (1 << (size-1-j));
      j++;
    }
    else if(str[i] == '0')
      j++;

  }
  return mask;
}

const char *luagl_mask2str(int mask)
{
  unsigned int i;
  static char str[17];
  for(i = 0; i < 16; i++)
  {
    if(mask & (1 << (15 - i)))
      str[i] = '1';
    else
      str[i] = '0';
  }
  str[i] = 0;
  return str;
}

static unsigned int luagl_find_enum(const luaglConst* gl_const, const char *str, int n)
{
  int i = 0;

  while(gl_const[i].str != 0)
  {
    if(strncmp(str, gl_const[i].str, n) == 0 && gl_const[i].str[n] == 0)
      return gl_const[i].value;
    i++;
  }
  return LUAGL_ENUM_ERROR;
}

unsigned int luagl_get_enum(lua_State *L, int index, const luaglConst* gl_const)
{
  if (lua_isnumber(L, index))
  {
    return (unsigned int)lua_tointeger(L, index);
  }
  else
  {
    unsigned int i;
    const char *str = lua_tostring(L, index);
    unsigned int len = (unsigned int)strlen(str);
    unsigned int temp = 0, ret = 0;

    for(i = 0; i < len; i++)
    {
      if(str[i] == ',')
      {
        temp = luagl_find_enum(gl_const, str, i);
        if(temp != LUAGL_ENUM_ERROR)
          ret |= temp;

        str += i+1;
        len -= i+1;
        i = 0;
      }
    }
    temp = luagl_find_enum(gl_const, str, len);

    if(temp == LUAGL_ENUM_ERROR)
    {
      if(ret == 0)
        return LUAGL_ENUM_ERROR;
      return ret;
    }

    return ret | temp;
  }
}

void luagl_initconst(lua_State *L, const luaglConst *gl_const)
{
  for (; gl_const->str; gl_const++) 
  {
    lua_pushstring(L, gl_const->str);
    lua_pushnumber(L, gl_const->value);
    lua_settable(L, -3);
  }
}
