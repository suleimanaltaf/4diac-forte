#******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
#
# Contributors:
#   Suleiman Altaf
#     - initial API and implementation and/or initial documentation
# *******************************************************************************/

option(FORTE_MODULE_MODBUS_IO "Support for Modbus as IO fieldbus (TCP)" OFF)

if (NOT FORTE_MODULE_MODBUS_IO)
    return()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbus)

add_library(forte-modbus-io
            ModbusIOController.h
            ModbusIOController.cpp
            ModbusIOHandle.h
            ModbusIOHandle.cpp
            fb/ModbusMaster.h
            fb/ModbusMaster.cpp
            fb/ModbusBusAdapter.h
            fb/ModbusBusAdapter.cpp
            fb/ModbusSlave.h
            fb/ModbusSlave.cpp
)
target_include_directories(forte-modbus-io PUBLIC ${LIBMODBUS_INCLUDE_DIRS})
target_link_libraries(forte-modbus-io PUBLIC forte-core ${LIBMODBUS_LIBRARIES})
target_link_libraries(forte PUBLIC $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,forte-modbus-io,$<LINK_LIBRARY:WHOLE_ARCHIVE,forte-modbus-io>>)
install(TARGETS forte-modbus-io EXPORT forte-export FILE_SET HEADERS)
