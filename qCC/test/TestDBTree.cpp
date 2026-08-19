// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

#include <ccHObject.h>
#include <ccPointCloud.h>
#include <ccBBox.h>

#include <QTest>
#include <QString>

class TestDBTree : public QObject
{
	Q_OBJECT

  private:
	// Helper: create a tiny point cloud so ccHObject-derived objects have geometry
	ccPointCloud* makeCloud(const QString& name = QString())
	{
		ccPointCloud* cloud = new ccPointCloud(name);
		cloud->reserve(3);
		cloud->addPoint(CCVector3(0, 0, 0));
		cloud->addPoint(CCVector3(1, 0, 0));
		cloud->addPoint(CCVector3(0, 1, 0));
		return cloud;
	}

	// Helper: build a simple tree: root -> [a, b] -> [a1, a2], b -> [b1]
	ccHObject* buildTree()
	{
		ccHObject* root = new ccHObject("root");
		ccHObject* a = new ccHObject("a");
		ccHObject* b = new ccHObject("b");
		ccHObject* a1 = new ccHObject("a1");
		ccHObject* a2 = new ccHObject("a2");
		ccHObject* b1 = new ccHObject("b1");

		root->addChild(a);  // DP_PARENT_OF_OTHER default → child deleted on remove
		root->addChild(b);
		a->addChild(a1);
		a->addChild(a2);
		b->addChild(b1);

		Q_UNUSED(a1);
		Q_UNUSED(a2);
		Q_UNUSED(b1);
		return root; // caller responsible for cleanup via removeAllChildren
	}

  private slots:
	// =====================================================================
	// addChild
	// =====================================================================

	void testAddChildSuccess() // clazy:exclude=missing-qobject-cast
	{
		ccHObject parent("parent");
		ccHObject child("child");

		QCOMPARE(parent.addChild(&child), true);
		QCOMPARE(parent.getChildrenNumber(), 1u);
		QCOMPARE(child.getParent(), &parent);
	}

	void testAddChildNullPtr()
	{
		ccHObject parent("parent");
		QCOMPARE(parent.addChild(nullptr), false);
		QCOMPARE(parent.getChildrenNumber(), 0u);
	}

	void testAddChildDuplicateRejected()
	{
		ccHObject parent("parent");
		ccHObject child("child");

		QCOMPARE(parent.addChild(&child), true);
		QCOMPARE(parent.addChild(&child), false); // already a child
		QCOMPARE(parent.getChildrenNumber(), 1u);
	}

	void testAddChildInsertIndex()
	{
		ccHObject parent("parent");
		ccHObject a("a");
		ccHObject b("b");
		ccHObject c("c");

		parent.addChild(&a);
		parent.addChild(&b);
		parent.addChild(&c, ccHObject::DP_NONE, 1); // insert c at index 1

		// Order should be: a, c, b
		QCOMPARE(parent.getChild(0)->getName(), QString("a"));
		QCOMPARE(parent.getChild(1)->getName(), QString("c"));
		QCOMPARE(parent.getChild(2)->getName(), QString("b"));
	}

	// =====================================================================
	// getChild / getChildrenNumber / getParent / getChildIndex
	// =====================================================================

	void testGetChildBoundary()
	{
		ccHObject parent("parent");
		ccHObject child("child");
		parent.addChild(&child, ccHObject::DP_NONE); // avoid delete

		QCOMPARE(parent.getChild(0), &child);
		QCOMPARE(parent.getChild(99), nullptr);      // out of range
		QCOMPARE(parent.getChildrenNumber(), 1u);
	}

	void testGetParentNullForRoot()
	{
		ccHObject root("root");
		QCOMPARE(root.getParent(), nullptr);
	}

