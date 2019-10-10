# Elm327 WiFi Obd2 Console
<b>The ELM327</b> is a programmed microcontroller produced by ELM Electronics for translating the on-board diagnostics (OBD) interface found in most modern cars. The ELM327 command protocol is one of the most popular PC-to-OBD interface standards and is also implemented by other vendors.</br>

The original ELM327 is implemented on the PIC18F2480 microcontroller from Microchip Technology.</br>

ELM327 is one of a family of OBD translators from ELM Electronics. Other variants implement only a subset of the OBD protocols.</br>

<b>ELM327 AT COMMANDS</b></br>

Service 03 : Read diagnostic trouble codes, both generic and manufacturer-specific, and display their meaning.</br>
Service 04 : Clear trouble codes and turn off the MIL</br>
<b>Service 01 : Display current sensor data</b></br>
PID Description  Renault / Dacia</br>
00 PIDs supported [01 - 20]</br>
01 Monitor status since DTCs cleared. </br>
05 Engine coolant temperature</br>
0B Intake manifold absolute pressure</br>
0C Engine RPM</br>
0D Vehicle speed</br>
0F Intake air temperature</br>
10 MAF air flow rate</br>
11 Throttle position </br>
1C OBD standards this vehicle conforms to</br>
20 PIDs supported [21 - 40]</br>
21 Distance travelled with malfunction indicator lamp (MIL) on (yes)</br>
23 Fuel rail Pressure (diesel, or gasoline direct inject)</br>
31 Distance traveled since codes cleared </br>
42 Control module voltage </br>
4D Time run with MIL on (yes) </br>
4E Time since trouble codes cleared </br>
50 Maximum value for air flow rate from mass air flow sensor </br>
61 Driver's demand engine - percent torque </br>
62 Actual engine - percent torque </br>

<p align="center"><a href="https://github.com/takyonxxx/Elm327_Obd2_Qt_Project/blob/master/elm327.jpg">
		<img src="https://github.com/takyonxxx/Elm327_Obd2_Qt_Project/blob/master/elm327.jpg" 
		name="Image3" align="bottom" width="480" height="800" border="1"></a></p>

