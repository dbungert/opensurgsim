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

#include "SurgSim/Devices/Sixense/SixenseDevice.h"

#include <iostream>
#include <iomanip>

#include <sixense.h>

#include "SurgSim/Devices/Sixense/SixenseManager.h"
#include "SurgSim/Math/Vector.h"
#include "SurgSim/Math/Matrix.h"
#include "SurgSim/Math/RigidTransform.h"
#include "SurgSim/Framework/Log.h"
#include "SurgSim/DataStructures/DataGroup.h"
#include "SurgSim/DataStructures/DataGroupBuilder.h"

using SurgSim::Math::Vector3d;
using SurgSim::Math::Vector3f;
using SurgSim::Math::Matrix44d;
using SurgSim::Math::Matrix33d;
using SurgSim::Math::RigidTransform3d;

using SurgSim::DataStructures::DataGroup;
using SurgSim::DataStructures::DataGroupBuilder;


namespace SurgSim
{
namespace Device
{


SixenseDevice::SixenseDevice(const std::string& uniqueName, int baseIndex, int controllerIndex,
							 std::shared_ptr<SurgSim::Framework::Logger> logger) :
	SurgSim::Input::CommonDevice(uniqueName, buildInputData()),
	m_logger(logger),
	m_baseIndex(baseIndex),
	m_controllerIndex(controllerIndex),
	m_messageLabel("Device " + uniqueName + ": ")
{
}

SixenseDevice::~SixenseDevice()
{
	finalize();  // it's OK if we finalized already
}

std::shared_ptr<SixenseDevice> SixenseDevice::create(const std::string& uniqueName)
{
	std::shared_ptr<SixenseManager> manager = SixenseManager::getOrCreateSharedInstance();
	std::shared_ptr<SixenseDevice> device = manager->createDevice(uniqueName);
	if (! device)
	{
		return std::shared_ptr<SixenseDevice>();
	}
	device->setManager(manager);
	return device;
}

bool SixenseDevice::initialize()
{
	// The device doesn't actually need any initialization as such, but we check that we can talk to the controller.
	{
		int status = sixenseSetActiveBase(m_baseIndex);
		if (status != SIXENSE_SUCCESS)
		{
			SURGSIM_LOG_CRITICAL(m_logger) << m_messageLabel << "Could not activate base unit #" <<
				m_baseIndex << " while initializing device! (status = " << status << ")";
			return false;
		}

		sixenseControllerData data;
		status = sixenseGetNewestData(m_controllerIndex, &data);
		if (status != SIXENSE_SUCCESS)
		{
			SURGSIM_LOG_CRITICAL(m_logger) << m_messageLabel << "Could not get data from controller #" <<
				m_baseIndex << "," << m_controllerIndex << " while initializing device! (status = " << status << ")";
			return false;
		}
	}

	SURGSIM_LOG_INFO(m_logger) << m_messageLabel << "Initialized." << std::endl;

	return true;
}

bool SixenseDevice::finalize()
{
	if (! m_manager)
	{
		SURGSIM_LOG_SEVERE(m_logger) << m_messageLabel << "Finalizing, but no manager is present!";
	}
	else
	{
		SURGSIM_LOG_DEBUG(m_logger) << m_messageLabel << "Finalizing.";
		m_manager->releaseDevice(this);
	}
	return true;
}

bool SixenseDevice::update()
{
	int status = sixenseSetActiveBase(m_baseIndex);
	if (status != SIXENSE_SUCCESS)
	{
		SURGSIM_LOG_CRITICAL(m_logger) << m_messageLabel << "Could not activate base unit #" << m_baseIndex <<
			" for existing device! (status = " << status << ")";
		return false;
	}

	sixenseControllerData data;
	status = sixenseGetNewestData(m_controllerIndex, &data);
	if (status != SIXENSE_SUCCESS)
	{
		SURGSIM_LOG_CRITICAL(m_logger) << m_messageLabel << "Could not get data from controller #" << m_baseIndex <<
			"," << m_controllerIndex << " for existing device! (status = " << status << ")";
		return false;
	}

	{
		// Use Eigen::Map to make the raw API output values look like Eigen data types
		Eigen::Map<Vector3f> position(data.pos);
		Eigen::Map<Eigen::Matrix<float, 3, 3, Eigen::ColMajor>> orientation(&(data.rot_mat[0][0]));

		RigidTransform3d pose;
		pose.makeAffine();
		pose.linear() = orientation.cast<double>();
		pose.translation() = position.cast<double>() * 0.001;  // convert from millimeters to meters!

		// TODO(bert): this code should cache the access indices.
		getInputData().poses().set("pose", pose);
		getInputData().scalars().set("trigger", data.trigger);
		getInputData().scalars().set("joystickX", data.joystick_x);
		getInputData().scalars().set("joystickY", data.joystick_y);
		getInputData().booleans().set("buttonTrigger", (data.trigger > 0));
		getInputData().booleans().set("buttonBumper", (data.buttons & SIXENSE_BUTTON_BUMPER) != 0);
		getInputData().booleans().set("button1", (data.buttons & SIXENSE_BUTTON_1) != 0);
		getInputData().booleans().set("button2", (data.buttons & SIXENSE_BUTTON_2) != 0);
		getInputData().booleans().set("button3", (data.buttons & SIXENSE_BUTTON_3) != 0);
		getInputData().booleans().set("button4", (data.buttons & SIXENSE_BUTTON_4) != 0);
		getInputData().booleans().set("buttonStart", (data.buttons & SIXENSE_BUTTON_START) != 0);
		getInputData().booleans().set("buttonJoystick", (data.buttons & SIXENSE_BUTTON_JOYSTICK) != 0);
	}

	return true;
}

DataGroup SixenseDevice::buildInputData()
{
	DataGroupBuilder builder;
	builder.addPose("pose");
	builder.addScalar("trigger");
	builder.addScalar("joystickX");
	builder.addScalar("joystickY");
	builder.addBoolean("buttonTrigger");
	builder.addBoolean("buttonBumper");
	builder.addBoolean("button1");
	builder.addBoolean("button2");
	builder.addBoolean("button3");
	builder.addBoolean("button4");
	builder.addBoolean("buttonStart");
	builder.addBoolean("buttonJoystick");
	return builder.createData();
}


};  // namespace Device
};  // namespace SurgSim