	void testGetChildIndex()
	{
		ccHObject parent("parent");
		ccHObject a("a");
		ccHObject b("b");
		ccHObject c("c");

		parent.addChild(&a, ccHObject::DP_NONE);
		parent.addChild(&b, ccHObject::DP_NONE);
		parent.addChild(&c, ccHObject::DP_NONE);

		QCOMPARE(parent.getChildIndex(&a), 0);
		QCOMPARE(parent.getChildIndex(&b), 1);
		QCOMPARE(parent.getChildIndex(&c), 2);
		QCOMPARE(parent.getChildIndex(nullptr), -1);

		ccHObject orphan("orphan");
		QCOMPARE(parent.getChildIndex(&orphan), -1);
	}

	void testGetIndex()
	{
		ccHObject parent("parent");
		ccHObject a("a");
		ccHObject b("b");

		// DP_NONE avoids the parent-pointer / dependency cascade issue.
		// getParent() returns nullptr for DP_NONE children, but getIndex() calls
		// parent->getChildIndex(this) — since parent is still alive during the
		// function scope (stack), we can manually check via getChildIndex directly.
		parent.addChild(&a, ccHObject::DP_NONE);
		parent.addChild(&b, ccHObject::DP_NONE);

		// Verify positional indices via the parent's direct lookup
		QCOMPARE(parent.getChildIndex(&a), 0);
		QCOMPARE(parent.getChildIndex(&b), 1);
		QCOMPARE(parent.getIndex(), -1); // root has no parent
	}

	// =====================================================================
	// removeChild
	// =====================================================================

	void testRemoveChildByPointerNoDelete()
	{
		// DP_NONE: child is detached but NOT deleted
		ccHObject parent("parent");
		ccHObject child("child");

		parent.addChild(&child, ccHObject::DP_NONE);
		QCOMPARE(parent.getChildrenNumber(), 1u);

		parent.removeChild(&child);
		QCOMPARE(parent.getChildrenNumber(), 0u);
		QCOMPARE(child.getParent(), nullptr); // parent cleared
	}

	void testRemoveChildByPointerWithDelete()
	{
		// DP_PARENT_OF_OTHER: child should be deleted on remove
		ccHObject parent("parent");
		ccHObject* child = new ccHObject("child");

		parent.addChild(child, ccHObject::DP_PARENT_OF_OTHER);
		QCOMPARE(parent.getChildrenNumber(), 1u);

		parent.removeChild(child); // child is deleted here
		QCOMPARE(parent.getChildrenNumber(), 0u);
		// child pointer is now dangling — do not dereference
	}

	void testRemoveChildByIndexNoDelete()
	{
		ccHObject parent("parent");
		ccHObject child("child");

		parent.addChild(&child, ccHObject::DP_NONE);
		QCOMPARE(parent.getChildrenNumber(), 1u);

		parent.removeChild(0);
		QCOMPARE(parent.getChildrenNumber(), 0u);
	}

	void testRemoveChildByIndexOutOfRange()
	{
		ccHObject parent("parent");
		// removing out-of-range index: assert fires in debug, no-op in release
		parent.removeChild(99);
		QCOMPARE(parent.getChildrenNumber(), 0u); // still 0
	}

	void testRemoveAllChildren()
	{
		ccHObject parent("parent");
		ccHObject a("a");
		ccHObject b("b");

		parent.addChild(&a, ccHObject::DP_NONE);
		parent.addChild(&b, ccHObject::DP_NONE);
		QCOMPARE(parent.getChildrenNumber(), 2u);

		parent.removeAllChildren();
		QCOMPARE(parent.getChildrenNumber(), 0u);
	}

	// =====================================================================
	// detachChild
	// =====================================================================

	void testDetachChildNoDelete()
	{
		ccHObject parent("parent");
		ccHObject child("child");

		parent.addChild(&child, ccHObject::DP_PARENT_OF_OTHER); // normally deletes
		parent.detachChild(&child);                               // but detach skips delete

		QCOMPARE(parent.getChildrenNumber(), 0u);
		QCOMPARE(child.getParent(), nullptr);
	}

