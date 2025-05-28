# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\cppgroupwork_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\cppgroupwork_autogen.dir\\ParseCache.txt"
  "cppgroupwork_autogen"
  )
endif()
