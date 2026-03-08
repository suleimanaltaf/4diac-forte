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

#include "forte/io/device/io_controller_multi.h"
#include "forte/arch/forte_sem.h"

#include <modbus/modbus.h>
#include <vector>
#include <memory>

namespace forte::eclipse4diac::io::modbus_io {

  class ModbusIOHandle;

  enum class ModbusFunctionType : uint8_t {
    Coil = 0,
    DiscreteInput = 1,
    HoldingRegister = 2,
    InputRegister = 3
  };

  class ModbusIOController : public ::forte::io::IODeviceMultiController {
    public:
      explicit ModbusIOController(CDeviceExecution &paDeviceExecution);

      struct Config : IODeviceController::Config {
        std::string mHost;
        uint16_t mPort = 502;
        uint8_t mSlaveId = 1;
        unsigned int mPollIntervalMs = 100;
        unsigned int mResponseTimeoutMs = 1000;
      };

      struct HandleDescriptor : IODeviceMultiController::HandleDescriptor {
        ModbusFunctionType mFunctionType;
        uint16_t mAddress;
        CIEC_ANY::EDataTypeID mType;

        HandleDescriptor(std::string const &paID,
                         forte::io::IOMapper::Direction paDirection,
                         size_t paSlaveIndex,
                         ModbusFunctionType paFunctionType,
                         uint16_t paAddress,
                         CIEC_ANY::EDataTypeID paType) :
            IODeviceMultiController::HandleDescriptor(paID, paDirection, paSlaveIndex),
            mFunctionType(paFunctionType),
            mAddress(paAddress),
            mType(paType) {
        }
      };

      void setConfig(struct IODeviceController::Config *paConfig) override;

      void addSlaveHandle(size_t paIndex, std::unique_ptr<forte::io::IOHandle> paHandle) override;
      void dropSlaveHandles(size_t paIndex) override;

    protected:
      const char *init() override;
      forte::io::IOHandle *createIOHandle(IODeviceController::HandleDescriptor &paHandleDescriptor) override;
      void deInit() override;
      void runLoop() override;

    private:
      bool isSlaveAvailable(size_t paIndex) override;
      bool checkSlaveType(size_t paIndex, int paType) override;
      bool isHandleValueEqual(forte::io::IOHandle &paHandle) override;

      modbus_t *mModbusCtx = nullptr;
      Config mConfig;
      arch::CSemaphore mTimeoutSemaphore;

      static const char *const scmFailedToCreateContext;
      static const char *const scmFailedToConnect;
      static const char *const scmFailedToSetSlave;
  };

} // namespace forte::eclipse4diac::io::modbus_io
