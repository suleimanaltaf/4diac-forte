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

#include "ModbusIOHandle.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_word.h"
#include "forte/datatypes/forte_dword.h"

using namespace forte::io;

namespace forte::eclipse4diac::io::modbus_io {

  ModbusIOHandle::ModbusIOHandle(ModbusIOController *paController,
                                 modbus_t *paModbusCtx,
                                 CIEC_ANY::EDataTypeID paType,
                                 IOMapper::Direction paDirection,
                                 ModbusFunctionType paFunctionType,
                                 uint16_t paAddress) :
      IOHandle(paController, paDirection, paType),
      mModbusCtx(paModbusCtx),
      mFunctionType(paFunctionType),
      mAddress(paAddress),
      mValueLastCheck(nullptr) {
    switch (mType) {
      case CIEC_ANY::e_BOOL: mValueLastCheck = new CIEC_BOOL; break;
      case CIEC_ANY::e_WORD: mValueLastCheck = new CIEC_WORD; break;
      case CIEC_ANY::e_DWORD: mValueLastCheck = new CIEC_DWORD; break;
      default: break;
    }
  }

  ModbusIOHandle::~ModbusIOHandle() {
    delete mValueLastCheck;
  }

  void ModbusIOHandle::set(const CIEC_ANY &paState) {
    if (mModbusCtx == nullptr) {
      return;
    }

    switch (mFunctionType) {
      case ModbusFunctionType::Coil: {
        int value = static_cast<const CIEC_BOOL &>(paState) ? TRUE : FALSE;
        if (modbus_write_bit(mModbusCtx, mAddress, value) == -1) {
          DEVLOG_ERROR("[ModbusIOHandle] Failed to write coil at address %d: %s\n", mAddress,
                       modbus_strerror(errno));
        }
        break;
      }
      case ModbusFunctionType::HoldingRegister: {
        if (mType == CIEC_ANY::e_WORD) {
          auto value = static_cast<uint16_t>(static_cast<TForteWord>(static_cast<const CIEC_WORD &>(paState)));
          if (modbus_write_register(mModbusCtx, mAddress, value) == -1) {
            DEVLOG_ERROR("[ModbusIOHandle] Failed to write register at address %d: %s\n", mAddress,
                         modbus_strerror(errno));
          }
        } else if (mType == CIEC_ANY::e_DWORD) {
          auto value = static_cast<TForteDWord>(static_cast<const CIEC_DWORD &>(paState));
          uint16_t regs[2];
          regs[0] = static_cast<uint16_t>(value >> 16);
          regs[1] = static_cast<uint16_t>(value & 0xFFFF);
          if (modbus_write_registers(mModbusCtx, mAddress, 2, regs) == -1) {
            DEVLOG_ERROR("[ModbusIOHandle] Failed to write registers at address %d: %s\n", mAddress,
                         modbus_strerror(errno));
          }
        }
        break;
      }
      default:
        DEVLOG_WARNING("[ModbusIOHandle] Cannot write to read-only function type at address %d\n", mAddress);
        break;
    }
  }

