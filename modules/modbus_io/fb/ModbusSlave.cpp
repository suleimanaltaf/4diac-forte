/*******************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v20.html
 *
 * Contributors:
 *   Suleiman Altaf - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "ModbusSlave.h"
#include "../ModbusIOController.h"

using namespace forte::literals;

namespace forte::eclipse4diac::io::modbus_io {

  // ============================================================================
  // ModbusSlaveRegisters — Holding Register Slave FB
  // ============================================================================

  DEFINE_FIRMWARE_FB(FORTE_ModbusSlaveRegisters, "eclipse4diac::io::modbus_io::ModbusSlaveRegisters"_STRID)

  namespace {
    const auto cRegDataInputNames = std::array{"QI"_STRID,         "StartAddress"_STRID, "IsReadOnly"_STRID,
                                               "Register_1"_STRID, "Register_2"_STRID,   "Register_3"_STRID,
                                               "Register_4"_STRID, "Register_5"_STRID,   "Register_6"_STRID,
                                               "Register_7"_STRID, "Register_8"_STRID};
    const auto cRegDataOutputNames = std::array{"QO"_STRID, "STATUS"_STRID};
    const auto cRegEventInputNames = std::array{"MAP"_STRID};
    const auto cRegEventOutputNames = std::array{"MAPO"_STRID, "IND"_STRID};
    const auto cRegSocketNameIds = std::array{"BusAdapterIn"_STRID};
    const auto cRegPlugNameIds = std::array{"BusAdapterOut"_STRID};

    const SFBInterfaceSpec cRegFBInterfaceSpec = {
        .mEINames = cRegEventInputNames,
        .mEITypeNames = {},
        .mEONames = cRegEventOutputNames,
        .mEOTypeNames = {},
        .mDINames = cRegDataInputNames,
        .mDONames = cRegDataOutputNames,
        .mDIONames = {},
        .mSocketNames = cRegSocketNameIds,
        .mPlugNames = cRegPlugNameIds,
    };
  } // namespace

  const TForteUInt8 FORTE_ModbusSlaveRegisters::scmSlaveConfigurationIO[] = {};
  const TForteUInt8 FORTE_ModbusSlaveRegisters::scmSlaveConfigurationIONum = 0;

  FORTE_ModbusSlaveRegisters::FORTE_ModbusSlaveRegisters(const forte::StringId paInstanceNameId,
                                                         CFBContainer &paContainer) :
      ::forte::io::IOConfigFBMultiSlave(
          scmSlaveConfigurationIO, scmSlaveConfigurationIONum, 0, paContainer, cRegFBInterfaceSpec, paInstanceNameId),
      conn_MAPO(*this, 0),
      conn_IND(*this, 1),
      conn_QI(nullptr),
      conn_StartAddress(nullptr),
      conn_IsReadOnly(nullptr),
      conn_Register_1(nullptr),
      conn_Register_2(nullptr),
      conn_Register_3(nullptr),
      conn_Register_4(nullptr),
      conn_Register_5(nullptr),
      conn_Register_6(nullptr),
      conn_Register_7(nullptr),
      conn_Register_8(nullptr),
      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS),
      var_BusAdapterOut("BusAdapterOut"_STRID, *this, 0),
      var_BusAdapterIn("BusAdapterIn"_STRID, *this, 0) {
  }

  void FORTE_ModbusSlaveRegisters::setInitialValues() {
    var_QI = 0_BOOL;
    var_StartAddress = 0_UINT;
    var_IsReadOnly = 0_BOOL;
    var_Register_1 = ""_STRING;
    var_Register_2 = ""_STRING;
    var_Register_3 = ""_STRING;
    var_Register_4 = ""_STRING;
    var_Register_5 = ""_STRING;
    var_Register_6 = ""_STRING;
    var_Register_7 = ""_STRING;
    var_Register_8 = ""_STRING;
    var_QO = 0_BOOL;
    var_STATUS = u""_WSTRING;
  }

  void FORTE_ModbusSlaveRegisters::initHandles() {
    auto startAddr = static_cast<CIEC_UINT::TValueType>(var_StartAddress);
    bool isReadOnly = static_cast<bool>(var_IsReadOnly);

    auto functionType = isReadOnly ? ModbusFunctionType::InputRegister : ModbusFunctionType::HoldingRegister;
    auto direction = isReadOnly ? ::forte::io::IOMapper::In : ::forte::io::IOMapper::InOut;

    // Register slots 1-8: DI indices 3..10
    for (int i = 0; i < 8; i++) {
      auto *nameParam = static_cast<CIEC_STRING *>(getDI(static_cast<size_t>(3 + i)));
      if (nameParam->length() == 0) {
        continue; // Skip unconfigured slots
      }
      auto addr = static_cast<uint16_t>(startAddr + i);
      ModbusIOController::HandleDescriptor desc(nameParam->getStorage(), direction, mIndex, functionType, addr,
                                                CIEC_ANY::e_WORD);
      initHandle(desc);
    }
  }

  void FORTE_ModbusSlaveRegisters::readInputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventMAPID: {
        readData(0, var_QI, conn_QI);
        readData(1, var_StartAddress, conn_StartAddress);
        readData(2, var_IsReadOnly, conn_IsReadOnly);
        readData(3, var_Register_1, conn_Register_1);
        readData(4, var_Register_2, conn_Register_2);
        readData(5, var_Register_3, conn_Register_3);
        readData(6, var_Register_4, conn_Register_4);
        readData(7, var_Register_5, conn_Register_5);
        readData(8, var_Register_6, conn_Register_6);
        readData(9, var_Register_7, conn_Register_7);
        readData(10, var_Register_8, conn_Register_8);
        break;
      }
      default: break;
    }
  }

  void FORTE_ModbusSlaveRegisters::writeOutputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventMAPOID: {
        writeData(cRegFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        break;
      }
      case scmEventINDID: {
        writeData(cRegFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        writeData(cRegFBInterfaceSpec.getNumDIs() + 1, var_STATUS, conn_STATUS);
        break;
      }
      default: break;
    }
  }

  CIEC_ANY *FORTE_ModbusSlaveRegisters::getDI(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QI;
      case 1: return &var_StartAddress;
      case 2: return &var_IsReadOnly;
      case 3: return &var_Register_1;
      case 4: return &var_Register_2;
      case 5: return &var_Register_3;
      case 6: return &var_Register_4;
      case 7: return &var_Register_5;
      case 8: return &var_Register_6;
      case 9: return &var_Register_7;
      case 10: return &var_Register_8;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_ModbusSlaveRegisters::getDO(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QO;
      case 1: return &var_STATUS;
    }
    return nullptr;
  }

  forte::IPlugPin *FORTE_ModbusSlaveRegisters::getPlugPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterOut : nullptr;
  }

  forte::ISocketPin *FORTE_ModbusSlaveRegisters::getSocketPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterIn : nullptr;
  }

  CEventConnection *FORTE_ModbusSlaveRegisters::getEOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_MAPO;
      case 1: return &conn_IND;
    }
    return nullptr;
  }

  CDataConnection **FORTE_ModbusSlaveRegisters::getDIConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QI;
      case 1: return &conn_StartAddress;
      case 2: return &conn_IsReadOnly;
      case 3: return &conn_Register_1;
      case 4: return &conn_Register_2;
      case 5: return &conn_Register_3;
      case 6: return &conn_Register_4;
      case 7: return &conn_Register_5;
      case 8: return &conn_Register_6;
      case 9: return &conn_Register_7;
      case 10: return &conn_Register_8;
    }
    return nullptr;
  }

  CDataConnection *FORTE_ModbusSlaveRegisters::getDOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QO;
      case 1: return &conn_STATUS;
    }
    return nullptr;
  }

  // ============================================================================
  // ModbusSlaveCoils — Coil Slave FB
  // ============================================================================

  DEFINE_FIRMWARE_FB(FORTE_ModbusSlaveCoils, "eclipse4diac::io::modbus_io::ModbusSlaveCoils"_STRID)

  namespace {
    const auto cCoilDataInputNames = std::array{"QI"_STRID,     "StartAddress"_STRID, "IsReadOnly"_STRID,
                                                "Coil_1"_STRID, "Coil_2"_STRID,       "Coil_3"_STRID,
                                                "Coil_4"_STRID, "Coil_5"_STRID,       "Coil_6"_STRID,
                                                "Coil_7"_STRID, "Coil_8"_STRID};
    const auto cCoilDataOutputNames = std::array{"QO"_STRID, "STATUS"_STRID};
    const auto cCoilEventInputNames = std::array{"MAP"_STRID};
    const auto cCoilEventOutputNames = std::array{"MAPO"_STRID, "IND"_STRID};
    const auto cCoilSocketNameIds = std::array{"BusAdapterIn"_STRID};
    const auto cCoilPlugNameIds = std::array{"BusAdapterOut"_STRID};

    const SFBInterfaceSpec cCoilFBInterfaceSpec = {
        .mEINames = cCoilEventInputNames,
        .mEITypeNames = {},
        .mEONames = cCoilEventOutputNames,
        .mEOTypeNames = {},
        .mDINames = cCoilDataInputNames,
        .mDONames = cCoilDataOutputNames,
        .mDIONames = {},
        .mSocketNames = cCoilSocketNameIds,
        .mPlugNames = cCoilPlugNameIds,
    };
  } // namespace

  const TForteUInt8 FORTE_ModbusSlaveCoils::scmSlaveConfigurationIO[] = {};
  const TForteUInt8 FORTE_ModbusSlaveCoils::scmSlaveConfigurationIONum = 0;

  FORTE_ModbusSlaveCoils::FORTE_ModbusSlaveCoils(const forte::StringId paInstanceNameId, CFBContainer &paContainer) :
      ::forte::io::IOConfigFBMultiSlave(
          scmSlaveConfigurationIO, scmSlaveConfigurationIONum, 0, paContainer, cCoilFBInterfaceSpec, paInstanceNameId),
      conn_MAPO(*this, 0),
      conn_IND(*this, 1),
      conn_QI(nullptr),
      conn_StartAddress(nullptr),
      conn_IsReadOnly(nullptr),
      conn_Coil_1(nullptr),
      conn_Coil_2(nullptr),
      conn_Coil_3(nullptr),
      conn_Coil_4(nullptr),
      conn_Coil_5(nullptr),
      conn_Coil_6(nullptr),
      conn_Coil_7(nullptr),
      conn_Coil_8(nullptr),
      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS),
      var_BusAdapterOut("BusAdapterOut"_STRID, *this, 0),
      var_BusAdapterIn("BusAdapterIn"_STRID, *this, 0) {
  }

  void FORTE_ModbusSlaveCoils::setInitialValues() {
    var_QI = 0_BOOL;
    var_StartAddress = 0_UINT;
    var_IsReadOnly = 0_BOOL;
    var_Coil_1 = ""_STRING;
    var_Coil_2 = ""_STRING;
    var_Coil_3 = ""_STRING;
    var_Coil_4 = ""_STRING;
    var_Coil_5 = ""_STRING;
    var_Coil_6 = ""_STRING;
    var_Coil_7 = ""_STRING;
    var_Coil_8 = ""_STRING;
    var_QO = 0_BOOL;
    var_STATUS = u""_WSTRING;
  }

  void FORTE_ModbusSlaveCoils::initHandles() {
    auto startAddr = static_cast<CIEC_UINT::TValueType>(var_StartAddress);
    bool isReadOnly = static_cast<bool>(var_IsReadOnly);

    auto functionType = isReadOnly ? ModbusFunctionType::DiscreteInput : ModbusFunctionType::Coil;
    auto direction = isReadOnly ? ::forte::io::IOMapper::In : ::forte::io::IOMapper::InOut;

    // Coil slots 1-8: DI indices 3..10
    for (int i = 0; i < 8; i++) {
      auto *nameParam = static_cast<CIEC_STRING *>(getDI(static_cast<size_t>(3 + i)));
      if (nameParam->length() == 0) {
        continue; // Skip unconfigured slots
      }
      auto addr = static_cast<uint16_t>(startAddr + i);
      ModbusIOController::HandleDescriptor desc(nameParam->getStorage(), direction, mIndex, functionType, addr,
                                                CIEC_ANY::e_BOOL);
      initHandle(desc);
    }
  }

  void FORTE_ModbusSlaveCoils::readInputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventMAPID: {
        readData(0, var_QI, conn_QI);
        readData(1, var_StartAddress, conn_StartAddress);
        readData(2, var_IsReadOnly, conn_IsReadOnly);
        readData(3, var_Coil_1, conn_Coil_1);
        readData(4, var_Coil_2, conn_Coil_2);
        readData(5, var_Coil_3, conn_Coil_3);
        readData(6, var_Coil_4, conn_Coil_4);
        readData(7, var_Coil_5, conn_Coil_5);
        readData(8, var_Coil_6, conn_Coil_6);
        readData(9, var_Coil_7, conn_Coil_7);
        readData(10, var_Coil_8, conn_Coil_8);
        break;
      }
      default: break;
    }
  }

  void FORTE_ModbusSlaveCoils::writeOutputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventMAPOID: {
        writeData(cCoilFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        break;
      }
      case scmEventINDID: {
        writeData(cCoilFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        writeData(cCoilFBInterfaceSpec.getNumDIs() + 1, var_STATUS, conn_STATUS);
        break;
      }
      default: break;
    }
  }

  CIEC_ANY *FORTE_ModbusSlaveCoils::getDI(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QI;
      case 1: return &var_StartAddress;
      case 2: return &var_IsReadOnly;
      case 3: return &var_Coil_1;
      case 4: return &var_Coil_2;
      case 5: return &var_Coil_3;
      case 6: return &var_Coil_4;
      case 7: return &var_Coil_5;
      case 8: return &var_Coil_6;
      case 9: return &var_Coil_7;
      case 10: return &var_Coil_8;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_ModbusSlaveCoils::getDO(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QO;
      case 1: return &var_STATUS;
    }
    return nullptr;
  }

  forte::IPlugPin *FORTE_ModbusSlaveCoils::getPlugPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterOut : nullptr;
  }

  forte::ISocketPin *FORTE_ModbusSlaveCoils::getSocketPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterIn : nullptr;
  }

  CEventConnection *FORTE_ModbusSlaveCoils::getEOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_MAPO;
      case 1: return &conn_IND;
    }
    return nullptr;
  }

  CDataConnection **FORTE_ModbusSlaveCoils::getDIConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QI;
      case 1: return &conn_StartAddress;
      case 2: return &conn_IsReadOnly;
      case 3: return &conn_Coil_1;
      case 4: return &conn_Coil_2;
      case 5: return &conn_Coil_3;
      case 6: return &conn_Coil_4;
      case 7: return &conn_Coil_5;
      case 8: return &conn_Coil_6;
      case 9: return &conn_Coil_7;
      case 10: return &conn_Coil_8;
    }
    return nullptr;
  }

  CDataConnection *FORTE_ModbusSlaveCoils::getDOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QO;
      case 1: return &conn_STATUS;
    }
    return nullptr;
  }

} // namespace forte::eclipse4diac::io::modbus_io
