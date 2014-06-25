// This file is a part of the OpenSurgSim project.
// Copyright 2013, SimQuest Solutions Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SURGSIM_DEVICES_LABJACK_LABJACKDEVICE_H
#define SURGSIM_DEVICES_LABJACK_LABJACKDEVICE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "SurgSim/Input/CommonDevice.h"

namespace SurgSim
{
namespace Device
{
class LabJackScaffold;

/// The types of LabJack devices.  Numbers come from LabJackUD.h.
enum LabJackType
{
	LABJACKTYPE_SEARCH = -1,
	LABJACKTYPE_UE9 = 9,
	LABJACKTYPE_U3 = 3,
	LABJACKTYPE_U6 = 6
};

/// The connection (i.e., communication media) for LabJacks.  Numbers come from LabJackUD.h.
enum LabJackConnection
{
	LABJACKCONNECTION_SEARCH = -1,
	LABJACKCONNECTION_USB = 1,
	LABJACKCONNECTION_ETHERNET = 2,
	LABJACKCONNECTION_ETHERNET_MB = 3,
	LABJACKCONNECTION_ETHERNET_DATA_ONLY = 4
};

/// The timer base frequencies for LabJacks.  A given value can correspond to different clock frequencies for different
/// LabJack models.  The same clock frequency corresponds to different values depending on whether the
/// high- or low-level driver is used.  See section 2.10 - Timers/Counters in the respective model's User's Guide.
enum LabJackTimerBase
{
	LABJACKTIMERBASE_DEFAULT = -1,
	LABJACKTIMERBASE_0 = 0,
	LABJACKTIMERBASE_1 = 1,
	LABJACKTIMERBASE_2 = 2,
	LABJACKTIMERBASE_3 = 3,
	LABJACKTIMERBASE_4 = 4,
	LABJACKTIMERBASE_5 = 5,
	LABJACKTIMERBASE_6 = 6,
	LABJACKTIMERBASE_20 = 20,
	LABJACKTIMERBASE_21 = 21,
	LABJACKTIMERBASE_22 = 22,
	LABJACKTIMERBASE_23 = 23,
	LABJACKTIMERBASE_24 = 24,
	LABJACKTIMERBASE_25 = 25,
	LABJACKTIMERBASE_26 = 26
};

/// The timer modes.  Numbers come from LabJackUD.h.  Note that edge-counting modes require processing time: see the
/// LabJack manual for restrictions on number of edges counted per second over all timers
/// (e.g., 30,000/second for U3 or U6).
enum LabJackTimerMode
{
	LABJACKTIMERMODE_PWM16 = 0, // 16 bit PWM
	LABJACKTIMERMODE_PWM8 = 1, // 8 bit PWM
	LABJACKTIMERMODE_RISINGEDGES32 = 2, // 32-bit rising to rising edge measurement
	LABJACKTIMERMODE_FALLINGEDGES32 = 3, // 32-bit falling to falling edge measurement
	LABJACKTIMERMODE_DUTYCYCLE = 4, // duty cycle measurement
	LABJACKTIMERMODE_FIRMCOUNTER = 5, // firmware based rising edge counter
	LABJACKTIMERMODE_FIRMCOUNTERDEBOUNCE = 6, // firmware counter with debounce
	LABJACKTIMERMODE_FREQOUT = 7, // frequency output
	LABJACKTIMERMODE_QUAD = 8, // Quadrature
	LABJACKTIMERMODE_TIMERSTOP = 9, // stops another timer after n pulses
	LABJACKTIMERMODE_SYSTIMERLOW = 10, // read lower 32-bits of system timer
	LABJACKTIMERMODE_SYSTIMERHIGH = 11, // read upper 32-bits of system timer
	LABJACKTIMERMODE_RISINGEDGES16 = 12, // 16-bit rising to rising edge measurement
	LABJACKTIMERMODE_FALLINGEDGES16 = 13, // 16-bit falling to falling edge measurement
	LABJACKTIMERMODE_LINETOLINE = 14 // Line to Line measurement
};

/// The analog input ranges.  Equivalent to gain.  Ignored for Linux scaffold, which auto-ranges.
enum LabJackAnalogInputRange
{
	LABJACKANALOGINPUTRANGE_20 = 1, // -20V to +20V, LJ_rgBIP20V
	LABJACKANALOGINPUTRANGE_10 = 2, // -10V to +10V, LJ_rgBIP10V
	LABJACKANALOGINPUTRANGE_5 = 3, // -5V to +5V, LJ_rgBIP5V
	LABJACKANALOGINPUTRANGE_4 = 4, // -4V to +4V, LJ_rgBIP4V
	LABJACKANALOGINPUTRANGE_2p5 = 5, // -2.5V to +2.5V, LJ_rgBIP2P5V
	LABJACKANALOGINPUTRANGE_2 = 6, // -2V to +2V, LJ_rgBIP2V
	LABJACKANALOGINPUTRANGE_1p25 = 7, // -1.25V to +1.25V, LJ_rgBIP1P25V
	LABJACKANALOGINPUTRANGE_1 = 8, // -1V to +1V, LJ_rgBIP1V
	LABJACKANALOGINPUTRANGE_0p625 = 9, // -0.625V to +0.625V, LJ_rgBIPP625V
	LABJACKANALOGINPUTRANGE_0p1 = 10, // -0.1V to +0.1V, LJ_rgBIPP1V
	LABJACKANALOGINPUTRANGE_0p01 = 11 // -0.01V to +0.01V, LJ_rgBIPP01V
};

/// A struct holding the data to be associated with the positive channel for a differential analog input.
struct LabJackAnalogInputsDifferentialData
{
	/// Equality comparison.
	/// \param other The object with which to compare.
	/// \return true if equivalent.
	bool operator==(const LabJackAnalogInputsDifferentialData& other) const
	{
		return (negativeChannel == other.negativeChannel) && (range == other.range);
	}

