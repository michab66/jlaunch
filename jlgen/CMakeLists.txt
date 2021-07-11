cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(headers
    ResourceMgr.h
    RtBase.h
    RtIcon.h
    RtIconGroup.h
    RtStringTable.h
    mod_icons.hpp
    mod_util.hpp
    winicon.h
)
set(implementation
    ResourceMgr.cpp
    RtBase.cpp
    RtIcon.cpp
    RtIconGroup.cpp
    RtStringTable.cpp
    mod_icons.cpp
    jlgen.cpp
)
set(resources
    jlgen.rc
)
	
add_executable(jlgen
    ${implementation}
    ${headers}
    ${resources}
)

target_link_libraries(jlgen
    smack_cpp
)

#target_include_directories(jlgen INTERFACE .)


#install(TARGETS
#  smack_cpp
#  DESTINATION lib)
#install(FILES
#  ${headers}
#  DESTINATION include/smack_cpp)
