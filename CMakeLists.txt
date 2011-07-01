# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(luagl C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(dist.cmake)

# We need OpenGL to compile
FIND_PACKAGE (OpenGL REQUIRED)
INCLUDE_DIRECTORIES (${OPENGL_INCLUDE_DIR} include)

# OpenGL module
INCLUDE_DIRECTORIES (include)

# Sources
SET (LUAGL_SRC src/luagl.c src/luagl_util.c)
SET (LUAGLU_SRC src/luaglu.c src/luagl_util.c)

INSTALL_LUA_MODULE(luagl ${LUAGL_SRC} LINK ${OPENGL_LIBRARIES} )
INSTALL_LUA_MODULE(luaglu ${LUAGLU_SRC} LINK ${OPENGL_LIBRARIES} )

# Install docs etc.
INSTALL (FILES src/README DESTINATION ${INSTALL_DATA})
