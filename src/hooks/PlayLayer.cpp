#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../PlayPhysics.hpp"

using namespace geode::prelude;

// NOTE: check PlayLayer method names/signatures against your Geode SDK version.
class $modify(PPPlayLayer, PlayLayer) {
	struct Fields {
		PlayPhysics physics;
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if(!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

		m_fields->physics.initWorld();
		m_fields->physics.buildBodies(m_objects);
		return true;
	}

	void update(float dt) {
		PlayLayer::update(dt);
		m_fields->physics.step(dt);
	}

	void resetLevel() {
		m_fields->physics.cleanWorld();
		m_fields->physics.initWorld();
		m_fields->physics.buildBodies(m_objects);
		PlayLayer::resetLevel();
	}

	void onQuit() {
		m_fields->physics.cleanWorld();
		PlayLayer::onQuit();
	}
};
