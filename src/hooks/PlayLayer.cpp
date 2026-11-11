#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../PlayPhysics.hpp"

using namespace geode::prelude;

class $modify(PPPlayLayer, PlayLayer) {
	struct Fields {
		PlayPhysics physics;
		bool enabled = false;
		CCLabelBMFont* label = nullptr;
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if(!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

		auto winSize = CCDirector::get()->getWinSize();

		auto btn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Fisica"),
			this,
			menu_selector(PPPlayLayer::onTogglePhysics)
		);
		auto menu = CCMenu::create();
		menu->addChild(btn);
		menu->setPosition({70.f, winSize.height - 40.f});
		m_uiLayer->addChild(menu, 100);

		m_fields->label = CCLabelBMFont::create("Fisica: OFF", "bigFont.fnt");
		m_fields->label->setScale(0.4f);
		m_fields->label->setAnchorPoint({0.f, 0.5f});
		m_fields->label->setPosition({10.f, winSize.height - 75.f});
		m_uiLayer->addChild(m_fields->label, 100);

		return true;
	}

	void onTogglePhysics(CCObject*) {
		auto& p = m_fields->physics;
		if(m_fields->enabled) {
			p.restoreOrigins();
			p.cleanWorld();
			m_fields->enabled = false;
		} else {
			p.cleanWorld();
			p.initWorld();
			p.buildBodies(m_objects);
			m_fields->enabled = true;
		}
		updatePPLabel();
	}

	void updatePPLabel() {
		if(!m_fields->label) return;
		auto& p = m_fields->physics;
		std::string text;
		if(m_fields->enabled) {
			text = fmt::format("ON | corpos {}/{} | acordados {} | queda max {:.0f} | resetados {}",
				p.m_bodiesCreated, p.m_solidsFound, p.m_stats.awake, p.m_stats.maxDrop, p.m_stats.overridden);
		} else {
			text = "Fisica: OFF";
		}
		m_fields->label->setString(text.c_str());
	}

	void update(float dt) {
		PlayLayer::update(dt);
		if(m_fields->enabled) {
			m_fields->physics.step(dt);
		}
		updatePPLabel();
	}

	void resetLevel() {
		if(m_fields->enabled) {
			m_fields->physics.restoreOrigins();
			m_fields->physics.cleanWorld();
			m_fields->enabled = false;
		}
		PlayLayer::resetLevel();
	}

	void onQuit() {
		m_fields->physics.cleanWorld();
		PlayLayer::onQuit();
	}
};
