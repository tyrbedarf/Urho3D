#
# Copyright (c) 2017 the Atomic project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# When syncing repository with upstream be sure to delete stb_rectpack.h. This will make imgui use main instance of
# stb_rectpack already included in the engine.
#set (TARGET_NAME ImGui)

#add_library (${TARGET_NAME} STATIC imgui.cpp imgui_draw.cpp imgui_demo.cpp)
#target_link_libraries (${TARGET_NAME} STB)
#target_compile_definitions(${TARGET_NAME} PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)
#target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#set (STATIC_LIBRARY_TARGETS ${STATIC_LIBRARY_TARGETS} ${TARGET_NAME} PARENT_SCOPE)

set(TARGET_NAME ImGui)
define_source_files(RECURSE GROUP)
setup_library()
install_header_files (DIRECTORY ./ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/ImGui FILES_MATCHING PATTERN *.h BUILD_TREE_ONLY) 