	/// The negative channel.
	int negativeChannel;

	/// The range.
	LabJackAnalogInputRange range;
};

/// A class implementing the communication with a LabJack data acquisition (DAQ) device.  Should work for the U3, U6,
/// and U9 models on Windows and the U3 and U6 on Linux. See the manual(s) for your LabJack device(s) to understand
/// the input and output data, the configuration parameters, timing limitations, etc. The various parameters and
/// inputs are almost always passed through unchanged to the device driver. Currently timers, digital
/// input/output, and analog input/output are supported. Counters are not yet supported. Using the same channel as
/// the positive input for multiple differential analog measurements is also not supported.
/// \warning The LabJack device is configurable to such a degree that neither this class nor LabJackScaffold are able
///		to do significant error-checking.  If the output DataGroup and the calls (e.g., addTimer) to this class
///		are not in agreement, the requests to the LabJack device driver will not be correct.
///
/// \par Application input provided by the device:
///   | type   | name              |                                                                           |
///   | ----   | ----              | ---                                                                       |
///   | scalar | "digitalInput0"   | %Digital input, line #0                                                   |
///   | scalar | "digitalInput1"   | %Digital input, line #1                                                   |
///   | ...    |  ...              | ...                                                                       |
///   | scalar | "digitalInput23"  | %Digital input, line #23                                                  |
///   | scalar | "timerInput0"     | %The input from timer #0 if that timer provides input values              |
///   | scalar | "timerInput1"     | %The input from timer #1 if that timer provides input values              |
///   | ...    |  ...              | ...                                                                       |
///   | scalar | "timerInput6"     | %The input from timer #6 if that timer provides input values              |
///
///
/// \par Application output used by the device:
///   | type   | name              |                                                                           |
///   | ----   | ----              | ---                                                                       |
///   | ----   | ----              | ---                                                                       |
///   | scalar | "digitalOutput0"  | %Digital output, line #0                                                  |
///   | scalar | "digitalOutput1"  | %Digital output, line #1                                                  |
///   | ...    |  ...              | ...                                                                       |
///   | scalar | "digitalOutput23" | %Digital output, line #23                                                 |
///   | scalar | "timerOutput0"    | %The output from timer #0 if that timer provides output values            |
///   | scalar | "timerOutput1"    | %The output from timer #1 if that timer provides output values            |
///   | ...    |  ...              | ...                                                                       |
///   | scalar | "timerOutput6"    | %The output from timer #6 if that timer provides output values            |
///
/// \sa SurgSim::Input::CommonDevice, SurgSim::Input::DeviceInterface, LabJackScaffold
class LabJackDevice : public SurgSim::Input::CommonDevice
{
public:
	/// Constructor.
	/// \param uniqueName A unique name for the device that will be used by the application.
	explicit LabJackDevice(const std::string& uniqueName);

	/// Destructor.
	virtual ~LabJackDevice();

