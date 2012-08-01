# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( luagl C )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )
include ( lua )

# We need OpenGL to compile
find_package ( OpenGL REQUIRED )
include_directories ( ${OPENGL_INCLUDE_DIR} include )

# OpenGL module
include_directories ( include )

# Sources
set ( LUAGL_SRC src/luagl.c src/luagl_util.c )
set ( LUAGLU_SRC src/luaglu.c src/luagl_util.c )

install_lua_module ( luagl ${LUAGL_SRC} LINK ${OPENGL_LIBRARIES} )
install_lua_module ( luaglu ${LUAGLU_SRC} LINK ${OPENGL_LIBRARIES} )

# Install other.
install_data ( src/README )