  void ModbusIOHandle::get(CIEC_ANY &paState) {
    if (mModbusCtx == nullptr) {
      return;
    }

    switch (mFunctionType) {
      case ModbusFunctionType::Coil: {
        uint8_t value = 0;
        if (modbus_read_bits(mModbusCtx, mAddress, 1, &value) == -1) {
          DEVLOG_ERROR("[ModbusIOHandle] Failed to read coil at address %d: %s\n", mAddress,
                       modbus_strerror(errno));
          return;
        }
        static_cast<CIEC_BOOL &>(paState) = CIEC_BOOL(value != 0);
        break;
      }
      case ModbusFunctionType::DiscreteInput: {
        uint8_t value = 0;
        if (modbus_read_input_bits(mModbusCtx, mAddress, 1, &value) == -1) {
          DEVLOG_ERROR("[ModbusIOHandle] Failed to read discrete input at address %d: %s\n", mAddress,
                       modbus_strerror(errno));
          return;
        }
        static_cast<CIEC_BOOL &>(paState) = CIEC_BOOL(value != 0);
        break;
      }
      case ModbusFunctionType::HoldingRegister: {
        if (mType == CIEC_ANY::e_WORD) {
          uint16_t value = 0;
          if (modbus_read_registers(mModbusCtx, mAddress, 1, &value) == -1) {
            DEVLOG_ERROR("[ModbusIOHandle] Failed to read holding register at address %d: %s\n", mAddress,
                         modbus_strerror(errno));
            return;
          }
          static_cast<CIEC_WORD &>(paState) = CIEC_WORD(static_cast<TForteWord>(value));
        } else if (mType == CIEC_ANY::e_DWORD) {
          uint16_t regs[2] = {0, 0};
          if (modbus_read_registers(mModbusCtx, mAddress, 2, regs) == -1) {
            DEVLOG_ERROR("[ModbusIOHandle] Failed to read holding registers at address %d: %s\n", mAddress,
                         modbus_strerror(errno));
            return;
          }
          auto value = static_cast<TForteDWord>((static_cast<uint32_t>(regs[0]) << 16) | regs[1]);
          static_cast<CIEC_DWORD &>(paState) = CIEC_DWORD(value);
        }
        break;
      }
      case ModbusFunctionType::InputRegister: {
        if (mType == CIEC_ANY::e_WORD) {
          uint16_t value = 0;
          if (modbus_read_input_registers(mModbusCtx, mAddress, 1, &value) == -1) {
            DEVLOG_ERROR("[ModbusIOHandle] Failed to read input register at address %d: %s\n", mAddress,
                         modbus_strerror(errno));
            return;
          }
          static_cast<CIEC_WORD &>(paState) = CIEC_WORD(static_cast<TForteWord>(value));
        } else if (mType == CIEC_ANY::e_DWORD) {
          uint16_t regs[2] = {0, 0};
          if (modbus_read_input_registers(mModbusCtx, mAddress, 2, regs) == -1) {
            DEVLOG_ERROR("[ModbusIOHandle] Failed to read input registers at address %d: %s\n", mAddress,
                         modbus_strerror(errno));
            return;
          }
          auto value = static_cast<TForteDWord>((static_cast<uint32_t>(regs[0]) << 16) | regs[1]);
          static_cast<CIEC_DWORD &>(paState) = CIEC_DWORD(value);
        }
        break;
      }
    }
  }

  bool ModbusIOHandle::check() {
    if (mValueLastCheck == nullptr) {
      return false;
    }

    bool changed = false;
    switch (mType) {
      case CIEC_ANY::e_BOOL: {
        CIEC_BOOL value;
        get(value);
        changed = value != *static_cast<CIEC_BOOL *>(mValueLastCheck);
        mValueLastCheck->setValue(value);
        break;
      }
      case CIEC_ANY::e_WORD: {
        CIEC_WORD value;
        get(value);
        changed = value != *static_cast<CIEC_WORD *>(mValueLastCheck);
        mValueLastCheck->setValue(value);
        break;
      }
      case CIEC_ANY::e_DWORD: {
        CIEC_DWORD value;
        get(value);
        changed = value != *static_cast<CIEC_DWORD *>(mValueLastCheck);
        mValueLastCheck->setValue(value);
        break;
      }
      default: break;
    }
    return changed;
  }

  void ModbusIOHandle::dropObserver() {
    // Reset output to zero when observer is dropped
    if (isOutput()) {
      switch (mFunctionType) {
        case ModbusFunctionType::Coil:
          set(CIEC_BOOL(false));
          break;
        case ModbusFunctionType::HoldingRegister:
          if (mType == CIEC_ANY::e_WORD) {
            set(CIEC_WORD(0));
          } else if (mType == CIEC_ANY::e_DWORD) {
            set(CIEC_DWORD(0));
          }
          break;
        default: break;
      }
    }
    IOHandle::dropObserver();
  }

} // namespace forte::eclipse4diac::io::modbus_io