	/// Fully initialize the device.
	/// When the manager object creates the device, the internal state of the device usually isn't fully
	/// initialized yet.  This method performs any needed initialization.
	/// \return True on success.
	/// \exception Asserts if already initialized, if unable to get a scaffold, or if it does not have an outputProducer
	///		and one is needed to provide outputs.
	virtual bool initialize() override;

	/// Check whether this device is initialized.
	bool isInitialized() const;

	/// Set the type of the LabJack, e.g., U6.
	/// \param deviceType The device type.
	/// \exception Asserts if already initialized.
	void setType(LabJackType deviceType);

	/// \return The type of the LabJack, e.g., U6.
	LabJackType getType() const;

	/// Set the connection type of the LabJack, e.g., USB.
	/// \param connection The communication medium.
	/// \exception Asserts if already initialized.
	void setConnection(LabJackConnection connection);

	/// \return The connection type of the LabJack, e.g., USB.
	LabJackConnection getConnection() const;

	/// Set the address of the LabJack, e.g., "1" or "192.168.7.23".  If the address is zero-length, attempt to open the
	/// first-found device of the specified type on the specified connection.
	/// \param address The address for the device, or a zero-length string.
	/// \exception Asserts if already initialized.
	void setAddress(std::string address);

	/// \return The address of the LabJack, e.g., "1" or "192.168.7.23".
	const std::string& getAddress() const;

	/// Enable digital input lines.
	/// \param digitalInputChannels The set of channel numbers.
	/// \exception Asserts if already initialized.
	void setDigitalInputChannels(const std::unordered_set<int>& digitalInputChannels);

	/// \return The enabled digital input lines.
	const std::unordered_set<int>& getDigitalInputChannels() const;

	/// Enable digital output lines.
	/// \param digitalOutputChannels The set of channel numbers.
	/// \exception Asserts if already initialized.
	void setDigitalOutputChannels(const std::unordered_set<int>& digitalOutputChannels);

	/// \return The enabled digital output lines.
	const std::unordered_set<int>& getDigitalOutputChannels() const;

	/// Set the timer base rate.  Timer base rates that end in "_DIV" are divided by the divisor to get the actual timer
	/// frequency.  See section 2.10 - Timers/Counters in the respective LabJack model's User's Guide.
	/// \param base The timer base rate.
	/// \exception Asserts if already initialized.
	void setTimerBase(LabJackTimerBase base);

	/// \return The timer base rate.
	LabJackTimerBase getTimerBase() const;

	/// If the Timer type ends in "_DIV", then the actual timer frequency is divided by the divisor.
	/// \param divisor The amount by which to divide the frequency.  Values from 1-255 are used directly, while 0 means
	/// divide by 256.  Values above 255 are not supported and cause an error.
	/// \exception Asserts if already initialized.
	void setTimerClockDivisor(int divisor);

	/// \return The timer clock divisor.
	int getTimerClockDivisor() const;

	/// The timers and counters are always on consecutive pins, but the start pin can be varied within limits.
	/// \param offset The channel number of the first timer/counter.
	/// \exception Asserts if already initialized.
	void setTimerCounterPinOffset(int offset);

	/// \return The channel number of the first timer/counter.
	int getTimerCounterPinOffset() const;

	/// Enable timers.  The key is the index of the timer, while the value is the mode.
	/// Since quadrature requires two lines, to measure a single quadrature encoder this function
	/// must be called twice on consecutive timerNumbers.  All output timers use the same clock (see setTimerBase and
	/// setTimerClockDivisor).
	/// \param timers A map from the index of the timer (not the line number, see setTimerCounterPinOffset) to the
	///		type of timer to enable.
	/// \exception Asserts if already initialized.
	void setTimers(const std::unordered_map<int, LabJackTimerMode>& timers);

	/// \return The enabled timers.
	const std::unordered_map<int, LabJackTimerMode>& getTimers() const;

	/// Set the maximum update rate for the LabJackThread.  Since the device driver blocks thread execution
	/// while acquiring new data, update rates have a definite upper-bound that is dependent on the requested
	/// inputs (at least).  See the LabJack User's Guide for details.
	void setMaximumUpdateRate(double rate);

	/// \return The maximum update rate for the LabJackThread.
	double getMaximumUpdateRate() const;

