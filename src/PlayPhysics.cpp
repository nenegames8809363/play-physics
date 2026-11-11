#include "PlayPhysics.hpp"
#include <cmath>
#include <algorithm>

void PlayPhysics::initWorld() {
	b2Vec2 gravity(0.0f, -m_settings.gravity);
	m_world = new b2World(gravity);
	s_active = this;

	b2BodyDef groundDef;
	b2Body* ground = m_world->CreateBody(&groundDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(99999.0f, 1.0f, b2Vec2(0.0f, groundHeight - 1.0f), 0.0f);
	b2FixtureDef groundFixture;
	groundFixture.shape = &groundBox;
	groundFixture.friction = 0.5f;
	ground->CreateFixture(&groundFixture);
}

void PlayPhysics::cleanWorld() {
	if(s_active == this) s_active = nullptr;
	if(m_world) {
		delete m_world;
		m_world = nullptr;
	}
	m_dynamicBodies.clear();
	m_dynamicMap.clear();
	m_origins.clear();
	m_lastWritten.clear();
	m_stats = Stats{};
	m_timeAccumulator = 0.0f;
}

void PlayPhysics::buildBodies(CCArray* objects) {
	m_solidsFound = 0;
	m_bodiesCreated = 0;
	if(!m_world || !objects) return;

	for(auto* obj : CCArrayExt<GameObject*>(objects)) {
		if(!obj || obj->getType() != GameObjectType::Solid) continue;
		m_solidsFound++;

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
		m_origins.push_back({obj, obj->getPosition(), obj->getRotation()});
		m_lastWritten.push_back(obj->getPosition());
		m_bodiesCreated++;
	}
}

void PlayPhysics::restoreOrigins() {
	for(auto& o : m_origins) {
		if(!o.obj) continue;
		o.obj->setPosition(o.pos);
		o.obj->setRotation(o.rot);
	}
}

void PlayPhysics::step(float dt) {
	if(!m_world || m_dynamicBodies.empty()) return;

	// Before stepping: did anything else move our objects since the last write?
	int overridden = 0;
	for(size_t i = 0; i < m_dynamicBodies.size(); ++i) {
		auto* obj = reinterpret_cast<GameObject*>(m_dynamicBodies[i]->GetUserData().pointer);
		if(!obj) continue;
		auto p = obj->getPosition();
		if(std::abs(p.x - m_lastWritten[i].x) > 0.5f || std::abs(p.y - m_lastWritten[i].y) > 0.5f) overridden++;
	}
	m_stats.overridden = overridden;

	bool stepped = false;
	m_timeAccumulator += dt;
	while(m_timeAccumulator >= m_settings.timeStep) {
		m_world->Step(m_settings.timeStep, 8, 3);
		m_timeAccumulator -= m_settings.timeStep;
		stepped = true;
	}
	if(!stepped) return;

	int awake = 0;
	float maxDrop = 0.f;
	for(size_t i = 0; i < m_dynamicBodies.size(); ++i) {
		auto* body = m_dynamicBodies[i];
		auto* obj = reinterpret_cast<GameObject*>(body->GetUserData().pointer);
		if(!obj) continue;

		if(body->IsAwake()) awake++;

		auto pos = body->GetPosition();
		CCPoint newPos = {pos.x * PTM_RATIO, pos.y * PTM_RATIO};
		maxDrop = std::max(maxDrop, m_origins[i].pos.y - newPos.y);

		obj->setPosition(newPos);
		obj->setRotation(-body->GetAngle() * (180.0f / b2_pi));
		m_lastWritten[i] = newPos;
	}
	m_stats.awake = awake;
	m_stats.maxDrop = maxDrop;
}
