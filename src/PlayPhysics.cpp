#include "PlayPhysics.hpp"
#include <cmath>

void PlayPhysics::initWorld() {
	b2Vec2 gravity(0.0f, -m_settings.gravity);
	m_world = new b2World(gravity);
	s_active = this;
}

void PlayPhysics::cleanWorld() {
	if(s_active == this) s_active = nullptr;
	if(m_world) {
		delete m_world;
		m_world = nullptr;
	}
	m_dynamicBodies.clear();
	m_dynamicMap.clear();
}

// Creates one dynamic box body per solid block.
// Blocks at or below floor height are skipped, same rule as the editor version,
// so the level floor doesn't fall away.
void PlayPhysics::buildBodies(CCArray* objects) {
	if(!m_world || !objects) return;

	for(auto* obj : CCArrayExt<GameObject*>(objects)) {
		if(!obj || obj->getType() != GameObjectType::Solid) continue;

		auto position = obj->getPosition() / PTM_RATIO;
		if(position.y <= groundHeight) continue;

		auto contentSize = obj->getScaledContentSize() / PTM_RATIO;
		float width = std::abs(contentSize.width);
		float height = std::abs(contentSize.height);

		b2PolygonShape box;
		box.SetAsBox(width / 2.0f, height / 2.0f);

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		bodyDef.angle = -obj->getRotation() * (b2_pi / 180.0f);
		bodyDef.linearDamping = m_settings.linearDamping;
		bodyDef.angularDamping = m_settings.angularDamping;
		bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(obj);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &box;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = m_settings.friction;
		fixtureDef.restitution = m_settings.restitution;

		b2Body* body = m_world->CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		m_dynamicBodies.push_back(body);
		m_dynamicMap[obj] = body;
	}
}

void PlayPhysics::step(float dt) {
	if(!m_world || m_dynamicBodies.empty()) return;

	bool stepped = false;
	m_timeAccumulator += dt;
	while(m_timeAccumulator >= m_settings.timeStep) {
		m_world->Step(m_settings.timeStep, 8, 3);
		m_timeAccumulator -= m_settings.timeStep;
		stepped = true;
	}
	if(!stepped) return;

	for(auto* body : m_dynamicBodies) {
		auto* obj = reinterpret_cast<GameObject*>(body->GetUserData().pointer);
		if(!obj) continue;

		auto pos = body->GetPosition();
		obj->setPosition({pos.x * PTM_RATIO, pos.y * PTM_RATIO});
		obj->setRotation(-body->GetAngle() * (180.0f / b2_pi));
	}
}