	/// Set the differential analog inputs.
	/// \param analogInputs The inputs. The key is the positive channel.
	/// \exception Asserts if already initialized.
	/// \note On Linux, does not correctly handle negative channels 31 or 32 for U3 model.
	void setAnalogInputsDifferential(std::unordered_map<int, LabJackAnalogInputsDifferentialData> analogInputs);

	/// Set the single-ended analog inputs.
	/// \param analogInputs The inputs. The key is the channel.  The value is the range.
	/// \exception Asserts if already initialized.
	void setAnalogInputsSingleEnded(std::unordered_map<int, LabJackAnalogInputRange> analogInputs);

	/// \return The enabled differential analog inputs.
	const std::unordered_map<int, LabJackAnalogInputsDifferentialData>& getAnalogInputsDifferential() const;

	/// \return The enabled single-ended analog inputs.
	const std::unordered_map<int, LabJackAnalogInputRange>& getAnalogInputsSingleEnded() const;

	/// Enable analog output lines.
	/// \param analogOutputChannels The set of channel numbers.
	/// \exception Asserts if already initialized.
	void setAnalogOutputChannels(const std::unordered_set<int>& analogOutputChannels);

	/// \return The enabled analog output lines.
	const std::unordered_set<int>& getAnalogOutputChannels() const;

	/// Set the resolution for all the analog inputs. The resolution parameter is a model-dependent code. Refer to the
	/// User's Guide for the specific model to determine behavior for different codes.  For example, for the U6 see
	/// http://labjack.com/support/u6/users-guide/4.3.3 and http://labjack.com/support/u6/users-guide/appendix-b
	/// \param resolution The resolution code.
	/// \exception Asserts if already initialized.
	void setAnalogInputResolution(int resolution);

	/// \return The resolution code for all the analog inputs.
	int getAnalogInputResolution() const;

	/// Set the settling time for all the analog inputs. The settling parameter is a model-dependent code. Refer to the
	/// User's Guide for the specific model to determine behavior for different codes.  For example, for the U6 see
	/// http://labjack.com/support/u6/users-guide/2.6
	/// \param settling The settling time code.
	/// \exception Asserts if already initialized.
	void setAnalogInputSettling(int settling);

	/// \return The settling time code for all the analog inputs.
	int getAnalogInputSettling() const;

private:
	/// Finalize (de-initialize) the device.
	/// \return True if device was successfully un-registered.
	/// \exception Asserts if not initialized.
	virtual bool finalize() override;

	friend class LabJackScaffold;

	/// The single scaffold object that handles communications with all instances of LabJackDevice.
	std::shared_ptr<LabJackScaffold> m_scaffold;

	/// The type of LabJack, e.g., U6.
	LabJackType m_type;

	/// The type of connection, e.g., USB.
	LabJackConnection m_connection;

	/// The address, or a zero-length string to indicate the first-found device of this type on this connection.
	std::string m_address;

	/// The line numbers for the digital inputs.
	std::unordered_set<int> m_digitalInputChannels;

	/// The single-ended analog inputs.  The key is the channel.  The value is the range (i.e., gain).
	std::unordered_map<int, LabJackAnalogInputRange> m_analogInputsSingleEnded;

	/// The differential analog inputs. The key is the positive channel.
	std::unordered_map<int, LabJackAnalogInputsDifferentialData> m_analogInputsDifferential;

	/// The line numbers for the digital outputs.
	std::unordered_set<int> m_digitalOutputChannels;

	/// The line numbers for the analog outputs.
	std::unordered_set<int> m_analogOutputChannels;

	/// The timer base, which is the frequency of all the output timers unless it ends in "_DIV",
	/// in which case the frequency is the base divided by the divisor.  See section 2.10 - Timers/Counters in the
	/// respective LabJack model's User's Guide.
	LabJackTimerBase m_timerBase;

	/// The timer clock's divisor, see m_timerBase.
	int m_timerClockDivisor;

	/// The number of the lowest FIO pin that is a timer or counter.
	int m_timerCounterPinOffset;

	/// A map from the timers' line numbers to their modes.
	std::unordered_map<int, LabJackTimerMode> m_timers;

	/// The maximum update rate for the LabJackThread.
	double m_threadRate;

	/// The resolution for all the analog inputs.
	int m_analogInputResolution;

	/// The settling time for all the analog inputs.
	int m_analogInputSettling;
};

};  // namespace Device
};  // namespace SurgSim

#endif  // SURGSIM_DEVICES_LABJACK_LABJACKDEVICE_H
