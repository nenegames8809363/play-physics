#pragma once
#include <Geode/Geode.hpp>
#include <box2d/box2d.h>
#include <unordered_map>
#include <vector>

using namespace geode::prelude;

constexpr float PTM_RATIO = 30.0f;
constexpr float groundHeight = 89.78f / PTM_RATIO;

class PlayPhysics {
public:
	struct Settings {
		float gravity = 12.5f;
		float timeStep = 1.0f / 60.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float linearDamping = 0.3f;
		float angularDamping = 0.5f;
	};
	struct Origin {
		GameObject* obj;
		CCPoint pos;
		float rot;
	};
	struct Stats {
		int awake = 0;        // bodies Box2D still considers moving
		float maxDrop = 0.f;  // biggest fall (px) of any body since start
		int overridden = 0;   // objects whose position was changed by something else
	};

	Settings m_settings;
	Stats m_stats;
	b2World* m_world = nullptr;
	std::vector<b2Body*> m_dynamicBodies;
	std::unordered_map<GameObject*, b2Body*> m_dynamicMap;
	std::vector<Origin> m_origins;        // same order as m_dynamicBodies
	std::vector<CCPoint> m_lastWritten;   // same order as m_dynamicBodies
	float m_timeAccumulator = 0.0f;
	int m_solidsFound = 0;
	int m_bodiesCreated = 0;
	static inline PlayPhysics* s_active = nullptr;

	void initWorld();
	void cleanWorld();
	void buildBodies(CCArray* objects);
	void restoreOrigins();
	void step(float dt);
	bool isObjectSimulated(GameObject* obj) const { return m_dynamicMap.contains(obj); }
	static bool isSimulatedAnywhere(GameObject* obj) { return s_active && s_active->isObjectSimulated(obj); }
};
