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

#include "forte/io/configFB/io_master_multi.h"
#include "ModbusBusAdapter.h"
#include "forte/adapter.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_uint.h"
#include "forte/datatypes/forte_wstring.h"
#include "forte/datatypes/forte_string.h"

namespace forte::eclipse4diac::io::modbus_io {

  class FORTE_ModbusMaster : public ::forte::io::IOConfigFBMultiMaster {
      DECLARE_FIRMWARE_FB(FORTE_ModbusMaster)

    private:
      static const TEventID scmEventINITID = 0;
      static const TEventID scmEventINITOID = 0;
      static const TEventID scmEventINDID = 1;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;

      ::forte::io::IODeviceController *createDeviceController(CDeviceExecution &paDeviceExecution) override;
      void setConfig() override;

    public:
      FORTE_ModbusMaster(const StringId paInstanceNameId, CFBContainer &paContainer);

      ~FORTE_ModbusMaster() override = default;

      CIEC_BOOL var_QI;
      CIEC_STRING var_Host;
      CIEC_UINT var_Port;
      CIEC_UINT var_SlaveId;
      CIEC_UINT var_PollIntervalMs;
      CIEC_UINT var_ResponseTimeoutMs;

      CIEC_BOOL var_QO;
      CIEC_WSTRING var_STATUS;

      CEventConnection conn_INITO;
      CEventConnection conn_IND;

      CDataConnection *conn_QI;
      CDataConnection *conn_Host;
      CDataConnection *conn_Port;
      CDataConnection *conn_SlaveId;
      CDataConnection *conn_PollIntervalMs;
      CDataConnection *conn_ResponseTimeoutMs;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_WSTRING> conn_STATUS;

      CPlugPin<FORTE_ModbusBusAdapter_Plug> var_BusAdapterOut;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;

      IPlugPin *getPlugPinUnchecked(size_t) override;

      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

      void evt_INIT(const CIEC_BOOL &paQI,
                    const CIEC_STRING &paHost,
                    const CIEC_UINT &paPort,
                    const CIEC_UINT &paSlaveId,
                    const CIEC_UINT &paPollIntervalMs,
                    const CIEC_UINT &paResponseTimeoutMs,
                    CAnyBitOutputParameter<CIEC_BOOL> paQO,
                    COutputParameter<CIEC_WSTRING> paSTATUS) {
        COutputGuard guard_paQO(paQO);
        COutputGuard guard_paSTATUS(paSTATUS);
        var_QI = paQI;
        var_Host = paHost;
        var_Port = paPort;
        var_SlaveId = paSlaveId;
        var_PollIntervalMs = paPollIntervalMs;
        var_ResponseTimeoutMs = paResponseTimeoutMs;
        executeEvent(scmEventINITID, nullptr);
        *paQO = var_QO;
        *paSTATUS = var_STATUS;
      }

      void operator()(const CIEC_BOOL &paQI,
                      const CIEC_STRING &paHost,
                      const CIEC_UINT &paPort,
                      const CIEC_UINT &paSlaveId,
                      const CIEC_UINT &paPollIntervalMs,
                      const CIEC_UINT &paResponseTimeoutMs,
                      CAnyBitOutputParameter<CIEC_BOOL> paQO,
                      COutputParameter<CIEC_WSTRING> paSTATUS) {
        evt_INIT(paQI, paHost, paPort, paSlaveId, paPollIntervalMs, paResponseTimeoutMs, paQO, paSTATUS);
      }
  };

} // namespace forte::eclipse4diac::io::modbus_io
