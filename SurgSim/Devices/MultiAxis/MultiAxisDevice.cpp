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

#include "SurgSim/Devices/MultiAxis/MultiAxisDevice.h"

#include "SurgSim/Devices/MultiAxis/RawMultiAxisDevice.h"
#include "SurgSim/Devices/MultiAxis/PoseIntegrator.h"
#include "SurgSim/Framework/Log.h"
#include "SurgSim/Framework/Assert.h"


namespace SurgSim
{
namespace Device
{


MultiAxisDevice::MultiAxisDevice(const std::string& uniqueName) :
	CommonDevice(uniqueName),
	m_name(uniqueName),
	m_rawDevice(new RawMultiAxisDevice(uniqueName + "_RawBase"))
{
	m_filter = std::make_shared<PoseIntegrator>(uniqueName + "_Integrator", m_rawDevice->getInitialInputData());
	m_filter->setNameForCallback(uniqueName);  // the filter should make callbacks as the entire device

	m_rawDevice->addInputConsumer(m_filter);
	m_rawDevice->setOutputProducer(m_filter);

	m_rawDevice->setPositionScale(defaultPositionScale());
	m_rawDevice->setOrientationScale(defaultOrientationScale());
	m_rawDevice->setAxisDominance(true);
}


MultiAxisDevice::~MultiAxisDevice()
{
}


std::string MultiAxisDevice::getName() const
{
	return m_name;
}


bool MultiAxisDevice::initialize()
{
	// Elements need to be initialized, in proper order.
	return m_filter->initialize() && m_rawDevice->initialize();
}


bool MultiAxisDevice::finalize()
{
	// All elements need to be finalized (beware of &&), in proper order.
	bool deviceOk = m_rawDevice->finalize();
	bool filterOk = m_filter->finalize();
	return deviceOk && filterOk;
}


bool MultiAxisDevice::isInitialized() const
{
	return m_rawDevice->isInitialized() && m_filter->isInitialized();
}

bool MultiAxisDevice::addInputConsumer(std::shared_ptr<SurgSim::Input::InputConsumerInterface> inputConsumer)
{
	return m_filter->addInputConsumer(inputConsumer);
}

bool MultiAxisDevice::removeInputConsumer(std::shared_ptr<SurgSim::Input::InputConsumerInterface> inputConsumer)
{
	return m_filter->removeInputConsumer(inputConsumer);
}

bool MultiAxisDevice::setOutputProducer(std::shared_ptr<SurgSim::Input::OutputProducerInterface> outputProducer)
{
	return m_filter->setOutputProducer(outputProducer);
}

bool MultiAxisDevice::removeOutputProducer(std::shared_ptr<SurgSim::Input::OutputProducerInterface> outputProducer)
{
	return m_filter->removeOutputProducer(outputProducer);
}

bool MultiAxisDevice::hasOutputProducer()
{
	return m_filter->hasOutputProducer();
}

void MultiAxisDevice::setPositionScale(double scale)
{
	m_rawDevice->setPositionScale(scale);
}


double MultiAxisDevice::getPositionScale() const
{
	return m_rawDevice->getPositionScale();
}


void MultiAxisDevice::setOrientationScale(double scale)
{
	m_rawDevice->setOrientationScale(scale);
}


double MultiAxisDevice::getOrientationScale() const
{
	return m_rawDevice->getOrientationScale();
}

void MultiAxisDevice::setAxisDominance(bool onOff)
{
	m_rawDevice->setAxisDominance(onOff);
}


bool MultiAxisDevice::isUsingAxisDominance() const
{
	return m_rawDevice->isUsingAxisDominance();
}


};  // namespace Device
};  // namespace SurgSim