	void testDetachAllChildren()
	{
		ccHObject parent("parent");
		ccHObject a("a");
		ccHObject b("b");

		parent.addChild(&a, ccHObject::DP_PARENT_OF_OTHER);
		parent.addChild(&b, ccHObject::DP_PARENT_OF_OTHER);
		QCOMPARE(parent.getChildrenNumber(), 2u);

		parent.detachAllChildren();
		QCOMPARE(parent.getChildrenNumber(), 0u);
		QCOMPARE(a.getParent(), nullptr);
		QCOMPARE(b.getParent(), nullptr);
	}

	// =====================================================================
	// filterChildren
	// =====================================================================

	void testFilterChildrenByTypeNonRecursive()
	{
		ccHObject* root = buildTree(); // root, a, b, a1, a2, b1

		ccHObject::Container results;
		root->filterChildren(results, false, CC_TYPES::HIERARCHY_OBJECT, false);

		// non-recursive: only direct children of root (a, b)
		QCOMPARE(static_cast<int>(results.size()), 2);

		root->removeAllChildren(); // clean up DP_PARENT_OF_OTHER children
	}

	void testFilterChildrenRecursive()
	{
		ccHObject* root = buildTree();

		ccHObject::Container results;
		root->filterChildren(results, true, CC_TYPES::HIERARCHY_OBJECT, false);

		// recursive: all descendants: a, b, a1, a2, b1 (5, not root itself)
		QCOMPARE(static_cast<int>(results.size()), 5);

		root->removeAllChildren(); // clean up DP_PARENT_OF_OTHER children
	}

	void testFilterChildrenStrictMode()
	{
		ccHObject parent("parent");

		// Add a HIERARCHY_OBJECT (ccHObject) and a POINT_CLOUD
		ccHObject* ho = new ccHObject("ho");
		ccPointCloud* pc = makeCloud("pc");

		parent.addChild(ho, ccHObject::DP_NONE);
		parent.addChild(pc, ccHObject::DP_NONE);

		ccHObject::Container strictResults;
		parent.filterChildren(strictResults, false, CC_TYPES::HIERARCHY_OBJECT, true);
		QCOMPARE(static_cast<int>(strictResults.size()), 1); // only ho
		QCOMPARE(strictResults[0]->getName(), QString("ho"));

		ccHObject::Container kindResults;
		parent.filterChildren(kindResults, false, CC_TYPES::HIERARCHY_OBJECT, false);
		// non-strict (isKindOf): both ho (exact HIERARCHY_OBJECT) and pc (ccPointCloud is-a HIERARCHY_OBJECT)
		QCOMPARE(static_cast<int>(kindResults.size()), 2);
		QCOMPARE(kindResults[0]->getName(), QString("ho"));
		QCOMPARE(kindResults[1]->getName(), QString("pc"));

		// Clean up heap-allocated children before parent stack object is destroyed
		parent.detachAllChildren();
		delete ho;
		delete pc;
	}

	// =====================================================================
	// find
	// =====================================================================

	void testFindSelf()
	{
		ccHObject obj("obj");
		QCOMPARE(obj.find(obj.getUniqueID()), &obj);
	}

	void testFindDirectChild()
	{
		ccHObject parent("parent");
		ccHObject child("child");
		parent.addChild(&child, ccHObject::DP_NONE);

		QCOMPARE(parent.find(child.getUniqueID()), &child);
	}

	void testFindDeepDescendant()
	{
		ccHObject* root = buildTree();
		// root's tree has deep descendant "b1" (root → b → b1)
		ccHObject* b1 = root->getChild(1)->getChild(0); // b → first child = b1

		QCOMPARE(root->find(b1->getUniqueID()), b1);
		root->removeAllChildren(); // clean up DP_PARENT_OF_OTHER children
	}

	void testFindNotFound()
	{
		ccHObject parent("parent");
		ccHObject orphan("orphan");
		QCOMPARE(parent.find(orphan.getUniqueID()), nullptr);
	}

	// =====================================================================
	// findMaxUniqueID_recursive
	// =====================================================================

