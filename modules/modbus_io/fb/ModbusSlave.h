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

#pragma once

#include "forte/io/configFB/io_slave_multi.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_string.h"
#include "forte/datatypes/forte_wstring.h"
#include "forte/datatypes/forte_uint.h"
#include "ModbusBusAdapter.h"

namespace forte::eclipse4diac::io::modbus_io {

  //! Slave FB for Modbus Holding Registers
  //! Each data input configures a named holding register by Modbus address.
  //! Format of each register name: the IO identifier used for IOMapper binding.
  //! Addresses are configured sequentially starting from StartAddress.
  class FORTE_ModbusSlaveRegisters : public ::forte::io::IOConfigFBMultiSlave {
      DECLARE_FIRMWARE_FB(FORTE_ModbusSlaveRegisters)

    private:
      static const TEventID scmEventMAPID = 0;
      static const TEventID scmEventMAPOID = 0;
      static const TEventID scmEventINDID = 1;

      static const TForteUInt8 scmSlaveConfigurationIO[];
      static const TForteUInt8 scmSlaveConfigurationIONum;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;

    public:
      FORTE_ModbusSlaveRegisters(const forte::StringId paInstanceNameId, CFBContainer &paContainer);

      ~FORTE_ModbusSlaveRegisters() override = default;

      // Data inputs: QI, StartAddress, direction params, and 8 register slot names
      CIEC_BOOL var_QI;
      CIEC_UINT var_StartAddress;
      CIEC_BOOL var_IsReadOnly;
      CIEC_STRING var_Register_1;
      CIEC_STRING var_Register_2;
      CIEC_STRING var_Register_3;
      CIEC_STRING var_Register_4;
      CIEC_STRING var_Register_5;
      CIEC_STRING var_Register_6;
      CIEC_STRING var_Register_7;
      CIEC_STRING var_Register_8;

      CIEC_BOOL var_QO;
      CIEC_WSTRING var_STATUS;

      CEventConnection conn_MAPO;
      CEventConnection conn_IND;

      CDataConnection *conn_QI;
      CDataConnection *conn_StartAddress;
      CDataConnection *conn_IsReadOnly;
      CDataConnection *conn_Register_1;
      CDataConnection *conn_Register_2;
      CDataConnection *conn_Register_3;
      CDataConnection *conn_Register_4;
      CDataConnection *conn_Register_5;
      CDataConnection *conn_Register_6;
      CDataConnection *conn_Register_7;
      CDataConnection *conn_Register_8;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_WSTRING> conn_STATUS;

      forte::CPlugPin<FORTE_ModbusBusAdapter_Plug> var_BusAdapterOut;
      forte::CSocketPin<FORTE_ModbusBusAdapter_Socket> var_BusAdapterIn;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;

      forte::IPlugPin *getPlugPinUnchecked(size_t) override;
      forte::ISocketPin *getSocketPinUnchecked(size_t) override;

      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

    protected:
      void initHandles() override;
  };

  //! Slave FB for Modbus Coils
  //! Each data input configures a named coil by Modbus address.
  class FORTE_ModbusSlaveCoils : public ::forte::io::IOConfigFBMultiSlave {
      DECLARE_FIRMWARE_FB(FORTE_ModbusSlaveCoils)

    private:
      static const TEventID scmEventMAPID = 0;
      static const TEventID scmEventMAPOID = 0;
      static const TEventID scmEventINDID = 1;

      static const TForteUInt8 scmSlaveConfigurationIO[];
      static const TForteUInt8 scmSlaveConfigurationIONum;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;

    public:
      FORTE_ModbusSlaveCoils(const forte::StringId paInstanceNameId, CFBContainer &paContainer);

      ~FORTE_ModbusSlaveCoils() override = default;

      CIEC_BOOL var_QI;
      CIEC_UINT var_StartAddress;
      CIEC_BOOL var_IsReadOnly;
      CIEC_STRING var_Coil_1;
      CIEC_STRING var_Coil_2;
      CIEC_STRING var_Coil_3;
      CIEC_STRING var_Coil_4;
      CIEC_STRING var_Coil_5;
      CIEC_STRING var_Coil_6;
      CIEC_STRING var_Coil_7;
      CIEC_STRING var_Coil_8;

      CIEC_BOOL var_QO;
      CIEC_WSTRING var_STATUS;

      CEventConnection conn_MAPO;
      CEventConnection conn_IND;

      CDataConnection *conn_QI;
      CDataConnection *conn_StartAddress;
      CDataConnection *conn_IsReadOnly;
      CDataConnection *conn_Coil_1;
      CDataConnection *conn_Coil_2;
      CDataConnection *conn_Coil_3;
      CDataConnection *conn_Coil_4;
      CDataConnection *conn_Coil_5;
      CDataConnection *conn_Coil_6;
      CDataConnection *conn_Coil_7;
      CDataConnection *conn_Coil_8;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_WSTRING> conn_STATUS;

      forte::CPlugPin<FORTE_ModbusBusAdapter_Plug> var_BusAdapterOut;
      forte::CSocketPin<FORTE_ModbusBusAdapter_Socket> var_BusAdapterIn;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;

      forte::IPlugPin *getPlugPinUnchecked(size_t) override;
      forte::ISocketPin *getSocketPinUnchecked(size_t) override;

      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

    protected:
      void initHandles() override;
  };

} // namespace forte::eclipse4diac::io::modbus_io
