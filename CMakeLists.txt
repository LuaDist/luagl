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

ADD_LUA_MOULE(luagl ${LUAGL_SRC})
TARGET_LINK_LIBRARIES(luagl ${OPENGL_LIBRARIES})

ADD_LUA_MOULE(luaglu ${LUAGLU_SRC})
TARGET_LINK_LIBRARIES(luaglu ${OPENGL_LIBRARIES})

# Install docs etc.
INSTALL (FILES src/README DESTINATION ${INSTALL_DATA})
INSTALL(TARGETS luagl luaglu DESTINATION ${INSTALL_CMOD})