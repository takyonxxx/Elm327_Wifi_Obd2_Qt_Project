#ifndef PID_H
#define PID_H
#include <QtCore>
#include <QScreen>

/* Details from http://en.wikipedia.org/wiki/OBD-II_PIDs */
/*
    0101-show current data
    0102-show freeze frame data
    0103-show stored diagnostic trouble code.
    0104-Calculate load value
    0105-Coolant temperatureEngine Coolant Temperature
    010A-Fuel pressure
    010B-Absolute inlet pressureIntake Manifold Absolute Pressure
    010C-engine speedEngine RPM
    010D-Vehicle speedVehicle Speed
    010E-Ignition timingIgnition Timing Advance
    010F-Intake temperatureIntake Air Temperature
    0110-Air flowAir Flow Rate
    0111-Throttle positionAbsolute Throttle Position
    011F-Engine running timeTime Since Engine Start
    0121-Mileage after fault lightDistance, Travelled, While, MIL, Is, Activated
    0122-The fuel guide rail is relative to the manifold vacuum pressureFuel, Rail, Pressure, relative, to, manifold, vacuum
    0123-Fuel guide pressure
    012E-Instruction fuel vapor removal
    012F-fuel level
    0130-The number of empty car fault code
    0131-Travel mileage after fault code is empty
    0132-EVAPVapour pressure
    0133-pressure
    013C-Catalyst temperatureBank1Sensors1
    013D--Catalyst temperatureBank2Sensors1
    013E--Catalyst temperatureBank1Sensors2
    013F--Catalyst temperatureBank2Sensors2
    0142-Control module voltage
    0143-Absolute load value
    0144-Command equivalence ratio
    0145-Relative position of throttle
    0146-ambient temperature
    0147-Throttle absolute positionB
    0148-Throttle absolute positionC
    0149-Accelerator pedal positionD
    014A-Accelerator pedal positionE
    014B-Accelerator pedal positionF
    014C-Throttle ratio of instructions
    014D-MILHow many minutes does the engine run after the light is on?
    014E-Run time after the fault code code is empty
    0153-EVAPAbsolute steam pressure
    0154-EVAPSystem steam pressure
    0159-Fuel guide absolute pressure
*/

template <typename T>
typename std::enable_if<std::is_unsigned<T>::value, int>::type
inline constexpr signum(T x) {
    return T(0) < x;
}

template <typename T>
typename std::enable_if<std::is_signed<T>::value, int>::type
inline constexpr signum(T x) {
    return (T(0) < x) - (x < T(0));
}

static QString DEFAULT = "ATD",
RESET = "ATZ",
SOFTRESET = "ATWS",
INFO = "ATI",
VOLTAGE = "ATRV",
GET_PROTOCOL = "ATDP",
PROTOCOL_AUTO1 = "ATSP0",
PROTOCOL_AUTO2 = "ATSPA0",
PROTOCOL_SEARCH_ORDER= "ATSS",
ECHO_OFF = "ATE0",
ECHO_ON = "ATE1",
LINEFEED_OFF = "ATL0",
LINEFEED_ON = "ATL1",
HEADERS_OFF = "ATH0",
HEADERS_ON = "ATH1",
SPACES_OFF = "ATS0",
SPACES_ON = "ATS1",
ADAPTIF_TIMING_OFF = "ATAT0",
ADAPTIF_TIMING_AUTO1 = "ATAT1",
ADAPTIF_TIMING_AUTO2 = "ATAT2",
TIMEOUT_01 = "ATST01",
TERMINATE_SESSION = "ATPC",
CHECK_DATA = "0100",
ENGINE_COOLANT_TEMP = "0105",  //A-40
ENGINE_RPM = "010C",  //((A*256)+B)/4
ENGINE_LOAD = "0104",  // A*100/255
VEHICLE_SPEED = "010D",  //A
INTAKE_AIR_TEMP = "010F",  //A-40
MAN_ABSOLUTE_PRESSURE = "010B", // A  Manifold Absolute Pressure
MAF_AIR_FLOW = "0110", //MAF air flow rate 0 - 655.35	grams/sec ((256*A)+B) / 100  [g/s]
ENGINE_OIL_TEMP = "015C",  //A-40
FUEL_PRESSURE = "010A",  // A*3
FUEL_RAIL_LOW_PRESSURE = "0122", // ((A*256)+B)*0.079
FUEL_RAIL_HIGH_PRESSURE = "0123", // ((A*256)+B) * 10
INTAKE_MAN_PRESSURE = "010B", //Intake manifold absolute pressure 0 - 255 kPa
CONT_MODULE_VOLT = "0142",  //((A*256)+B)/1000
AMBIENT_AIR_TEMP = "0146",  //A-40
CATALYST_TEMP_B1S1 = "013C",  //(((A*256)+B)/10)-40
STATUS_DTC = "0101", //Status since DTC Cleared
THROTTLE_POSITION = "0111", //Throttle position 0 -100 % A*100/255
OBD_STANDARDS = "011C", //OBD standards this vehicle
PIDS_SUPPORTED = "0120", //PIDs supported
FUEL_RATE = "015E", // (A*256 + B) / 20  -->L/h
REQUEST_TROUBLE = "03", //Request trouble codes
CLEAR_TROUBLE = "04"; //Clear trouble codes / Malfunction indicator lamp (MIL) / Check engine light

static QStringList initializeCommands{TERMINATE_SESSION, SPACES_OFF, ECHO_OFF, LINEFEED_OFF,
            HEADERS_OFF, ADAPTIF_TIMING_AUTO1, TIMEOUT_01,
            PROTOCOL_SEARCH_ORDER, PROTOCOL_AUTO1, PROTOCOL_AUTO2, GET_PROTOCOL, INFO};

static QStringList runtimeCommands{VOLTAGE, ENGINE_RPM, ENGINE_LOAD, VEHICLE_SPEED, ENGINE_COOLANT_TEMP,
            INTAKE_AIR_TEMP, MAF_AIR_FLOW, MAN_ABSOLUTE_PRESSURE, FUEL_RAIL_HIGH_PRESSURE, FUEL_RATE};

static QStringList gaugeCommands{ENGINE_RPM, VEHICLE_SPEED};

static QStringList PIDS {
    "01", "02", "03", "04", "05", "06", "07", "08",
    "09", "0A", "0B", "0C", "0D", "0E", "0F", "10",
    "11", "12", "13", "14", "15", "16", "17", "18",
    "19", "1A", "1B", "1C", "1D", "1E", "1F", "20"};


#endif // PID_H
