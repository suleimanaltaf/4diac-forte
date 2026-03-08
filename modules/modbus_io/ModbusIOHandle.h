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

#include "forte/io/mapper/io_handle.h"
#include "ModbusIOController.h"

#include <modbus/modbus.h>

namespace forte::eclipse4diac::io::modbus_io {

  class ModbusIOHandle : public forte::io::IOHandle {
    public:
      ModbusIOHandle(ModbusIOController *paController,
                     modbus_t *paModbusCtx,
                     CIEC_ANY::EDataTypeID paType,
                     forte::io::IOMapper::Direction paDirection,
                     ModbusFunctionType paFunctionType,
                     uint16_t paAddress);

      ~ModbusIOHandle() override;

      void set(const CIEC_ANY &paState) override;
      void get(CIEC_ANY &paState) override;

      bool check();

    protected:
      void dropObserver() override;

    private:
      modbus_t *mModbusCtx;
      ModbusFunctionType mFunctionType;
      uint16_t mAddress;
      CIEC_ANY *mValueLastCheck;
  };

} // namespace forte::eclipse4diac::io::modbus_io