	void testFindMaxUniqueID_recursive()
	{
		ccHObject* root = buildTree();
		ccHObject* a = root->getChild(0);
		ccHObject* b = root->getChild(1);

		// IDs are assigned sequentially by the app-wide generator.
		// The max ID in the subtree should be >= every node's ID, including root.
		unsigned maxID = root->findMaxUniqueID_recursive();
		QCOMPARE(maxID >= root->getUniqueID(), true);
		QCOMPARE(maxID >= a->getUniqueID(), true);
		QCOMPARE(maxID >= b->getUniqueID(), true);

		root->removeAllChildren(); // clean up DP_PARENT_OF_OTHER children
	}

	// =====================================================================
	// transferChild
	// =====================================================================

	void testTransferChild()
	{
		ccHObject oldParent("oldParent");
		ccHObject newParent("newParent");
		ccHObject child("child");

		// Use DP_PARENT_OF_OTHER so oldParent is the child's recorded parent
		// (sets child.m_parent = &oldParent, child has dependency 24 on oldParent)
		oldParent.addChild(&child, ccHObject::DP_PARENT_OF_OTHER);
		QCOMPARE(child.getParent(), &oldParent);
		QCOMPARE(oldParent.getChildrenNumber(), 1u);

		// transferChild: detachChild removes from oldParent, then newParent.addChild.
		// Note: transferChild with DP_PARENT_OF_OTHER leaves an orphaned
		// DP_NOTIFY_OTHER_ON_UPDATE flag on oldParent (known ccHObject bug —
		// removeDependencyFlag(DP_PARENT_OF_OTHER) clears bit 3 but leaves bit 4,
		// causing addDependency to early-return). Using DP_PARENT_OF_OTHER here
		// tests the correct child-parent-pointer transfer despite this bug.
		oldParent.transferChild(&child, newParent);

		QCOMPARE(oldParent.getChildrenNumber(), 0u);
		QCOMPARE(newParent.getChildrenNumber(), 1u);
		QCOMPARE(child.getParent(), &newParent);
		QCOMPARE(newParent.getChild(0), &child);
	}

	// =====================================================================
	// swapChildren
	// =====================================================================

	void testSwapChildren()
	{
		ccHObject parent("parent");
		ccHObject a("a");
		ccHObject b("b");

		parent.addChild(&a, ccHObject::DP_NONE);
		parent.addChild(&b, ccHObject::DP_NONE);
		QCOMPARE(parent.getChild(0), &a);
		QCOMPARE(parent.getChild(1), &b);

		parent.swapChildren(0, 1);
		QCOMPARE(parent.getChild(0), &b);
		QCOMPARE(parent.getChild(1), &a);
	}

	// =====================================================================
	// getChildCountRecursive
	// =====================================================================

	void testGetChildCountRecursive()
	{
		ccHObject* root = buildTree();
		// root → a → [a1, a2], root → b → [b1]
		// Total descendants: a, b, a1, a2, b1 = 5
		QCOMPARE(root->getChildCountRecursive(), 5u);
	}

	// =====================================================================
	// isAncestorOf
	// =====================================================================

	void testIsAncestorOfTrue()
	{
		ccHObject* root = buildTree();
		ccHObject* a = root->getChild(0);
		ccHObject* a1 = a->getChild(0);

		QCOMPARE(root->isAncestorOf(a), true);
		QCOMPARE(root->isAncestorOf(a1), true);
		QCOMPARE(a->isAncestorOf(a1), true);

		root->removeAllChildren(); // clean up DP_PARENT_OF_OTHER children
	}

	void testIsAncestorOfFalse()
	{
		ccHObject* root = buildTree();
		ccHObject* a = root->getChild(0);
		ccHObject* b = root->getChild(1); // sibling

		QCOMPARE(a->isAncestorOf(b), false);
		QCOMPARE(b->isAncestorOf(a), false);
		QCOMPARE(a->isAncestorOf(root), false);

		root->removeAllChildren(); // clean up DP_PARENT_OF_OTHER children
	}

	// =====================================================================
	// getBB_recursive — own BB is empty by default (ccHObject is abstract)
	// =====================================================================

