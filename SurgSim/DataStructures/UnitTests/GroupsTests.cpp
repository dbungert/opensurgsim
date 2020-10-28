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

#include <gtest/gtest.h>

#include "SurgSim/DataStructures/Groups.h"
#include "SurgSim/Framework/BasicSceneElement.h"
#include "SurgSim/Testing/Utilities.h"

namespace SurgSim
{
namespace DataStructures
{

TEST(GroupsTests, InitTest)
{
	// The comma in the template is throwing off the macro, predefine the types for testing
	typedef Groups<int, int> TypeA;
	EXPECT_NO_THROW(TypeA groups;);

	typedef Groups<std::string, int> TypeB;
	EXPECT_NO_THROW({TypeB groups;});

	typedef Groups <std::string, std::shared_ptr<int>> TypeC;
	EXPECT_NO_THROW({TypeC groups;});
}

TEST(GroupsTest, DefaultInitialization)
{
	Groups<std::string, int> groups;

	EXPECT_TRUE(groups.getGroups().empty());

	EXPECT_TRUE(groups.getGroups(1).empty());

	EXPECT_TRUE(groups.getMembers("None").empty());
}

TEST(GroupsTest, Clear)
{
	Groups<std::string, std::shared_ptr<Framework::BasicSceneElement>> groups;

	auto element1 = std::make_shared<Framework::BasicSceneElement>("One");
	EXPECT_TRUE(groups.add("One", element1));
	EXPECT_TRUE(groups.add("Two", element1));

	groups.clear();

	EXPECT_EQ(0u, groups.getGroups().size());
	EXPECT_EQ(0u, groups.getGroups(element1).size());
	EXPECT_EQ(0u, groups.getMembers("One").size());
	EXPECT_EQ(0u, groups.getMembers("Two").size());
}

TEST(GroupsTests, AddElement)
{
	Groups<std::string, std::shared_ptr<Framework::BasicSceneElement>> groups;

	auto element1 = std::make_shared<Framework::BasicSceneElement>("One");
	auto element2 = std::make_shared<Framework::BasicSceneElement>("Two");

	EXPECT_TRUE(groups.add("One", element1));

	EXPECT_EQ(1u, groups.getGroups().size());
	EXPECT_EQ(1u, groups.getGroups(element1).size());
	EXPECT_EQ(1u, groups.getMembers("One").size());

	EXPECT_FALSE(groups.add("One", element1));

	EXPECT_EQ(1u, groups.getGroups().size());
	EXPECT_EQ(1u, groups.getGroups(element1).size());
	EXPECT_EQ(1u, groups.getMembers("One").size());

	groups.add("Two", element1);

	EXPECT_EQ(2u, groups.getGroups().size());
	EXPECT_EQ(2u, groups.getGroups(element1).size());
	EXPECT_EQ(1u, groups.getMembers("One").size());

	groups.add("Two", element2);

	EXPECT_EQ(2u, groups.getGroups().size());
	EXPECT_EQ(2u, groups.getGroups(element1).size());
	EXPECT_EQ(1u, groups.getGroups(element2).size());
	EXPECT_EQ(1u, groups.getMembers("One").size());
	EXPECT_EQ(2u, groups.getMembers("Two").size());


	auto members = groups.getMembers("Two");


	auto names = groups.getGroups();

	EXPECT_TRUE(SurgSim::Testing::doesContain(members, element1));
	EXPECT_TRUE(SurgSim::Testing::doesContain(members, element2));

	EXPECT_TRUE(SurgSim::Testing::doesContain(names, "One"));
	EXPECT_TRUE(SurgSim::Testing::doesContain(names, "Two"));
}

TEST(GroupsTests, MultiAddTest)
{
	Groups<std::string, std::shared_ptr<Framework::BasicSceneElement>> groups;

	auto element1 = std::make_shared<Framework::BasicSceneElement>("One");
	auto element2 = std::make_shared<Framework::BasicSceneElement>("Two");

	std::vector<std::string> names;
	names.push_back("One");
	names.push_back("Two");

	groups.add(names, element1);
	EXPECT_EQ(2u, groups.getGroups().size());
	EXPECT_EQ(2u, groups.getGroups(element1).size());
}

TEST(GroupsTests, AddGroups)
{
	Groups<std::string, int> groups;

	Groups<std::string, int> sourceGroups;

	groups.add("One", 1);
	groups.add("Two", 1);

	sourceGroups.add("One", 2);
	sourceGroups.add("Two", 1);
	sourceGroups.add("Three", 1);

	EXPECT_FALSE(groups.add(groups));

	EXPECT_EQ(2u, groups.getGroups().size());
	EXPECT_EQ(2u, groups.getGroups(1).size());

	EXPECT_TRUE(groups.add(sourceGroups));

	EXPECT_EQ(3u, groups.getGroups().size());
	EXPECT_EQ(3u, groups.getGroups(1).size());
	EXPECT_EQ(1u, groups.getGroups(2).size());
	EXPECT_EQ(2u, groups.getMembers("One").size());
	EXPECT_EQ(1u, groups.getMembers("Two").size());
	EXPECT_EQ(1u, groups.getMembers("Three").size());
}

TEST(GroupsTests, BracketOperator)
{
	Groups<std::string, std::shared_ptr<Framework::BasicSceneElement>> groups;

	auto element1 = std::make_shared<Framework::BasicSceneElement>("One");
	auto element2 = std::make_shared<Framework::BasicSceneElement>("Two");


	EXPECT_EQ(0u, groups["One"].size());

	groups.add("One", element1);
	groups.add("One", element2);

	EXPECT_EQ(2u, groups["One"].size());
}

TEST(GroupsTests, ValidRemoves)
{
	Groups<std::string, std::shared_ptr<Framework::BasicSceneElement>> groups;

	auto element1 = std::make_shared<Framework::BasicSceneElement>("One");
	auto element2 = std::make_shared<Framework::BasicSceneElement>("Two");
	auto element3 = std::make_shared<Framework::BasicSceneElement>("Three");

	groups.add("One", element1);
	groups.add("Two", element1);
	groups.add("Three", element1);
	groups.add("One", element2);
	groups.add("Two", element2);

	// Just check for the expected state ... (is tested above)
	EXPECT_EQ(3u, groups.getGroups().size());
	EXPECT_EQ(3u, groups.getGroups(element1).size());
	EXPECT_EQ(2u, groups.getGroups(element2).size());
	EXPECT_EQ(2u, groups.getMembers("One").size());
	EXPECT_EQ(2u, groups.getMembers("Two").size());
	EXPECT_EQ(1u, groups.getMembers("Three").size());

	EXPECT_TRUE(groups.remove("Three", element1));

	EXPECT_EQ(2u, groups.getGroups().size());
	EXPECT_EQ(2u, groups.getGroups(element1).size());
	EXPECT_EQ(2u, groups.getGroups(element2).size());
	EXPECT_EQ(2u, groups.getMembers("One").size());
	EXPECT_EQ(2u, groups.getMembers("Two").size());
	EXPECT_EQ(0u, groups.getMembers("Three").size());

	EXPECT_TRUE(groups.remove("One", element2));

	EXPECT_EQ(2u, groups.getGroups().size());
	EXPECT_EQ(2u, groups.getGroups(element1).size());
	EXPECT_EQ(1u, groups.getGroups(element2).size());
	EXPECT_EQ(1u, groups.getMembers("One").size());
	EXPECT_EQ(2u, groups.getMembers("Two").size());
	EXPECT_EQ(0u, groups.getMembers("Three").size());

	groups.remove("One", element1);
	groups.remove("Two", element1);
	groups.remove("Two", element2);

	EXPECT_EQ(0u, groups.getGroups().size());
	EXPECT_EQ(0u, groups.getGroups(element1).size());
	EXPECT_EQ(0u, groups.getGroups(element2).size());
	EXPECT_EQ(0u, groups.getMembers("One").size());
	EXPECT_EQ(0u, groups.getMembers("Two").size());
	EXPECT_EQ(0u, groups.getMembers("Three").size());

}

TEST(GroupsTests, InvalidRemoves)
{
	Groups<std::string, std::shared_ptr<Framework::BasicSceneElement>> groups;

	auto element1 = std::make_shared<Framework::BasicSceneElement>("One");
	auto element2 = std::make_shared<Framework::BasicSceneElement>("Two");
	auto element3 = std::make_shared<Framework::BasicSceneElement>("Three");

	groups.add("One", element1);
	groups.add("Two", element1);
	groups.add("Three", element1);
	groups.add("One", element2);
	groups.add("Two", element2);

	// Remove non existing element from group should not do anything
	EXPECT_FALSE(groups.remove("One", element3));
	EXPECT_EQ(2u, groups.getMembers("One").size());
	EXPECT_EQ(0u, groups.getGroups(element3).size());

	// Removing from a non existing group should not do anything
	EXPECT_FALSE(groups.remove("None", element1));
	EXPECT_EQ(3u, groups.getGroups().size());
	EXPECT_EQ(3u, groups.getGroups(element1).size());

	// Removing a non exisiting member from a nonexisting group should not do anything
	EXPECT_FALSE(groups.remove("None", element3));
	EXPECT_EQ(3u, groups.getGroups().size());
	EXPECT_EQ(0u, groups.getGroups(element3).size());

	// Removing a non exisitng member categorically should not do anything
	EXPECT_FALSE(groups.remove(element3));
	EXPECT_EQ(3u, groups.getGroups().size());
	EXPECT_EQ(0u, groups.getGroups(element3).size());
}

}
}
