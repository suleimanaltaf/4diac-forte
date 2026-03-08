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

#include "ModbusIOController.h"
#include "ModbusIOHandle.h"

using namespace forte::io;

namespace forte::eclipse4diac::io::modbus_io {

  const char *const ModbusIOController::scmFailedToCreateContext = "Failed to create Modbus TCP context";
  const char *const ModbusIOController::scmFailedToConnect = "Failed to connect to Modbus device";
  const char *const ModbusIOController::scmFailedToSetSlave = "Failed to set Modbus slave ID";

  ModbusIOController::ModbusIOController(CDeviceExecution &paDeviceExecution) :
      IODeviceMultiController(paDeviceExecution) {
  }

  void ModbusIOController::setConfig(struct IODeviceController::Config *paConfig) {
    Config newConfig = *static_cast<Config *>(paConfig);

    if (newConfig.mPollIntervalMs == 0) {
      DEVLOG_WARNING("[ModbusIOController] PollInterval is 0, setting to default 100ms\n");
      newConfig.mPollIntervalMs = 100;
    }
    if (newConfig.mPort == 0) {
      DEVLOG_WARNING("[ModbusIOController] Port is 0, setting to default 502\n");
      newConfig.mPort = 502;
    }

    mConfig = newConfig;
  }

  const char *ModbusIOController::init() {
    mModbusCtx = modbus_new_tcp(mConfig.mHost.c_str(), mConfig.mPort);
    if (mModbusCtx == nullptr) {
      DEVLOG_ERROR("[ModbusIOController] %s: %s:%d\n", scmFailedToCreateContext, mConfig.mHost.c_str(), mConfig.mPort);
      return scmFailedToCreateContext;
    }

    // Set response timeout
    modbus_set_response_timeout(mModbusCtx, mConfig.mResponseTimeoutMs / 1000,
                                (mConfig.mResponseTimeoutMs % 1000) * 1000);

    if (modbus_set_slave(mModbusCtx, mConfig.mSlaveId) != 0) {
      DEVLOG_ERROR("[ModbusIOController] %s: %d\n", scmFailedToSetSlave, mConfig.mSlaveId);
      modbus_free(mModbusCtx);
      mModbusCtx = nullptr;
      return scmFailedToSetSlave;
    }

    if (modbus_connect(mModbusCtx) != 0) {
      DEVLOG_ERROR("[ModbusIOController] %s: %s:%d - %s\n", scmFailedToConnect, mConfig.mHost.c_str(), mConfig.mPort,
                   modbus_strerror(errno));
      modbus_free(mModbusCtx);
      mModbusCtx = nullptr;
      return scmFailedToConnect;
    }

    DEVLOG_INFO("[ModbusIOController] Connected to %s:%d (slave %d)\n", mConfig.mHost.c_str(), mConfig.mPort,
                mConfig.mSlaveId);

    return nullptr;
  }

  IOHandle *ModbusIOController::createIOHandle(IODeviceController::HandleDescriptor &paHandleDescriptor) {
    auto &desc = static_cast<HandleDescriptor &>(paHandleDescriptor);
    return new ModbusIOHandle(this, mModbusCtx, desc.mType, desc.mDirection, desc.mFunctionType, desc.mAddress);
  }

  void ModbusIOController::deInit() {
    if (mModbusCtx != nullptr) {
      modbus_close(mModbusCtx);
      modbus_free(mModbusCtx);
      mModbusCtx = nullptr;
      DEVLOG_INFO("[ModbusIOController] Disconnected\n");
    }
  }

  void ModbusIOController::runLoop() {
    while (isAlive()) {
      mTimeoutSemaphore.timedWait(
          static_cast<uint64_t>(mConfig.mPollIntervalMs) * 1000000ULL);

      this->checkForInputChanges();
    }
  }

  void ModbusIOController::addSlaveHandle(size_t, std::unique_ptr<IOHandle> paHandle) {
    forte::util::CCriticalRegion criticalRegion(mHandleMutex);
    if (paHandle->isInput()) {
      mInputHandles.push_back(std::move(paHandle));
    } else {
      mOutputHandles.push_back(std::move(paHandle));
    }
  }

  void ModbusIOController::dropSlaveHandles(size_t) {
    // Handled by base class dropHandles
  }

  bool ModbusIOController::isSlaveAvailable(size_t) {
    // For Modbus TCP, slaves are addressed by slave ID in the config.
    // We treat all slave indices as available since the connection is to a single device.
    return mModbusCtx != nullptr;
  }

  bool ModbusIOController::checkSlaveType(size_t, int) {
    // Modbus does not have a concept of typed slaves
    return true;
  }

  bool ModbusIOController::isHandleValueEqual(IOHandle &paHandle) {
    return !static_cast<ModbusIOHandle &>(paHandle).check();
  }

} // namespace forte::eclipse4diac::io::modbus_io