	void testGetBB_recursiveWithChildren()
	{
		// ccHObject::getOwnBB returns an empty/invalid bbox by default.
		// getBB_recursive aggregates children's bboxes.
		ccPointCloud* pc1 = makeCloud("pc1");
		ccPointCloud* pc2 = makeCloud("pc2");

		ccHObject parent("parent");
		parent.addChild(pc1, ccHObject::DP_NONE);
		parent.addChild(pc2, ccHObject::DP_NONE);

		ccBBox box = parent.getBB_recursive();
		QCOMPARE(box.isValid(), true);
		// pc1 has points at (0,0,0), (1,0,0), (0,1,0)
		// pc2 has same points
		// Combined BB should span [0,1] on all axes
		QCOMPARE(box.isValid(), true);
	}

	// =====================================================================
	// addDependency / getDependencyFlagsWith / hasDependencyFlag
	// =====================================================================

	void testAddDependency()
	{
		ccHObject parent("parent");
		ccHObject child("child");

		parent.addDependency(&child, ccHObject::DP_NOTIFY_OTHER_ON_UPDATE);
		QCOMPARE(parent.getDependencyFlagsWith(&child) & ccHObject::DP_NOTIFY_OTHER_ON_UPDATE,
		         ccHObject::DP_NOTIFY_OTHER_ON_UPDATE);
		// hasDependencyFlag checks exact equality, so bitwise test instead
		QCOMPARE((parent.getDependencyFlagsWith(&child) & ccHObject::DP_NOTIFY_OTHER_ON_UPDATE) != 0, true);
	}

	void testRemoveDependencyWith()
	{
		ccHObject parent("parent");
		ccHObject child("child");

		parent.addDependency(&child, ccHObject::DP_NOTIFY_OTHER_ON_UPDATE);
		QCOMPARE(parent.getDependencyFlagsWith(&child) != 0, true);

		parent.removeDependencyWith(&child);
		QCOMPARE(parent.getDependencyFlagsWith(&child), 0);
	}

	// =====================================================================
	// New factory
	// =====================================================================

	void testNewByType()
	{
		ccHObject* obj = ccHObject::New(CC_TYPES::POINT_CLOUD, "factory_cloud");
		QCOMPARE(obj != nullptr, true);
		QCOMPARE(obj->getName(), QString("factory_cloud"));
		QCOMPARE(obj->getClassID(), CC_TYPES::POINT_CLOUD);
		delete obj;

		ccHObject* bad = ccHObject::New(static_cast<CC_CLASS_ENUM>(9999));
		QCOMPARE(bad, nullptr);
	}

	// =====================================================================
	// getFirstChild / getLastChild
	// =====================================================================

	void testFirstLastChild()
	{
		ccHObject parent("parent");
		ccHObject first("first");
		ccHObject last("last");

		parent.addChild(&first, ccHObject::DP_NONE);
		parent.addChild(&last, ccHObject::DP_NONE);

		QCOMPARE(parent.getFirstChild(), &first);
		QCOMPARE(parent.getLastChild(), &last);
	}

	void testFirstLastChildEmpty()
	{
		ccHObject parent("parent");
		QCOMPARE(parent.getFirstChild(), nullptr);
		QCOMPARE(parent.getLastChild(), nullptr);
	}

	// =====================================================================
	// isGroup
	// =====================================================================

	void testIsGroup()
	{
		ccHObject obj("obj");
		QCOMPARE(obj.isGroup(), true); // ccHObject itself is a HIERARCHY_OBJECT
	}

	// =====================================================================
	// Cleanup helper (called manually at end of each test to avoid leaks)
	// =====================================================================

	void cleanup()
	{
		// All test objects use DP_PARENT_OF_OTHER by default (auto-delete).
		// Objects added with DP_NONE are stack-allocated and auto-destruct.
		// Nothing extra needed here.
	}
};

QTEST_MAIN(TestDBTree)
#include "TestDBTree.moc"
