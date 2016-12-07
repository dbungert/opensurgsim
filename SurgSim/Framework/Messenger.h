// This file is a part of the OpenSurgSim project.
// Copyright 2013-2016, SimQuest Solutions Inc.
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


#ifndef SURGSIM_FRAMEWORK_MESSENGER_H
#define SURGSIM_FRAMEWORK_MESSENGER_H


#include "SurgSim/Framework/Timer.h"
#include "SurgSim/Framework/Component.h"

#include <boost/thread/mutex.hpp>

namespace SurgSim
{
namespace Framework
{

/// Messenger implements asynchronous communication to OSS, components can add themselves as subscribers to
/// this class when it is in the system, any component can publish events to the messenger. Events are named via
/// free strings, to be notified the name that was used to subscribe has to match the name that was used to post
/// the event.
/// The publish function doesn't block it just stores the event in a local queue that will be worked off during when
/// update is called by the thread. That will cause a delay in the posting of the event.
/// The event structure sent to the receiver contains the senders full name, the actual name of the event, the time
/// that the event was received by the messenger (this based on a local clock inside the messenger) and
/// some optional data. To decode the data the receiver has to know what type the original data was in.
class Messenger
{
public:

	/// Datastructure to contain basic event data
	struct Event
	{
		Event(const std::string& name, const std::string& sender, double time, const boost::any& data) :
			name(name), sender(sender), time(time), data(data) {}

		Event() : time(0.0) {}
		std::string name; /// Name of the event
		std::string sender; /// Name of the sender
		double time; /// Time the event is received
		boost::any data; /// Data
	};

	typedef std::function<void(const Event&)> EventCallback; /// To receive events this is the format of the callback


	Messenger();

	/// Execute all the queued up callbacks
	void update();

	/// Put an event onto the queue to be sent to all subscribers
	/// \param event The name of the event
	/// \param sender The name of the sender
	/// \param data Optional data
	void publish(const std::string& event, const std::string& sender, const boost::any& data = boost::any());

	/// Put an event onto the queue to be sent to all subscribers
	/// \param event The name of the event
	/// \param sender The Component doing the publishing
	/// \param data Optional data
	void publish(const std::string& event,
				 const std::shared_ptr<Component>& sender,
				 const boost::any& data = boost::any());

	/// Subscribe to receiving events, when an event occurs that matches the `event` the callback function will be
	/// called in the update loop of this class
	/// \param event The name of the event that the subscriber wants to receive
	/// \param subscriber The component receiving the callback
	/// \param callback The function to be called when the event occurs
	void subscribe(const std::string& event, const std::shared_ptr<SurgSim::Framework::Component>& subscriber,
				   const EventCallback& callback);

	/// Subscribe to receiving all events, the subscriber will get notified of all events in the system
	/// \param subscriber The component receiving the callback
	/// \param callback The function to be called when the event occurs
	void subscribe(const std::shared_ptr<SurgSim::Framework::Component>& subscriber,
				   const EventCallback& callback);

	/// Unsubscribe from receiving specific events, prevent subscriber from receiving events of type 'event'
	/// \param event The name of the event that the subscriber doesn't want to receive any more
	/// \param subscriber The subscriber that wants to be unsubscribed
	void unsubscribe(const std::string& event, const std::shared_ptr<SurgSim::Framework::Component>& subscriber);

	/// Remove all subscriptions for the given subscriber
	/// \param subscriber The subscriber that wants to be unsubscribed
	void unsubscribe(const std::shared_ptr<SurgSim::Framework::Component>& subscriber);

private:

	/// Local timer for global wall-clock
	SurgSim::Framework::Timer m_timer;

	typedef std::pair<std::weak_ptr<SurgSim::Framework::Component>, EventCallback> Subscriber;

	/// Subscribers to specific events
	std::unordered_map<std::string, std::vector<Subscriber>> m_subscribers;

	/// Subscribers to all events
	std::vector<Subscriber> m_universalSubscribers;

	/// Mutex for managing the subscribers
	boost::mutex m_subscriberMutex;

	/// List of events that haven't been sent to subscribers
	std::vector<Event> m_events;

	/// Mutex to protect list of events
	boost::mutex m_eventMutex;

	/// Post an event to all its receivers
	void sendEvent(const Event& event, const std::vector<Subscriber>& receivers);

};
}
}

#endif