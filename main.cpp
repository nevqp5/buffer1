#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace {
	struct Pending {
		bool active = false;       // a jump press is being held back
		bool releaseQueued = false; // player let go while it was held back
		int deadline = 0;          // step at which we give up waiting
	};

	struct State {
		Pending pend[2];           // [0] = player 1, [1] = player 2
		bool releaseNext[2] = {false, false};
		int step = 0;
		bool passthrough = false;  // true while WE call the original handleButton
	} g;

	bool isCubeLike(PlayerObject* p) {
		return !(p->m_isShip || p->m_isBall || p->m_isBird || p->m_isDart ||
		         p->m_isRobot || p->m_isSpider || p->m_isSwing);
	}

	// Should a press right now be held back? Only for a cube that is in the air.
	bool shouldDelay(PlayerObject* p) {
		return p && isCubeLike(p) && !p->m_isOnGround;
	}
}

class $modify(BufferLayer, GJBaseGameLayer) {
	// Called once per physics step (240 TPS by default)
	void processCommands(float dt) {
		g.step++;

		for (int i = 0; i < 2; i++) {
			bool p1 = (i == 0);
			auto* player = p1 ? m_player1 : m_player2;

			if (g.releaseNext[i]) {
				g.releaseNext[i] = false;
				g.passthrough = true;
				GJBaseGameLayer::handleButton(false, 1, p1);
				g.passthrough = false;
			}

			auto& p = g.pend[i];
			if (!p.active) continue;
			if (!player) { p = Pending{}; continue; }

			// Fire on the first step the cube is grounded, or at the deadline
			if (!shouldDelay(player) || g.step >= p.deadline) {
				g.passthrough = true;
				GJBaseGameLayer::handleButton(true, 1, p1);
				g.passthrough = false;
				if (p.releaseQueued) g.releaseNext[i] = true;
				p = Pending{};
			}
		}

		GJBaseGameLayer::processCommands(dt);
	}

	void handleButton(bool down, int button, bool isPlayer1) {
		if (g.passthrough || button != 1 ||
		    !Mod::get()->getSettingValue<bool>("enabled")) {
			return GJBaseGameLayer::handleButton(down, button, isPlayer1);
		}

		int i = isPlayer1 ? 0 : 1;
		auto& p = g.pend[i];
		auto* player = isPlayer1 ? m_player1 : m_player2;

		if (down) {
			if (!shouldDelay(player)) {
				return GJBaseGameLayer::handleButton(down, button, isPlayer1);
			}
			int window = static_cast<int>(Mod::get()->getSettingValue<int64_t>("window"));
			p.active = true;
			p.releaseQueued = false;
			p.deadline = g.step + window;
			return; // swallow the early press
		}

		// release
		if (p.active) {
			p.releaseQueued = true; // let go after the delayed press fires
			return;
		}
		GJBaseGameLayer::handleButton(down, button, isPlayer1);
	}
};

class $modify(BufferPlayLayer, PlayLayer) {
	void resetLevel() {
		g = State{};
		PlayLayer::resetLevel();
	}
};
