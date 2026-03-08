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

#include "ModbusMaster.h"
#include "../ModbusIOController.h"

using namespace forte::literals;
using namespace forte::io;

namespace forte::eclipse4diac::io::modbus_io {

  DEFINE_FIRMWARE_FB(FORTE_ModbusMaster, "eclipse4diac::io::modbus_io::ModbusMaster"_STRID)

  namespace {
    const auto cDataInputNames = std::array{
        "QI"_STRID, "Host"_STRID, "Port"_STRID, "SlaveId"_STRID, "PollIntervalMs"_STRID, "ResponseTimeoutMs"_STRID};
    const auto cDataOutputNames = std::array{"QO"_STRID, "STATUS"_STRID};
    const auto cEventInputNames = std::array{"INIT"_STRID};
    const auto cEventInputTypeIds = std::array{"EInit"_STRID};
    const auto cEventOutputNames = std::array{"INITO"_STRID, "IND"_STRID};
    const auto cEventOutputTypeIds = std::array{"EInit"_STRID, "Event"_STRID};
    const auto cPlugNameIds = std::array{"BusAdapterOut"_STRID};
    const SFBInterfaceSpec cFBInterfaceSpec = {
        .mEINames = cEventInputNames,
        .mEITypeNames = cEventInputTypeIds,
        .mEONames = cEventOutputNames,
        .mEOTypeNames = cEventOutputTypeIds,
        .mDINames = cDataInputNames,
        .mDONames = cDataOutputNames,
        .mDIONames = {},
        .mSocketNames = {},
        .mPlugNames = cPlugNameIds,
    };
  } // namespace

  FORTE_ModbusMaster::FORTE_ModbusMaster(const forte::StringId paInstanceNameId, CFBContainer &paContainer) :
      ::forte::io::IOConfigFBMultiMaster(paContainer, cFBInterfaceSpec, paInstanceNameId),
      var_Port(502_UINT),
      var_SlaveId(1_UINT),
      var_PollIntervalMs(100_UINT),
      var_ResponseTimeoutMs(1000_UINT),
      conn_INITO(*this, 0),
      conn_IND(*this, 1),
      conn_QI(nullptr),
      conn_Host(nullptr),
      conn_Port(nullptr),
      conn_SlaveId(nullptr),
      conn_PollIntervalMs(nullptr),
      conn_ResponseTimeoutMs(nullptr),
      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS),
      var_BusAdapterOut("BusAdapterOut"_STRID, *this, 0) {
  }

  void FORTE_ModbusMaster::setInitialValues() {
    var_QI = 0_BOOL;
    var_Host = ""_STRING;
    var_Port = 502_UINT;
    var_SlaveId = 1_UINT;
    var_PollIntervalMs = 100_UINT;
    var_ResponseTimeoutMs = 1000_UINT;
    var_QO = 0_BOOL;
    var_STATUS = u""_WSTRING;
  }

  void FORTE_ModbusMaster::setConfig() {
    ModbusIOController::Config config;
    config.mHost = var_Host.getStorage();
    config.mPort = static_cast<CIEC_UINT::TValueType>(var_Port);
    config.mSlaveId = static_cast<uint8_t>(static_cast<CIEC_UINT::TValueType>(var_SlaveId));
    config.mPollIntervalMs = static_cast<CIEC_UINT::TValueType>(var_PollIntervalMs);
    config.mResponseTimeoutMs = static_cast<CIEC_UINT::TValueType>(var_ResponseTimeoutMs);
    getDeviceController()->setConfig(&config);
  }

  forte::io::IODeviceController *FORTE_ModbusMaster::createDeviceController(CDeviceExecution &paDeviceExecution) {
    return new ModbusIOController(paDeviceExecution);
  }

  void FORTE_ModbusMaster::readInputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventINITID: {
        readData(0, var_QI, conn_QI);
        readData(1, var_Host, conn_Host);
        readData(2, var_Port, conn_Port);
        readData(3, var_SlaveId, conn_SlaveId);
        readData(4, var_PollIntervalMs, conn_PollIntervalMs);
        readData(5, var_ResponseTimeoutMs, conn_ResponseTimeoutMs);
        break;
      }
      default: break;
    }
  }

  void FORTE_ModbusMaster::writeOutputData(const TEventID paEIID) {
    switch (paEIID) {
      case scmEventINITOID: {
        writeData(cFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        writeData(cFBInterfaceSpec.getNumDIs() + 1, var_STATUS, conn_STATUS);
        break;
      }
      case scmEventINDID: {
        writeData(cFBInterfaceSpec.getNumDIs() + 0, var_QO, conn_QO);
        writeData(cFBInterfaceSpec.getNumDIs() + 1, var_STATUS, conn_STATUS);
        break;
      }
      default: break;
    }
  }

  CIEC_ANY *FORTE_ModbusMaster::getDI(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QI;
      case 1: return &var_Host;
      case 2: return &var_Port;
      case 3: return &var_SlaveId;
      case 4: return &var_PollIntervalMs;
      case 5: return &var_ResponseTimeoutMs;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_ModbusMaster::getDO(const size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_QO;
      case 1: return &var_STATUS;
    }
    return nullptr;
  }

  forte::IPlugPin *FORTE_ModbusMaster::getPlugPinUnchecked(size_t paIndex) {
    return (paIndex == 0) ? &var_BusAdapterOut : nullptr;
  }

  CEventConnection *FORTE_ModbusMaster::getEOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_INITO;
      case 1: return &conn_IND;
    }
    return nullptr;
  }

  CDataConnection **FORTE_ModbusMaster::getDIConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QI;
      case 1: return &conn_Host;
      case 2: return &conn_Port;
      case 3: return &conn_SlaveId;
      case 4: return &conn_PollIntervalMs;
      case 5: return &conn_ResponseTimeoutMs;
    }
    return nullptr;
  }

  CDataConnection *FORTE_ModbusMaster::getDOConUnchecked(const TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_QO;
      case 1: return &conn_STATUS;
    }
    return nullptr;
  }

} // namespace forte::eclipse4diac::io::modbus_io
