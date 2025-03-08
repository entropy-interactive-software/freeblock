#include "joint.hpp"

#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/NarrowPhaseCollision/btManifoldPoint.h"
#include "BulletCollision/NarrowPhaseCollision/btPersistentManifold.h"
#include "BulletDynamics/ConstraintSolver/btFixedConstraint.h"
#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "block.hpp"
#include "instance.hpp"
#include "model.hpp"
namespace freeblock {
INSTANCE_CTOR(WeldInstance, Instance){};

void WeldInstance::link() {
  btRigidBody* a =
      getDM()->getInstanceByUUID<BlockInstance>(this->a)->getRigidBody();
  btRigidBody* b =
      getDM()->getInstanceByUUID<BlockInstance>(this->b)->getRigidBody();

  btTransform frameInA;
  frameInA.setIdentity();
  frameInA.setOrigin(pointA);

  btTransform frameInB;
  frameInB.setIdentity();
  frameInB.setOrigin(pointB);

  frameInA = a->getCenterOfMassTransform().inverse() * frameInA;
  frameInB = b->getCenterOfMassTransform().inverse() * frameInB;

  float totalMass = 1.f / a->getInvMass() + 1.f / b->getInvMass();

  btFixedConstraint* p2p = new btFixedConstraint(*a, *b, frameInA, frameInB);
  // p2p->setBreakingImpulseThreshold(10 * totalMass);]
  btVector3 xzz(0, 0, 0);
  p2p->setAngularLowerLimit(xzz);
  p2p->setAngularUpperLimit(xzz);
  p2p->setLinearLowerLimit(xzz);
  p2p->setLinearUpperLimit(xzz);
  for (int i = 0; i < 6; i++) {
    p2p->setStiffness(i, 1.0);
    p2p->setDamping(i, 1.0);
    p2p->setBounce(i, 0.0);
    p2p->setLimit(i, 0.0, 0.0);
    p2p->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
    p2p->setParam(BT_CONSTRAINT_STOP_ERP, 0.2, i);
  }

  getDM()->getWorld()->getPhysicsWorld()->getWorld()->addConstraint(p2p, true);
}

INSTANCE_CTOR(JointService, Service) { setName("JointService"); };

void JointService::buildJoints(ModelInstance* model) {
  getDM()
      ->getWorld()
      ->getPhysicsWorld()
      ->getWorld()
      ->performDiscreteCollisionDetection();
  btDispatcher* dp =
      getDM()->getWorld()->getPhysicsWorld()->getWorld()->getDispatcher();
  int numManifolds = dp->getNumManifolds();
  rdm::Log::printf(rdm::LOG_DEBUG, "%i manifolds", numManifolds);
  int skipped = 0;

  for (int i = 0; i < numManifolds; i++) {
    if (i % (numManifolds / 4) == 0) {
      rdm::Log::printf(rdm::LOG_DEBUG, "%f%% done",
                       ((float)i / (float)numManifolds) * 100.f);
    }

    btPersistentManifold* man = dp->getManifoldByIndexInternal(i);

    const btRigidBody* obA = static_cast<const btRigidBody*>(man->getBody0());
    const btRigidBody* obB = static_cast<const btRigidBody*>(man->getBody1());

    if (!obA->checkCollideWith(obB)) {
      skipped++;
      continue;
    }

    BlockInstance* ba = (BlockInstance*)obA->getUserPointer();
    BlockInstance* bb = (BlockInstance*)obB->getUserPointer();

    if (!ba->isDescendantOf(model) || !bb->isDescendantOf(model)) {
      skipped++;
      continue;
    }

    int numContacts = man->getNumContacts();
    for (int j = 0; j < numContacts; j++) {
      btManifoldPoint& pt = man->getContactPoint(j);
      if (pt.getDistance() < 0.0f) {
        rdm::Log::printf(rdm::LOG_DEBUG, "%s -> %s %f (%f, %f, %f)",
                         ba->getName().c_str(), bb->getName().c_str(),
                         pt.getDistance(), pt.m_normalWorldOnB.x(),
                         pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());

        const btVector3& ptA = pt.getPositionWorldOnA();
        const btVector3& ptB = pt.getPositionWorldOnB();
        WeldInstance* weld = createChild<WeldInstance>();
        weld->setA(ba);
        weld->setPointA(ptA);
        weld->setB(bb);
        weld->setPointB(ptB);
        weld->link();
      }
    }
  }

  rdm::Log::printf(rdm::LOG_DEBUG, "%i skipped manifolds", skipped);
}
};  // namespace freeblock
