ADD_EXECUTABLE(t_uhdf5 "t_uhdf5.cpp")
TARGET_LINK_LIBRARIES(t_uhdf5 ${HDF5LIBS})

ADD_EXECUTABLE(t_integer_types "t_integer_types.cpp")
TARGET_LINK_LIBRARIES(t_integer_types ${HDF5LIBS})

ADD_EXECUTABLE(t_create_open_close "t_create_open_close.cpp")
TARGET_LINK_LIBRARIES(t_create_open_close ${HDF5LIBS})

INSTALL(TARGETS 
    t_uhdf5
    t_create_open_close 
    t_integer_types    
    DESTINATION ${CMAKE_SOURCE_DIR}/bin)