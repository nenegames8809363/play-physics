#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>

#include "../PlayPhysics.hpp"

using namespace geode::prelude;

// Stops GD from deactivating solids the physics world is moving.
class $modify(PPGameObject, GameObject) {
	void deactivateObject(bool deactivate) {
		if(PlayPhysics::isSimulatedAnywhere(this)) return;
		GameObject::deactivateObject(deactivate);
	}
};
