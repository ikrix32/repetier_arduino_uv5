/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

    This firmware is a nearly complete rewrite of the sprinter firmware
    by kliment (https://github.com/kliment/Sprinter)
    which based on Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.

  Functions in this file are used to communicate using ascii or repetier protocol.
*/

#include "Repetier.h"


void EEPROM::update(GCode *com)
{
#if EEPROM_MODE != 0
    if(com->hasT() && com->hasP()) switch(com->T)
        {
        case 0:
            if(com->hasS()) HAL::eprSetByte(com->P,(uint8_t)com->S);
            break;
        case 1:
            if(com->hasS()) HAL::eprSetInt16(com->P,(int)com->S);
            break;
        case 2:
            if(com->hasS()) HAL::eprSetInt32(com->P,(int32_t)com->S);
            break;
        case 3:
            if(com->hasX()) HAL::eprSetFloat(com->P,com->X);
            break;
        }
    uint8_t newcheck = computeChecksum();
    if(newcheck != HAL::eprGetByte(EPR_INTEGRITY_BYTE))
        HAL::eprSetByte(EPR_INTEGRITY_BYTE,newcheck);
    readDataFromEEPROM();
    Extruder::selectExtruderById(Extruder::current->id);
#else
    Console::printErrorF(Messages::tNoEEPROMSupport);
#endif
}

void EEPROM::restoreEEPROMSettingsFromConfiguration()
{
#if EEPROM_MODE != 0
    baudrate = REP_BAUDRATE;
    maxInactiveTime = MAX_INACTIVE_TIME*1000L;
    stepperInactiveTime = STEPPER_INACTIVE_TIME*1000L;
    Printer::axisStepsPerMM[X_AXIS] = XAXIS_STEPS_PER_MM;
    Printer::axisStepsPerMM[Y_AXIS] = YAXIS_STEPS_PER_MM;
    Printer::axisStepsPerMM[Z_AXIS] = ZAXIS_STEPS_PER_MM;
    Printer::axisStepsPerMM[E_AXIS] = 1;
    Printer::maxFeedrate[X_AXIS] = MAX_FEEDRATE_X;
    Printer::maxFeedrate[Y_AXIS] = MAX_FEEDRATE_Y;
    Printer::maxFeedrate[Z_AXIS] = MAX_FEEDRATE_Z;
    Printer::homingFeedrate[X_AXIS] = HOMING_FEEDRATE_X;
    Printer::homingFeedrate[Y_AXIS] = HOMING_FEEDRATE_Y;
    Printer::homingFeedrate[Z_AXIS] = HOMING_FEEDRATE_Z;
    Printer::maxJerk = MAX_JERK;
#if DRIVE_SYSTEM != DELTA
    Printer::maxZJerk = MAX_ZJERK;
#endif
#if RAMP_ACCELERATION
    Printer::maxAccelerationMMPerSquareSecond[X_AXIS] = MAX_ACCELERATION_UNITS_PER_SQ_SECOND_X;
    Printer::maxAccelerationMMPerSquareSecond[Y_AXIS] = MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Y;
    Printer::maxAccelerationMMPerSquareSecond[Z_AXIS] = MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Z;
    Printer::maxTravelAccelerationMMPerSquareSecond[X_AXIS] = MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_X;
    Printer::maxTravelAccelerationMMPerSquareSecond[Y_AXIS] = MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Y;
    Printer::maxTravelAccelerationMMPerSquareSecond[Z_AXIS] = MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Z;
#endif
#if HAVE_HEATED_BED
    heatedBedController.heatManager= HEATED_BED_HEAT_MANAGER;
#if TEMP_PID
    heatedBedController.pidDriveMax = HEATED_BED_PID_INTEGRAL_DRIVE_MAX;
    heatedBedController.pidDriveMin = HEATED_BED_PID_INTEGRAL_DRIVE_MIN;
    heatedBedController.pidPGain = HEATED_BED_PID_PGAIN_OR_DEAD_TIME;
    heatedBedController.pidIGain = HEATED_BED_PID_IGAIN;
    heatedBedController.pidDGain = HEATED_BED_PID_DGAIN;
    heatedBedController.pidMax = HEATED_BED_PID_MAX;
#endif
#endif
    Printer::xLength = X_MAX_LENGTH;
    Printer::yLength = Y_MAX_LENGTH;
    Printer::zLength = Z_MAX_LENGTH;
    Printer::xMin = X_MIN_POS;
    Printer::yMin = Y_MIN_POS;
    Printer::zMin = Z_MIN_POS;
#if ENABLE_BACKLASH_COMPENSATION
    Printer::backlashX = X_BACKLASH;
    Printer::backlashY = Y_BACKLASH;
    Printer::backlashZ = Z_BACKLASH;
#endif
    Extruder *e;
#if NUM_EXTRUDER>0
    e = &extruder[0];
    e->stepsPerMM = EXT0_STEPS_PER_MM;
    e->maxFeedrate = EXT0_MAX_FEEDRATE;
    e->maxStartFeedrate = EXT0_MAX_START_FEEDRATE;
    e->maxAcceleration = EXT0_MAX_ACCELERATION;
    e->tempControl.heatManager = EXT0_HEAT_MANAGER;
#if TEMP_PID
    e->tempControl.pidDriveMax = EXT0_PID_INTEGRAL_DRIVE_MAX;
    e->tempControl.pidDriveMin = EXT0_PID_INTEGRAL_DRIVE_MIN;
    e->tempControl.pidPGain = EXT0_PID_PGAIN_OR_DEAD_TIME;
    e->tempControl.pidIGain = EXT0_PID_I;
    e->tempControl.pidDGain = EXT0_PID_D;
    e->tempControl.pidMax = EXT0_PID_MAX;
#endif
    e->yOffset = EXT0_Y_OFFSET;
    e->xOffset = EXT0_X_OFFSET;
    e->watchPeriod = EXT0_WATCHPERIOD;
#if RETRACT_DURING_HEATUP
    e->waitRetractTemperature = EXT0_WAIT_RETRACT_TEMP;
    e->waitRetractUnits = EXT0_WAIT_RETRACT_UNITS;
#endif
    e->coolerSpeed = EXT0_EXTRUDER_COOLER_SPEED;
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
    e->advanceK = EXT0_ADVANCE_K;
#endif
    e->advanceL = EXT0_ADVANCE_L;
#endif
#endif // NUM_EXTRUDER>0
#if NUM_EXTRUDER>1
    e = &extruder[1];
    e->stepsPerMM = EXT1_STEPS_PER_MM;
    e->maxFeedrate = EXT1_MAX_FEEDRATE;
    e->maxStartFeedrate = EXT1_MAX_START_FEEDRATE;
    e->maxAcceleration = EXT1_MAX_ACCELERATION;
    e->tempControl.heatManager = EXT1_HEAT_MANAGER;
#if TEMP_PID
    e->tempControl.pidDriveMax = EXT1_PID_INTEGRAL_DRIVE_MAX;
    e->tempControl.pidDriveMin = EXT1_PID_INTEGRAL_DRIVE_MIN;
    e->tempControl.pidPGain = EXT1_PID_PGAIN_OR_DEAD_TIME;
    e->tempControl.pidIGain = EXT1_PID_I;
    e->tempControl.pidDGain = EXT1_PID_D;
    e->tempControl.pidMax = EXT1_PID_MAX;
#endif
    e->yOffset = EXT1_Y_OFFSET;
    e->xOffset = EXT1_X_OFFSET;
    e->watchPeriod = EXT1_WATCHPERIOD;
#if RETRACT_DURING_HEATUP
    e->waitRetractTemperature = EXT1_WAIT_RETRACT_TEMP;
    e->waitRetractUnits = EXT1_WAIT_RETRACT_UNITS;
#endif
    e->coolerSpeed = EXT1_EXTRUDER_COOLER_SPEED;
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
    e->advanceK = EXT1_ADVANCE_K;
#endif
    e->advanceL = EXT1_ADVANCE_L;
#endif
#endif // NUM_EXTRUDER > 1
#if NUM_EXTRUDER>2
    e = &extruder[2];
    e->stepsPerMM = EXT2_STEPS_PER_MM;
    e->maxFeedrate = EXT2_MAX_FEEDRATE;
    e->maxStartFeedrate = EXT2_MAX_START_FEEDRATE;
    e->maxAcceleration = EXT2_MAX_ACCELERATION;
    e->tempControl.heatManager = EXT2_HEAT_MANAGER;
#if TEMP_PID
    e->tempControl.pidDriveMax = EXT2_PID_INTEGRAL_DRIVE_MAX;
    e->tempControl.pidDriveMin = EXT2_PID_INTEGRAL_DRIVE_MIN;
    e->tempControl.pidPGain = EXT2_PID_PGAIN_OR_DEAD_TIME;
    e->tempControl.pidIGain = EXT2_PID_I;
    e->tempControl.pidDGain = EXT2_PID_D;
    e->tempControl.pidMax = EXT2_PID_MAX;
#endif
    e->yOffset = EXT2_Y_OFFSET;
    e->xOffset = EXT2_X_OFFSET;
    e->watchPeriod = EXT2_WATCHPERIOD;
#if RETRACT_DURING_HEATUP
    e->waitRetractTemperature = EXT2_WAIT_RETRACT_TEMP;
    e->waitRetractUnits = EXT2_WAIT_RETRACT_UNITS;
#endif
    e->coolerSpeed = EXT2_EXTRUDER_COOLER_SPEED;
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
    e->advanceK = EXT2_ADVANCE_K;
#endif
    e->advanceL = EXT2_ADVANCE_L;
#endif
#endif // NUM_EXTRUDER > 2
#if NUM_EXTRUDER>3
    e = &extruder[3];
    e->stepsPerMM = EXT3_STEPS_PER_MM;
    e->maxFeedrate = EXT3_MAX_FEEDRATE;
    e->maxStartFeedrate = EXT3_MAX_START_FEEDRATE;
    e->maxAcceleration = EXT3_MAX_ACCELERATION;
    e->tempControl.heatManager = EXT3_HEAT_MANAGER;
#if TEMP_PID
    e->tempControl.pidDriveMax = EXT3_PID_INTEGRAL_DRIVE_MAX;
    e->tempControl.pidDriveMin = EXT3_PID_INTEGRAL_DRIVE_MIN;
    e->tempControl.pidPGain = EXT3_PID_PGAIN_OR_DEAD_TIME;
    e->tempControl.pidIGain = EXT3_PID_I;
    e->tempControl.pidDGain = EXT3_PID_D;
    e->tempControl.pidMax = EXT3_PID_MAX;
#endif
    e->yOffset = EXT3_Y_OFFSET;
    e->xOffset = EXT3_X_OFFSET;
    e->watchPeriod = EXT3_WATCHPERIOD;
#if RETRACT_DURING_HEATUP
    e->waitRetractTemperature = EXT3_WAIT_RETRACT_TEMP;
    e->waitRetractUnits = EXT3_WAIT_RETRACT_UNITS;
#endif
    e->coolerSpeed = EXT3_EXTRUDER_COOLER_SPEED;
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
    e->advanceK = EXT3_ADVANCE_K;
#endif
    e->advanceL = EXT3_ADVANCE_L;
#endif
#endif // NUM_EXTRUDER > 3
#if NUM_EXTRUDER>4
    e = &extruder[4];
    e->stepsPerMM = EXT4_STEPS_PER_MM;
    e->maxFeedrate = EXT4_MAX_FEEDRATE;
    e->maxStartFeedrate = EXT4_MAX_START_FEEDRATE;
    e->maxAcceleration = EXT4_MAX_ACCELERATION;
    e->tempControl.heatManager = EXT4_HEAT_MANAGER;
#if TEMP_PID
    e->tempControl.pidDriveMax = EXT4_PID_INTEGRAL_DRIVE_MAX;
    e->tempControl.pidDriveMin = EXT4_PID_INTEGRAL_DRIVE_MIN;
    e->tempControl.pidPGain = EXT4_PID_PGAIN_OR_DEAD_TIME;
    e->tempControl.pidIGain = EXT4_PID_I;
    e->tempControl.pidDGain = EXT4_PID_D;
    e->tempControl.pidMax = EXT4_PID_MAX;
#endif
    e->yOffset = EXT4_Y_OFFSET;
    e->xOffset = EXT4_X_OFFSET;
    e->watchPeriod = EXT4_WATCHPERIOD;
#if RETRACT_DURING_HEATUP
    e->waitRetractTemperature = EXT4_WAIT_RETRACT_TEMP;
    e->waitRetractUnits = EXT4_WAIT_RETRACT_UNITS;
#endif
    e->coolerSpeed = EXT4_EXTRUDER_COOLER_SPEED;
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
    e->advanceK = EXT4_ADVANCE_K;
#endif
    e->advanceL = EXT4_ADVANCE_L;
#endif
#endif // NUM_EXTRUDER > 4
#if NUM_EXTRUDER>5
    e = &extruder[5];
    e->stepsPerMM = EXT5_STEPS_PER_MM;
    e->maxFeedrate = EXT5_MAX_FEEDRATE;
    e->maxStartFeedrate = EXT5_MAX_START_FEEDRATE;
    e->maxAcceleration = EXT5_MAX_ACCELERATION;
    e->tempControl.heatManager = EXT5_HEAT_MANAGER;
#if TEMP_PID
    e->tempControl.pidDriveMax = EXT5_PID_INTEGRAL_DRIVE_MAX;
    e->tempControl.pidDriveMin = EXT5_PID_INTEGRAL_DRIVE_MIN;
    e->tempControl.pidPGain = EXT5_PID_PGAIN_OR_DEAD_TIME;
    e->tempControl.pidIGain = EXT5_PID_I;
    e->tempControl.pidDGain = EXT5_PID_D;
    e->tempControl.pidMax = EXT5_PID_MAX;
#endif
    e->yOffset = EXT5_Y_OFFSET;
    e->xOffset = EXT5_X_OFFSET;
    e->watchPeriod = EXT5_WATCHPERIOD;
#if RETRACT_DURING_HEATUP
    e->waitRetractTemperature = EXT5_WAIT_RETRACT_TEMP;
    e->waitRetractUnits = EXT5_WAIT_RETRACT_UNITS;
#endif
    e->coolerSpeed = EXT5_EXTRUDER_COOLER_SPEED;
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
    e->advanceK = EXT5_ADVANCE_K;
#endif
    e->advanceL = EXT5_ADVANCE_L;
#endif
#endif // NUM_EXTRUDER > 5
#if FEATURE_AUTOLEVEL
    Printer::setAutolevelActive(false);
    Printer::resetTransformationMatrix(true);
#endif
#if MIXING_EXTRUDER
    restoreMixingRatios();
#endif
    initalizeUncached();
    Printer::updateDerivedParameter();
    Extruder::selectExtruderById(Extruder::current->id);
    Extruder::initHeatedBed();
    Console::printInfoF(Messages::tEPRConfigResetDefaults);
#else
    Console::printErrorF(Messages::tNoEEPROMSupport);
#endif

}

void EEPROM::storeDataIntoEEPROM(uint8_t corrupted)
{
#if EEPROM_MODE != 0
    HAL::eprSetInt32(EPR_BAUDRATE,baudrate);
    HAL::eprSetInt32(EPR_MAX_INACTIVE_TIME,maxInactiveTime);
    HAL::eprSetInt32(EPR_STEPPER_INACTIVE_TIME,stepperInactiveTime);
//#define EPR_ACCELERATION_TYPE 1
    HAL::eprSetFloat(EPR_XAXIS_STEPS_PER_MM,Printer::axisStepsPerMM[X_AXIS]);
    HAL::eprSetFloat(EPR_YAXIS_STEPS_PER_MM,Printer::axisStepsPerMM[Y_AXIS]);
    HAL::eprSetFloat(EPR_ZAXIS_STEPS_PER_MM,Printer::axisStepsPerMM[Z_AXIS]);
    HAL::eprSetFloat(EPR_X_MAX_FEEDRATE,Printer::maxFeedrate[X_AXIS]);
    HAL::eprSetFloat(EPR_Y_MAX_FEEDRATE,Printer::maxFeedrate[Y_AXIS]);
    HAL::eprSetFloat(EPR_Z_MAX_FEEDRATE,Printer::maxFeedrate[Z_AXIS]);
    HAL::eprSetFloat(EPR_X_HOMING_FEEDRATE,Printer::homingFeedrate[X_AXIS]);
    HAL::eprSetFloat(EPR_Y_HOMING_FEEDRATE,Printer::homingFeedrate[Y_AXIS]);
    HAL::eprSetFloat(EPR_Z_HOMING_FEEDRATE,Printer::homingFeedrate[Z_AXIS]);
    HAL::eprSetFloat(EPR_MAX_JERK,Printer::maxJerk);
#if DRIVE_SYSTEM != DELTA
    HAL::eprSetFloat(EPR_MAX_ZJERK,Printer::maxZJerk);
#endif
#if RAMP_ACCELERATION
    HAL::eprSetFloat(EPR_X_MAX_ACCEL,Printer::maxAccelerationMMPerSquareSecond[X_AXIS]);
    HAL::eprSetFloat(EPR_Y_MAX_ACCEL,Printer::maxAccelerationMMPerSquareSecond[Y_AXIS]);
    HAL::eprSetFloat(EPR_Z_MAX_ACCEL,Printer::maxAccelerationMMPerSquareSecond[Z_AXIS]);
    HAL::eprSetFloat(EPR_X_MAX_TRAVEL_ACCEL,Printer::maxTravelAccelerationMMPerSquareSecond[X_AXIS]);
    HAL::eprSetFloat(EPR_Y_MAX_TRAVEL_ACCEL,Printer::maxTravelAccelerationMMPerSquareSecond[Y_AXIS]);
    HAL::eprSetFloat(EPR_Z_MAX_TRAVEL_ACCEL,Printer::maxTravelAccelerationMMPerSquareSecond[Z_AXIS]);
#endif
#if HAVE_HEATED_BED
    HAL::eprSetByte(EPR_BED_HEAT_MANAGER,heatedBedController.heatManager);
#else
    HAL::eprSetByte(EPR_BED_HEAT_MANAGER,HEATED_BED_HEAT_MANAGER);
#endif
#if defined(TEMP_PID) && HAVE_HEATED_BED
    HAL::eprSetByte(EPR_BED_DRIVE_MAX,heatedBedController.pidDriveMax);
    HAL::eprSetByte(EPR_BED_DRIVE_MIN,heatedBedController.pidDriveMin);
    HAL::eprSetFloat(EPR_BED_PID_PGAIN,heatedBedController.pidPGain);
    HAL::eprSetFloat(EPR_BED_PID_IGAIN,heatedBedController.pidIGain);
    HAL::eprSetFloat(EPR_BED_PID_DGAIN,heatedBedController.pidDGain);
    HAL::eprSetByte(EPR_BED_PID_MAX,heatedBedController.pidMax);
#else
    HAL::eprSetByte(EPR_BED_DRIVE_MAX,HEATED_BED_PID_INTEGRAL_DRIVE_MAX);
    HAL::eprSetByte(EPR_BED_DRIVE_MIN,HEATED_BED_PID_INTEGRAL_DRIVE_MIN);
    HAL::eprSetFloat(EPR_BED_PID_PGAIN,HEATED_BED_PID_PGAIN_OR_DEAD_TIME);
    HAL::eprSetFloat(EPR_BED_PID_IGAIN,HEATED_BED_PID_IGAIN);
    HAL::eprSetFloat(EPR_BED_PID_DGAIN,HEATED_BED_PID_DGAIN);
    HAL::eprSetByte(EPR_BED_PID_MAX,HEATED_BED_PID_MAX);
#endif
    //SHOT("storeDataIntoEEPROM"); SHOWM(Printer::xMin);SHOWM(Printer::yMin);SHOWM(Printer::zMin);
    HAL::eprSetFloat(EPR_X_HOME_OFFSET,Printer::xMin);
    HAL::eprSetFloat(EPR_Y_HOME_OFFSET,Printer::yMin);
    HAL::eprSetFloat(EPR_Z_HOME_OFFSET,Printer::zMin);
    HAL::eprSetFloat(EPR_X_LENGTH,Printer::xLength);
    HAL::eprSetFloat(EPR_Y_LENGTH,Printer::yLength);
    HAL::eprSetFloat(EPR_Z_LENGTH,Printer::zLength);
#if NONLINEAR_SYSTEM
    HAL::eprSetFloat(EPR_DELTA_HORIZONTAL_RADIUS, Printer::radius0);
#endif
#if ENABLE_BACKLASH_COMPENSATION
    HAL::eprSetFloat(EPR_BACKLASH_X,Printer::backlashX);
    HAL::eprSetFloat(EPR_BACKLASH_Y,Printer::backlashY);
    HAL::eprSetFloat(EPR_BACKLASH_Z,Printer::backlashZ);
#else
    HAL::eprSetFloat(EPR_BACKLASH_X,0);
    HAL::eprSetFloat(EPR_BACKLASH_Y,0);
    HAL::eprSetFloat(EPR_BACKLASH_Z,0);
#endif
#if FEATURE_AUTOLEVEL
    HAL::eprSetByte(EPR_AUTOLEVEL_ACTIVE,Printer::isAutolevelActive());
    for(uint8_t i = 0; i < 9; i++)
        HAL::eprSetFloat(EPR_AUTOLEVEL_MATRIX + (((int)i) << 2),Printer::autolevelTransformation[i]);
#endif
    // now the extruder
    for(uint8_t i = 0; i < NUM_EXTRUDER; i++)
    {
#if FEATURE_WATCHDOG
        HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

        int o=i*EEPROM_EXTRUDER_LENGTH+EEPROM_EXTRUDER_OFFSET;
        Extruder *e = &extruder[i];
        HAL::eprSetFloat(o+EPR_EXTRUDER_STEPS_PER_MM,e->stepsPerMM);
        HAL::eprSetFloat(o+EPR_EXTRUDER_MAX_FEEDRATE,e->maxFeedrate);
        HAL::eprSetFloat(o+EPR_EXTRUDER_MAX_START_FEEDRATE,e->maxStartFeedrate);
        HAL::eprSetFloat(o+EPR_EXTRUDER_MAX_ACCELERATION,e->maxAcceleration);
        HAL::eprSetByte(o+EPR_EXTRUDER_HEAT_MANAGER,e->tempControl.heatManager);
#if TEMP_PID
        HAL::eprSetByte(o+EPR_EXTRUDER_DRIVE_MAX,e->tempControl.pidDriveMax);
        HAL::eprSetByte(o+EPR_EXTRUDER_DRIVE_MIN,e->tempControl.pidDriveMin);
        HAL::eprSetFloat(o+EPR_EXTRUDER_PID_PGAIN,e->tempControl.pidPGain);
        HAL::eprSetFloat(o+EPR_EXTRUDER_PID_IGAIN,e->tempControl.pidIGain);
        HAL::eprSetFloat(o+EPR_EXTRUDER_PID_DGAIN,e->tempControl.pidDGain);
        HAL::eprSetByte(o+EPR_EXTRUDER_PID_MAX,e->tempControl.pidMax);
#endif
        HAL::eprSetInt32(o+EPR_EXTRUDER_X_OFFSET,e->xOffset);
        HAL::eprSetInt32(o+EPR_EXTRUDER_Y_OFFSET,e->yOffset);
        HAL::eprSetInt16(o+EPR_EXTRUDER_WATCH_PERIOD,e->watchPeriod);
#if RETRACT_DURING_HEATUP
        HAL::eprSetInt16(o+EPR_EXTRUDER_WAIT_RETRACT_TEMP,e->waitRetractTemperature);
        HAL::eprSetInt16(o+EPR_EXTRUDER_WAIT_RETRACT_UNITS,e->waitRetractUnits);
#else
        HAL::eprSetInt16(o+EPR_EXTRUDER_WAIT_RETRACT_TEMP,EXT0_WAIT_RETRACT_TEMP);
        HAL::eprSetInt16(o+EPR_EXTRUDER_WAIT_RETRACT_UNITS,EXT0_WAIT_RETRACT_UNITS);
#endif
        HAL::eprSetByte(o+EPR_EXTRUDER_COOLER_SPEED,e->coolerSpeed);
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
        HAL::eprSetFloat(o+EPR_EXTRUDER_ADVANCE_K,e->advanceK);
#else
        HAL::eprSetFloat(o+EPR_EXTRUDER_ADVANCE_K,0);
#endif
        HAL::eprSetFloat(o+EPR_EXTRUDER_ADVANCE_L,e->advanceL);
#else
        HAL::eprSetFloat(o+EPR_EXTRUDER_ADVANCE_K,0);
        HAL::eprSetFloat(o+EPR_EXTRUDER_ADVANCE_L,0);
#endif
    }
#if MIXING_EXTRUDER
    storeMixingRatios(false);
#endif
    if(corrupted)
    {
        HAL::eprSetInt32(EPR_PRINTING_TIME,0);
        HAL::eprSetFloat(EPR_PRINTING_DISTANCE,0);
        initalizeUncached();
    }
    // Save version and build checksum
    HAL::eprSetByte(EPR_VERSION,EEPROM_PROTOCOL_VERSION);
    HAL::eprSetByte(EPR_INTEGRITY_BYTE,computeChecksum());
#endif
}
void EEPROM::initalizeUncached()
{
    HAL::eprSetFloat(EPR_Z_PROBE_HEIGHT,Z_PROBE_HEIGHT);
    HAL::eprSetFloat(EPR_Z_PROBE_SPEED,Z_PROBE_SPEED);
    HAL::eprSetFloat(EPR_Z_PROBE_XY_SPEED,Z_PROBE_XY_SPEED);
    HAL::eprSetFloat(EPR_Z_PROBE_X_OFFSET,Z_PROBE_X_OFFSET);
    HAL::eprSetFloat(EPR_Z_PROBE_Y_OFFSET,Z_PROBE_Y_OFFSET);
    HAL::eprSetFloat(EPR_Z_PROBE_X1,Z_PROBE_X1);
    HAL::eprSetFloat(EPR_Z_PROBE_Y1,Z_PROBE_Y1);
    HAL::eprSetFloat(EPR_Z_PROBE_X2,Z_PROBE_X2);
    HAL::eprSetFloat(EPR_Z_PROBE_Y2,Z_PROBE_Y2);
    HAL::eprSetFloat(EPR_Z_PROBE_X3,Z_PROBE_X3);
    HAL::eprSetFloat(EPR_Z_PROBE_Y3,Z_PROBE_Y3);
    HAL::eprSetFloat(EPR_AXISCOMP_TANXY,AXISCOMP_TANXY);
    HAL::eprSetFloat(EPR_AXISCOMP_TANYZ,AXISCOMP_TANYZ);
    HAL::eprSetFloat(EPR_AXISCOMP_TANXZ,AXISCOMP_TANXZ);
    HAL::eprSetFloat(EPR_Z_PROBE_BED_DISTANCE,Z_PROBE_BED_DISTANCE);
#if DRIVE_SYSTEM == DELTA
    HAL::eprSetFloat(EPR_DELTA_DIAGONAL_ROD_LENGTH,DELTA_DIAGONAL_ROD);
    HAL::eprSetFloat(EPR_DELTA_HORIZONTAL_RADIUS,ROD_RADIUS);
    HAL::eprSetInt16(EPR_DELTA_SEGMENTS_PER_SECOND_PRINT,DELTA_SEGMENTS_PER_SECOND_PRINT);
    HAL::eprSetInt16(EPR_DELTA_SEGMENTS_PER_SECOND_MOVE,DELTA_SEGMENTS_PER_SECOND_MOVE);
    HAL::eprSetInt16(EPR_DELTA_TOWERX_OFFSET_STEPS,DELTA_X_ENDSTOP_OFFSET_STEPS);
    HAL::eprSetInt16(EPR_DELTA_TOWERY_OFFSET_STEPS,DELTA_Y_ENDSTOP_OFFSET_STEPS);
    HAL::eprSetInt16(EPR_DELTA_TOWERZ_OFFSET_STEPS,DELTA_Z_ENDSTOP_OFFSET_STEPS);
    HAL::eprSetFloat(EPR_DELTA_ALPHA_A,DELTA_ALPHA_A);
    HAL::eprSetFloat(EPR_DELTA_ALPHA_B,DELTA_ALPHA_B);
    HAL::eprSetFloat(EPR_DELTA_ALPHA_C,DELTA_ALPHA_C);
    HAL::eprSetFloat(EPR_DELTA_RADIUS_CORR_A,DELTA_RADIUS_CORRECTION_A);
    HAL::eprSetFloat(EPR_DELTA_RADIUS_CORR_B,DELTA_RADIUS_CORRECTION_B);
    HAL::eprSetFloat(EPR_DELTA_RADIUS_CORR_C,DELTA_RADIUS_CORRECTION_C);
    HAL::eprSetFloat(EPR_DELTA_MAX_RADIUS,DELTA_MAX_RADIUS);
    HAL::eprSetFloat(EPR_DELTA_DIAGONAL_CORRECTION_A,DELTA_DIAGONAL_CORRECTION_A);
    HAL::eprSetFloat(EPR_DELTA_DIAGONAL_CORRECTION_B,DELTA_DIAGONAL_CORRECTION_B);
    HAL::eprSetFloat(EPR_DELTA_DIAGONAL_CORRECTION_C,DELTA_DIAGONAL_CORRECTION_C);
#endif
    HAL::eprSetFloat(EPR_AXISCOMP_TANXY,AXISCOMP_TANXY);
    HAL::eprSetFloat(EPR_AXISCOMP_TANYZ,AXISCOMP_TANYZ);
    HAL::eprSetFloat(EPR_AXISCOMP_TANXZ,AXISCOMP_TANXZ);
    HAL::eprSetByte(EPR_DISTORTION_CORRECTION_ENABLED,0);
}

void EEPROM::readDataFromEEPROM()
{
#if EEPROM_MODE != 0
    uint8_t version = HAL::eprGetByte(EPR_VERSION); // This is the saved version. Don't copy data not set in older versions!
    baudrate = HAL::eprGetInt32(EPR_BAUDRATE);
    maxInactiveTime = HAL::eprGetInt32(EPR_MAX_INACTIVE_TIME);
    stepperInactiveTime = HAL::eprGetInt32(EPR_STEPPER_INACTIVE_TIME);
//#define EPR_ACCELERATION_TYPE 1
    Printer::axisStepsPerMM[X_AXIS] = HAL::eprGetFloat(EPR_XAXIS_STEPS_PER_MM);
    Printer::axisStepsPerMM[Y_AXIS] = HAL::eprGetFloat(EPR_YAXIS_STEPS_PER_MM);
    Printer::axisStepsPerMM[Z_AXIS] = HAL::eprGetFloat(EPR_ZAXIS_STEPS_PER_MM);
    Printer::maxFeedrate[X_AXIS] = HAL::eprGetFloat(EPR_X_MAX_FEEDRATE);
    Printer::maxFeedrate[Y_AXIS] = HAL::eprGetFloat(EPR_Y_MAX_FEEDRATE);
    Printer::maxFeedrate[Z_AXIS] = HAL::eprGetFloat(EPR_Z_MAX_FEEDRATE);
    Printer::homingFeedrate[X_AXIS] = HAL::eprGetFloat(EPR_X_HOMING_FEEDRATE);
    Printer::homingFeedrate[Y_AXIS] = HAL::eprGetFloat(EPR_Y_HOMING_FEEDRATE);
    Printer::homingFeedrate[Z_AXIS] = HAL::eprGetFloat(EPR_Z_HOMING_FEEDRATE);
    Printer::maxJerk = HAL::eprGetFloat(EPR_MAX_JERK);
#if DRIVE_SYSTEM!=DELTA
    Printer::maxZJerk = HAL::eprGetFloat(EPR_MAX_ZJERK);
#endif
#if RAMP_ACCELERATION
    Printer::maxAccelerationMMPerSquareSecond[X_AXIS] = HAL::eprGetFloat(EPR_X_MAX_ACCEL);
    Printer::maxAccelerationMMPerSquareSecond[Y_AXIS] = HAL::eprGetFloat(EPR_Y_MAX_ACCEL);
    Printer::maxAccelerationMMPerSquareSecond[Z_AXIS] = HAL::eprGetFloat(EPR_Z_MAX_ACCEL);
    Printer::maxTravelAccelerationMMPerSquareSecond[X_AXIS] = HAL::eprGetFloat(EPR_X_MAX_TRAVEL_ACCEL);
    Printer::maxTravelAccelerationMMPerSquareSecond[Y_AXIS] = HAL::eprGetFloat(EPR_Y_MAX_TRAVEL_ACCEL);
    Printer::maxTravelAccelerationMMPerSquareSecond[Z_AXIS] = HAL::eprGetFloat(EPR_Z_MAX_TRAVEL_ACCEL);
#endif
#if HAVE_HEATED_BED
    heatedBedController.heatManager= HAL::eprGetByte(EPR_BED_HEAT_MANAGER);
#if TEMP_PID
    heatedBedController.pidDriveMax = HAL::eprGetByte(EPR_BED_DRIVE_MAX);
    heatedBedController.pidDriveMin = HAL::eprGetByte(EPR_BED_DRIVE_MIN);
    heatedBedController.pidPGain = HAL::eprGetFloat(EPR_BED_PID_PGAIN);
    heatedBedController.pidIGain = HAL::eprGetFloat(EPR_BED_PID_IGAIN);
    heatedBedController.pidDGain = HAL::eprGetFloat(EPR_BED_PID_DGAIN);
    heatedBedController.pidMax = HAL::eprGetByte(EPR_BED_PID_MAX);
#endif
#endif
    Printer::xMin = HAL::eprGetFloat(EPR_X_HOME_OFFSET);
    Printer::yMin = HAL::eprGetFloat(EPR_Y_HOME_OFFSET);
    Printer::zMin = HAL::eprGetFloat(EPR_Z_HOME_OFFSET);
    Printer::xLength = HAL::eprGetFloat(EPR_X_LENGTH);
    Printer::yLength = HAL::eprGetFloat(EPR_Y_LENGTH);
    Printer::zLength = HAL::eprGetFloat(EPR_Z_LENGTH);
#if NONLINEAR_SYSTEM
    Printer::radius0 = HAL::eprGetFloat(EPR_DELTA_HORIZONTAL_RADIUS);
#endif
#if ENABLE_BACKLASH_COMPENSATION
    Printer::backlashX = HAL::eprGetFloat(EPR_BACKLASH_X);
    Printer::backlashY = HAL::eprGetFloat(EPR_BACKLASH_Y);
    Printer::backlashZ = HAL::eprGetFloat(EPR_BACKLASH_Z);
#endif
#if FEATURE_AUTOLEVEL
    if(version>2)
    {
        float sum = 0;
        for(uint8_t i=0; i<9; i++)
            Printer::autolevelTransformation[i] = HAL::eprGetFloat(EPR_AUTOLEVEL_MATRIX + (((int)i) << 2));
        if(isnan(Printer::autolevelTransformation[0]))   // a bug caused storage of matrix at the wrong place. Read from old position instead.
        {
            for(uint8_t i=0; i<9; i++)
                Printer::autolevelTransformation[i] = HAL::eprGetFloat((EPR_AUTOLEVEL_MATRIX + (int)i) << 2);
        }
        for(uint8_t i=0; i<9; i++)
        {
            if(isnan(Printer::autolevelTransformation[i]))
                sum += 10;
            else
                sum += RMath::sqr(Printer::autolevelTransformation[i]);
        }
        if(sum < 2.7 || sum > 3.3)
            Printer::resetTransformationMatrix(false);
        Printer::setAutolevelActive(HAL::eprGetByte(EPR_AUTOLEVEL_ACTIVE));
        Console::printArrayFLN(Messages::tTransformationMatrix,Printer::autolevelTransformation,9,6);
    }
#endif
#if MIXING_EXTRUDER
    readMixingRatios();
#endif
    // now the extruder
    for(uint8_t i=0; i<NUM_EXTRUDER; i++)
    {
#if FEATURE_WATCHDOG
        HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

        int o=i*EEPROM_EXTRUDER_LENGTH+EEPROM_EXTRUDER_OFFSET;
        Extruder *e = &extruder[i];
        e->stepsPerMM = HAL::eprGetFloat(o+EPR_EXTRUDER_STEPS_PER_MM);
        e->maxFeedrate = HAL::eprGetFloat(o+EPR_EXTRUDER_MAX_FEEDRATE);
        e->maxStartFeedrate = HAL::eprGetFloat(o+EPR_EXTRUDER_MAX_START_FEEDRATE);
        e->maxAcceleration = HAL::eprGetFloat(o+EPR_EXTRUDER_MAX_ACCELERATION);
        e->tempControl.heatManager = HAL::eprGetByte(o+EPR_EXTRUDER_HEAT_MANAGER);
#if TEMP_PID
        e->tempControl.pidDriveMax = HAL::eprGetByte(o+EPR_EXTRUDER_DRIVE_MAX);
        e->tempControl.pidDriveMin = HAL::eprGetByte(o+EPR_EXTRUDER_DRIVE_MIN);
        e->tempControl.pidPGain = HAL::eprGetFloat(o+EPR_EXTRUDER_PID_PGAIN);
        e->tempControl.pidIGain = HAL::eprGetFloat(o+EPR_EXTRUDER_PID_IGAIN);
        e->tempControl.pidDGain = HAL::eprGetFloat(o+EPR_EXTRUDER_PID_DGAIN);
        e->tempControl.pidMax = HAL::eprGetByte(o+EPR_EXTRUDER_PID_MAX);
#endif
        e->xOffset = HAL::eprGetInt32(o+EPR_EXTRUDER_X_OFFSET);
        e->yOffset = HAL::eprGetInt32(o+EPR_EXTRUDER_Y_OFFSET);
        e->watchPeriod = HAL::eprGetInt16(o+EPR_EXTRUDER_WATCH_PERIOD);
#if RETRACT_DURING_HEATUP
        e->waitRetractTemperature = HAL::eprGetInt16(o+EPR_EXTRUDER_WAIT_RETRACT_TEMP);
        e->waitRetractUnits = HAL::eprGetInt16(o+EPR_EXTRUDER_WAIT_RETRACT_UNITS);
#endif
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
        e->advanceK = HAL::eprGetFloat(o+EPR_EXTRUDER_ADVANCE_K);
#endif
        e->advanceL = HAL::eprGetFloat(o+EPR_EXTRUDER_ADVANCE_L);
#endif
        if(version > 1)
            e->coolerSpeed = HAL::eprGetByte(o+EPR_EXTRUDER_COOLER_SPEED);
    }
    if(version != EEPROM_PROTOCOL_VERSION)
    {
        Console::printInfoFLN(Messages::tEPRProtocolChanged);
        if(version < 3)
        {
            HAL::eprSetFloat(EPR_Z_PROBE_HEIGHT,Z_PROBE_HEIGHT);
            HAL::eprSetFloat(EPR_Z_PROBE_SPEED,Z_PROBE_SPEED);
            HAL::eprSetFloat(EPR_Z_PROBE_XY_SPEED,Z_PROBE_XY_SPEED);
            HAL::eprSetFloat(EPR_Z_PROBE_X_OFFSET,Z_PROBE_X_OFFSET);
            HAL::eprSetFloat(EPR_Z_PROBE_Y_OFFSET,Z_PROBE_Y_OFFSET);
            HAL::eprSetFloat(EPR_Z_PROBE_X1,Z_PROBE_X1);
            HAL::eprSetFloat(EPR_Z_PROBE_Y1,Z_PROBE_Y1);
            HAL::eprSetFloat(EPR_Z_PROBE_X2,Z_PROBE_X2);
            HAL::eprSetFloat(EPR_Z_PROBE_Y2,Z_PROBE_Y2);
            HAL::eprSetFloat(EPR_Z_PROBE_X3,Z_PROBE_X3);
            HAL::eprSetFloat(EPR_Z_PROBE_Y3,Z_PROBE_Y3);
        }
        if(version < 4)
        {
#if DRIVE_SYSTEM == DELTA
            HAL::eprSetFloat(EPR_DELTA_DIAGONAL_ROD_LENGTH,DELTA_DIAGONAL_ROD);
            HAL::eprSetFloat(EPR_DELTA_HORIZONTAL_RADIUS,ROD_RADIUS);
            HAL::eprSetInt16(EPR_DELTA_SEGMENTS_PER_SECOND_PRINT,DELTA_SEGMENTS_PER_SECOND_PRINT);
            HAL::eprSetInt16(EPR_DELTA_SEGMENTS_PER_SECOND_MOVE,DELTA_SEGMENTS_PER_SECOND_MOVE);
            HAL::eprSetInt16(EPR_DELTA_TOWERX_OFFSET_STEPS,DELTA_X_ENDSTOP_OFFSET_STEPS);
            HAL::eprSetInt16(EPR_DELTA_TOWERY_OFFSET_STEPS,DELTA_Y_ENDSTOP_OFFSET_STEPS);
            HAL::eprSetInt16(EPR_DELTA_TOWERZ_OFFSET_STEPS,DELTA_Z_ENDSTOP_OFFSET_STEPS);
#endif
        }
#if DRIVE_SYSTEM == DELTA
        if(version < 5)
        {
            HAL::eprSetFloat(EPR_DELTA_ALPHA_A,DELTA_ALPHA_A);
            HAL::eprSetFloat(EPR_DELTA_ALPHA_B,DELTA_ALPHA_B);
            HAL::eprSetFloat(EPR_DELTA_ALPHA_C,DELTA_ALPHA_C);
        }
        if(version < 6)
        {
            HAL::eprSetFloat(EPR_DELTA_RADIUS_CORR_A,DELTA_RADIUS_CORRECTION_A);
            HAL::eprSetFloat(EPR_DELTA_RADIUS_CORR_B,DELTA_RADIUS_CORRECTION_B);
            HAL::eprSetFloat(EPR_DELTA_RADIUS_CORR_C,DELTA_RADIUS_CORRECTION_C);
        }
        if(version < 7)
        {
            HAL::eprSetFloat(EPR_DELTA_MAX_RADIUS,DELTA_MAX_RADIUS);
            HAL::eprSetFloat(EPR_DELTA_DIAGONAL_CORRECTION_A,DELTA_DIAGONAL_CORRECTION_A);
            HAL::eprSetFloat(EPR_DELTA_DIAGONAL_CORRECTION_B,DELTA_DIAGONAL_CORRECTION_B);
            HAL::eprSetFloat(EPR_DELTA_DIAGONAL_CORRECTION_C,DELTA_DIAGONAL_CORRECTION_C);
        }
#endif
        if(version < 8)
        {
            HAL::eprSetFloat(EPR_Z_PROBE_BED_DISTANCE,Z_PROBE_BED_DISTANCE);
        }
        if(version < 9)
        {
#if MIXING_EXTRUDER
            storeMixingRatios(false);
#endif
        }
        if(version < 10) {
            HAL::eprSetFloat(EPR_AXISCOMP_TANXY,AXISCOMP_TANXY);
            HAL::eprSetFloat(EPR_AXISCOMP_TANYZ,AXISCOMP_TANYZ);
            HAL::eprSetFloat(EPR_AXISCOMP_TANXZ,AXISCOMP_TANXZ);
        }
        if(version < 11) {
            HAL::eprSetByte(EPR_DISTORTION_CORRECTION_ENABLED, 0);
        }
        /*        if (version<8) {
        #if DRIVE_SYSTEM==DELTA
                  // Prior to verion 8, the cartesian max was stored in the zmax
                  // Now, x,y and z max are used for tower a, b anc c
                  // Of tower min are all set at 0, tower max is larger than cartesian max
                  // by the height of any tower for coordinate 0,0,0
                  long cart[Z_AXIS_ARRAY], delta[TOWER_ARRAY];
                  cart[X_AXIS] = cart[Y_AXIS] = cart[Z_AXIS] = 0;
                  transformCartesianStepsToDeltaSteps(cart, delta);
                  // We can only count on ZLENGTH being set correctly, as it was used for all towers
                  Printer::xLength = Printer::zLength + delta[X_AXIS];
                  Printer::yLength = Printer::zLength + delta[Y_AXIS];
                  Printer::zLength += delta[Z_AXIS];
        #endif
                }*/

        storeDataIntoEEPROM(false); // Store new fields for changed version
    }
    Printer::updateDerivedParameter();
    Extruder::initHeatedBed();
#endif
}

void EEPROM::initBaudrate()
{
    // Invariant - baudrate is intitalized with or without eeprom!
    baudrate = REP_BAUDRATE;
#if EEPROM_MODE != 0
    if(HAL::eprGetByte(EPR_MAGIC_BYTE)==EEPROM_MODE)
    {
        baudrate = HAL::eprGetInt32(EPR_BAUDRATE);
    }
#endif
}

#ifndef USE_CONFIGURATION_BAUD_RATE
#define USE_CONFIGURATION_BAUD_RATE 0
#endif // USE_CONFIGURATION_BAUD_RATE
void EEPROM::init()
{
#if EEPROM_MODE != 0
    uint8_t check = computeChecksum();
    uint8_t storedcheck = HAL::eprGetByte(EPR_INTEGRITY_BYTE);
    if(HAL::eprGetByte(EPR_MAGIC_BYTE) == EEPROM_MODE && storedcheck == check)
    {
        readDataFromEEPROM();
        if (USE_CONFIGURATION_BAUD_RATE)
        {
            // Used if eeprom gets unusable baud rate set and communication wont work at all.
            if(HAL::eprGetInt32(EPR_BAUDRATE) != REP_BAUDRATE)
            {
                HAL::eprSetInt32(EPR_BAUDRATE,REP_BAUDRATE);
                baudrate = REP_BAUDRATE;
                uint8_t newcheck = computeChecksum();
                if(newcheck != HAL::eprGetByte(EPR_INTEGRITY_BYTE))
                    HAL::eprSetByte(EPR_INTEGRITY_BYTE,newcheck);
            }
            Console::printFLN(PSTR("EEprom baud rate restored from configuration."));
            Console::printFLN(PSTR("RECOMPILE WITH USE_CONFIGURATION_BAUD_RATE == 0 to alter baud rate via EEPROM"));
        }
    }
    else
    {
        HAL::eprSetByte(EPR_MAGIC_BYTE,EEPROM_MODE); // Make datachange permanent
        initalizeUncached();
        storeDataIntoEEPROM(storedcheck != check);
    }
#endif
}

void EEPROM::updatePrinterUsage()
{
#if EEPROM_MODE != 0
    if(Printer::filamentPrinted == 0) return; // No miles only enabled
    uint32_t seconds = (HAL::timeInMilliseconds() - Printer::msecondsPrinting) / 1000;
    seconds += HAL::eprGetInt32(EPR_PRINTING_TIME);
    HAL::eprSetInt32(EPR_PRINTING_TIME,seconds);
    HAL::eprSetFloat(EPR_PRINTING_DISTANCE,HAL::eprGetFloat(EPR_PRINTING_DISTANCE) + Printer::filamentPrinted * 0.001);
    Printer::filamentPrinted = 0;
    Printer::msecondsPrinting = HAL::timeInMilliseconds();
    updateChecksum();
    Commands::reportPrinterUsage();
#endif
}

/** \brief Writes all eeprom settings to serial console.

For each value stored, this function generates one line with syntax

EPR: pos type value description

With
- pos = Position in EEPROM, the data starts.
- type = Value type: 0 = byte, 1 = int, 2 = long, 3 = float
- value = The value currently stored
- description = Definition of the value
*/
void EEPROM::writeSettings()
{
#if EEPROM_MODE != 0
    writeLong(EPR_BAUDRATE, Messages::tEPRBaudrate);
    writeFloat(EPR_PRINTING_DISTANCE, Messages::tEPRFilamentPrinted);
    writeLong(EPR_PRINTING_TIME, Messages::tEPRPrinterActive);
    writeLong(EPR_MAX_INACTIVE_TIME, Messages::tEPRMaxInactiveTime);
    writeLong(EPR_STEPPER_INACTIVE_TIME, Messages::tEPRStopAfterInactivty);
//#define EPR_ACCELERATION_TYPE 1
#if DRIVE_SYSTEM != DELTA
    writeFloat(EPR_XAXIS_STEPS_PER_MM, Messages::tEPRXStepsPerMM, 4);
    writeFloat(EPR_YAXIS_STEPS_PER_MM, Messages::tEPRYStepsPerMM, 4);
#endif
    writeFloat(EPR_ZAXIS_STEPS_PER_MM, Messages::tEPRZStepsPerMM, 4);
#if DRIVE_SYSTEM != DELTA
    writeFloat(EPR_X_MAX_FEEDRATE, Messages::tEPRXMaxFeedrate);
    writeFloat(EPR_Y_MAX_FEEDRATE, Messages::tEPRYMaxFeedrate);
#endif
    writeFloat(EPR_Z_MAX_FEEDRATE, Messages::tEPRZMaxFeedrate);
#if DRIVE_SYSTEM != DELTA
    writeFloat(EPR_X_HOMING_FEEDRATE, Messages::tEPRXHomingFeedrate);
    writeFloat(EPR_Y_HOMING_FEEDRATE, Messages::tEPRYHomingFeedrate);
#endif
    writeFloat(EPR_Z_HOMING_FEEDRATE, Messages::tEPRZHomingFeedrate);
    writeFloat(EPR_MAX_JERK, Messages::tEPRMaxJerk);
#if DRIVE_SYSTEM != DELTA
    writeFloat(EPR_MAX_ZJERK, Messages::tEPRMaxZJerk);
#endif
    writeFloat(EPR_X_HOME_OFFSET, Messages::tEPRXHomePos);
    writeFloat(EPR_Y_HOME_OFFSET, Messages::tEPRYHomePos);
    writeFloat(EPR_Z_HOME_OFFSET, Messages::tEPRZHomePos);
    writeFloat(EPR_X_LENGTH, Messages::tEPRXMaxLength);
    writeFloat(EPR_Y_LENGTH, Messages::tEPRYMaxLength);
    writeFloat(EPR_Z_LENGTH, Messages::tEPRZMaxLength);
#if ENABLE_BACKLASH_COMPENSATION
    writeFloat(EPR_BACKLASH_X, Messages::tEPRXBacklash);
    writeFloat(EPR_BACKLASH_Y, Messages::tEPRYBacklash);
    writeFloat(EPR_BACKLASH_Z, Messages::tEPRZBacklash);
#endif

#if RAMP_ACCELERATION
    //epr_out_float(EPR_X_MAX_START_SPEED,PSTR("X-axis start speed [mm/s]"));
    //epr_out_float(EPR_Y_MAX_START_SPEED,PSTR("Y-axis start speed [mm/s]"));
    //epr_out_float(EPR_Z_MAX_START_SPEED,PSTR("Z-axis start speed [mm/s]"));
#if DRIVE_SYSTEM == TUGA
    writeFloat(EPR_DELTA_DIAGONAL_ROD_LENGTH, Messages::tEPRDiagonalRodLength);
#endif
#if DRIVE_SYSTEM == DELTA
    writeFloat(EPR_Z_MAX_ACCEL, Messages::tEPRZAcceleration);
    writeFloat(EPR_Z_MAX_TRAVEL_ACCEL, Messages::tEPRZTravelAcceleration);
    writeFloat(EPR_DELTA_DIAGONAL_ROD_LENGTH, Messages::tEPRDiagonalRodLength);
    writeFloat(EPR_DELTA_HORIZONTAL_RADIUS, Messages::tEPRHorizontalRadius);
    writeFloat(EPR_DELTA_MAX_RADIUS, Messages::tEPRDeltaMaxRadius);
    writeInt(EPR_DELTA_SEGMENTS_PER_SECOND_MOVE, Messages::tEPRSegmentsPerSecondTravel);
    writeInt(EPR_DELTA_SEGMENTS_PER_SECOND_PRINT, Messages::tEPRSegmentsPerSecondPrint);
    writeInt(EPR_DELTA_TOWERX_OFFSET_STEPS, Messages::tEPRTowerXOffset);
    writeInt(EPR_DELTA_TOWERY_OFFSET_STEPS, Messages::tEPRTowerYOffset);
    writeInt(EPR_DELTA_TOWERZ_OFFSET_STEPS, Messages::tEPRTowerZOffset);
    writeFloat(EPR_DELTA_ALPHA_A, Messages::tDeltaAlphaA);
    writeFloat(EPR_DELTA_ALPHA_B, Messages::tDeltaAlphaB);
    writeFloat(EPR_DELTA_ALPHA_C, Messages::tDeltaAlphaC);
    writeFloat(EPR_DELTA_RADIUS_CORR_A, Messages::tDeltaRadiusCorrectionA);
    writeFloat(EPR_DELTA_RADIUS_CORR_B, Messages::tDeltaRadiusCorrectionB);
    writeFloat(EPR_DELTA_RADIUS_CORR_C, Messages::tDeltaRadiusCorrectionC);
    writeFloat(EPR_DELTA_DIAGONAL_CORRECTION_A, Messages::tDeltaDiagonalCorrectionA);
    writeFloat(EPR_DELTA_DIAGONAL_CORRECTION_B, Messages::tDeltaDiagonalCorrectionB);
    writeFloat(EPR_DELTA_DIAGONAL_CORRECTION_C, Messages::tDeltaDiagonalCorrectionC);
#else
    writeFloat(EPR_X_MAX_ACCEL, Messages::tEPRXAcceleration);
    writeFloat(EPR_Y_MAX_ACCEL, Messages::tEPRYAcceleration);
    writeFloat(EPR_Z_MAX_ACCEL, Messages::tEPRZAcceleration);
    writeFloat(EPR_X_MAX_TRAVEL_ACCEL, Messages::tEPRXTravelAcceleration);
    writeFloat(EPR_Y_MAX_TRAVEL_ACCEL, Messages::tEPRYTravelAcceleration);
    writeFloat(EPR_Z_MAX_TRAVEL_ACCEL, Messages::tEPRZTravelAcceleration);
#endif
#endif
#if FEATURE_Z_PROBE
    writeFloat(EPR_Z_PROBE_HEIGHT, Messages::tZProbeHeight);
    writeFloat(EPR_Z_PROBE_BED_DISTANCE, Messages::tZProbeBedDitance);
    writeFloat(EPR_Z_PROBE_SPEED, Messages::tZProbeSpeed);
    writeFloat(EPR_Z_PROBE_XY_SPEED, Messages::tZProbeSpeedXY);
    writeFloat(EPR_Z_PROBE_X_OFFSET, Messages::tZProbeOffsetX);
    writeFloat(EPR_Z_PROBE_Y_OFFSET, Messages::tZProbeOffsetY);
    writeFloat(EPR_Z_PROBE_X1, Messages::tZProbeX1);
    writeFloat(EPR_Z_PROBE_Y1, Messages::tZProbeY1);
    writeFloat(EPR_Z_PROBE_X2, Messages::tZProbeX2);
    writeFloat(EPR_Z_PROBE_Y2, Messages::tZProbeY2);
    writeFloat(EPR_Z_PROBE_X3, Messages::tZProbeX3);
    writeFloat(EPR_Z_PROBE_Y3, Messages::tZProbeY3);
#endif
#if FEATURE_AUTOLEVEL
    writeByte(EPR_AUTOLEVEL_ACTIVE, Messages::tAutolevelActive);
#endif

#if FEATURE_AXISCOMP
    writeFloat(EPR_AXISCOMP_TANXY, Messages::tAxisCompTanXY);
    writeFloat(EPR_AXISCOMP_TANYZ, Messages::tAxisCompTanYZ);
    writeFloat(EPR_AXISCOMP_TANXZ, Messages::tAxisCompTanXZ);
#endif


#if HAVE_HEATED_BED
    writeByte(EPR_BED_HEAT_MANAGER, Messages::tEPRBedHeatManager);
#if TEMP_PID
    writeByte(EPR_BED_DRIVE_MAX, Messages::tEPRBedPIDDriveMax);
    writeByte(EPR_BED_DRIVE_MIN, Messages::tEPRBedPIDDriveMin);
    writeFloat(EPR_BED_PID_PGAIN, Messages::tEPRBedPGain);
    writeFloat(EPR_BED_PID_IGAIN, Messages::tEPRBedIGain);
    writeFloat(EPR_BED_PID_DGAIN, Messages::tEPRBedDGain);
    writeByte(EPR_BED_PID_MAX, Messages::tEPRBedPISMaxValue);
#endif
#endif
    // now the extruder
    for(uint8_t i = 0; i < NUM_EXTRUDER; i++)
    {
        int o = i * EEPROM_EXTRUDER_LENGTH + EEPROM_EXTRUDER_OFFSET;
        writeFloat(o + EPR_EXTRUDER_STEPS_PER_MM, Messages::tEPRStepsPerMM);
        writeFloat(o + EPR_EXTRUDER_MAX_FEEDRATE, Messages::tEPRMaxFeedrate);
        writeFloat(o + EPR_EXTRUDER_MAX_START_FEEDRATE, Messages::tEPRStartFeedrate);
        writeFloat(o + EPR_EXTRUDER_MAX_ACCELERATION, Messages::tEPRAcceleration);
        writeByte(o + EPR_EXTRUDER_HEAT_MANAGER, Messages::tEPRHeatManager);
#if TEMP_PID
        writeByte(o + EPR_EXTRUDER_DRIVE_MAX, Messages::tEPRDriveMax);
        writeByte(o + EPR_EXTRUDER_DRIVE_MIN, Messages::tEPRDriveMin);
        writeFloat(o + EPR_EXTRUDER_PID_PGAIN, Messages::tEPRPGain,4);
        writeFloat(o + EPR_EXTRUDER_PID_IGAIN, Messages::tEPRIGain,4);
        writeFloat(o + EPR_EXTRUDER_PID_DGAIN, Messages::tEPRDGain,4);
        writeByte(o + EPR_EXTRUDER_PID_MAX, Messages::tEPRPIDMaxValue);
#endif
        writeLong(o + EPR_EXTRUDER_X_OFFSET, Messages::tEPRXOffset);
        writeLong(o + EPR_EXTRUDER_Y_OFFSET, Messages::tEPRYOffset);
        writeInt(o + EPR_EXTRUDER_WATCH_PERIOD, Messages::tEPRStabilizeTime);
#if RETRACT_DURING_HEATUP
        writeInt(o + EPR_EXTRUDER_WAIT_RETRACT_TEMP, Messages::tEPRRetractionWhenHeating);
        writeInt(o + EPR_EXTRUDER_WAIT_RETRACT_UNITS, Messages::tEPRDistanceRetractHeating);
#endif
        writeByte(o + EPR_EXTRUDER_COOLER_SPEED, Messages::tEPRExtruderCoolerSpeed);
#if USE_ADVANCE
#if ENABLE_QUADRATIC_ADVANCE
        writeFloat(o + EPR_EXTRUDER_ADVANCE_K, Messages::tEPRAdvanceK);
#endif
        writeFloat(o + EPR_EXTRUDER_ADVANCE_L, Messages::tEPRAdvanceL);
#endif
#if MIXING_EXTRUDER
        for(uint8_t v = 0; v < VIRTUAL_EXTRUDER; v++)
        {
            uint16_t pos = o + EPR_EXTRUDER_MIXING_RATIOS + 2 * v;
            Console::printF(Messages::tEPR1,(int)pos);
            Console::print(' ');
            Console::print(HAL::eprGetInt16(pos));
            Console::print(' ');
            writeExtruderPrefix(pos);
            Console::printFLN(PSTR("Weight "), (int)(v + 1));
        }
#endif
    }
#else
    Console::printErrorF(Messages::tNoEEPROMSupport);
#endif
}

#if EEPROM_MODE != 0

uint8_t EEPROM::computeChecksum()
{
    unsigned int i;
    uint8_t checksum = 0;
    for(i = 0; i < 2048; i++)
    {
        if(i == EEPROM_OFFSET + EPR_INTEGRITY_BYTE) continue;
        checksum += HAL::eprGetByte(i);
    }
    return checksum;
}

void EEPROM::updateChecksum()
{
    uint8_t newcheck = computeChecksum();
    if(newcheck!=HAL::eprGetByte(EPR_INTEGRITY_BYTE))
        HAL::eprSetByte(EPR_INTEGRITY_BYTE,newcheck);
}

void EEPROM::writeExtruderPrefix(uint pos)
{
    if(pos < EEPROM_EXTRUDER_OFFSET || pos >= 800) return;
    int n = (pos - EEPROM_EXTRUDER_OFFSET) / EEPROM_EXTRUDER_LENGTH + 1;
    Console::printF(Messages::tExtrDot, n);
    Console::print(' ');
}

void EEPROM::writeFloat(uint pos,PGM_P text,uint8_t digits)
{
    Console::printF(Messages::tEPR3, static_cast<int>(pos));
    Console::print(' ');
    Console::printFloat(HAL::eprGetFloat(pos),digits);
    Console::print(' ');
    writeExtruderPrefix(pos);
    Console::printFLN(text);
}

void EEPROM::writeLong(uint pos,PGM_P text)
{
    Console::printF(Messages::tEPR2, static_cast<int>(pos));
    Console::print(' ');
    Console::print(HAL::eprGetInt32(pos));
    Console::print(' ');
    writeExtruderPrefix(pos);
    Console::printFLN(text);
}

void EEPROM::writeInt(uint pos,PGM_P text)
{
    Console::printF(Messages::tEPR1, static_cast<int>(pos));
    Console::print(' ');
    Console::print(HAL::eprGetInt16(pos));
    Console::print(' ');
    writeExtruderPrefix(pos);
    Console::printFLN(text);
}

void EEPROM::writeByte(uint pos,PGM_P text)
{
    Console::printF(Messages::tEPR0, static_cast<int>(pos));
    Console::print(' ');
    Console::print((int)HAL::eprGetByte(pos));
    Console::print(' ');
    writeExtruderPrefix(pos);
    Console::printFLN(text);
}

#if MIXING_EXTRUDER
void EEPROM::storeMixingRatios(bool _updateChecksum)
{
    for(uint8_t e = 0; e < NUM_EXTRUDER; e++)
    {
        for(uint8_t i = 0; i < VIRTUAL_EXTRUDER; i++)
        {
            HAL::eprSetInt16(EEPROM_EXTRUDER_OFFSET + e * EEPROM_EXTRUDER_LENGTH + EPR_EXTRUDER_MIXING_RATIOS + 2 * i, extruder[e].virtualWeights[i]);
        }
    }
    if(_updateChecksum)
        updateChecksum();
}

void EEPROM::readMixingRatios()
{
    for(uint8_t e = 0; e < NUM_EXTRUDER; e++)
    {
        for(uint8_t i = 0; i < VIRTUAL_EXTRUDER; i++)
        {
            extruder[e].virtualWeights[i] = HAL::eprGetInt16(EEPROM_EXTRUDER_OFFSET + e * EEPROM_EXTRUDER_LENGTH + EPR_EXTRUDER_MIXING_RATIOS + 2 * i);
        }
    }
}

void EEPROM::restoreMixingRatios()
{
    for(uint8_t e = 0; e < NUM_EXTRUDER; e++)
    {
        for(uint8_t i = 0; i < VIRTUAL_EXTRUDER; i++)
        {
            extruder[e].virtualWeights[i] = 10;
        }
    }
}

#endif

void EEPROM::setZCorrection(int32_t c,int index) {
    HAL::eprSetInt32(2048 + (index << 2), c);
}

#endif

