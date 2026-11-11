#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../PlayPhysics.hpp"

using namespace geode::prelude;

class $modify(PPPlayLayer, PlayLayer) {
struct Fields {
PlayPhysics physics;
};

bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
if(!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

m_fields->physics.initWorld();
m_fields->physics.buildBodies(m_objects);

// Diagnostic popup. Remove once the mod works.
auto msg = fmt::format(
"World created: {}\nSolids found: {}\nBodies created: {}",
m_fields->physics.m_world != nullptr,
m_fields->physics.m_solidsFound,
m_fields->physics.m_bodiesCreated
);
FLAlertLayer::create("Play Physics", msg, "OK")->show();
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
