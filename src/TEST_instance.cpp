#include <iostream>

#include "test.h"
#include "uiframe.h"
#include "uitext.h"

void testInstances() {
	UIFrame grandparent, parent;
	UIText child, newParent;

	ASSERT_EQUALS(grandparent.getParent(), nullptr, "Grandparent has no initial parent")
	ASSERT_EQUALS(parent.getParent(), nullptr, "Parent has no initial parent")
	ASSERT_EQUALS(child.getParent(), nullptr, "Child has no initial parent")
	
	grandparent.name = "grandparent";
	parent.name = "parent";
	child.name = "child";

	ASSERT_EQUALS(grandparent.name, "grandparent", "Grandparent has correct name")
	ASSERT_EQUALS(parent.name, "parent", "parent has correct name")
	ASSERT_EQUALS(child.name, "child", "Child has correct name")
	
	child.setParent(dynamic_cast<Instance*>(&parent));
	parent.setParent(dynamic_cast<Instance*>(&grandparent));

	ASSERT_EQUALS(grandparent.getParent(), nullptr, "Grandparent has correct parent")
	ASSERT_EQUALS(parent.getParent(), &grandparent, "Parent has correct parent")
	ASSERT_EQUALS(child.getParent(), &parent, "Child has correct parent")

	ASSERT_EQUALS(grandparent.findChild("parent"), &parent, "Parent found in grandparent children")
	ASSERT_EQUALS(parent.findChild("child"), &child, "Child found in parent children")

	child.setParent(dynamic_cast<Instance*>(&newParent));

	ASSERT_EQUALS(child.getParent(), &newParent, "Child has new parent")
	ASSERT_EQUALS(parent.findChild("child"), nullptr, "Child gone in parent children")
	ASSERT_EQUALS(newParent.findChild("child"), &child, "Child found in newParent children")

	ASSERT_THROWS({
		child.setParent(dynamic_cast<Instance*>(&child));
	}, "Setting child parent to parent throws");
}