// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvsc;
using namespace ssvsc::Utils;
using namespace sses;

namespace ob
{
	OBGame::OBGame(GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets), factory{assets, *this, manager, world},
		world(createResolver<Impulse>(), createSpatial<HashGrid>(1000, 1000, 1000, 500)),  debugText{assets.get<BitmapFont>("limeStroked")}
	{
		gameState.onUpdate += [this](float mFrameTime){ update(mFrameTime); };
		gameState.onDraw += [this]{ draw(); };

		// Debug
		debugText.setTracking(-3);

		// Input
		using k = Keyboard::Key;
		using b = Mouse::Button;
		using t = Input::Trigger::Type;

		gameState.addInput({{k::Escape}}, [this](float){ assets.musicPlayer.stop(); std::terminate(); });

		gameState.addInput({{k::A}}, [=](float){ camera.pan(-4, 0); });
		gameState.addInput({{k::D}}, [=](float){ camera.pan(4, 0); });
		gameState.addInput({{k::W}}, [=](float){ camera.pan(0, -4); });
		gameState.addInput({{k::S}}, [=](float){ camera.pan(0, 4); });
		gameState.addInput({{k::Q}}, [=](float){ camera.zoomOut(1.1f); });
		gameState.addInput({{k::E}}, [=](float){ camera.zoomIn(1.1f); });

		add2StateInput(gameState, {{k::Z}}, inputShoot);
		add2StateInput(gameState, {{k::X}}, inputSwitch, t::Once);
		add2StateInput(gameState, {{k::Space}}, inputBomb, t::Once);

		add3StateInput(gameState, {{k::Left}}, {{k::Right}}, inputX);
		add3StateInput(gameState, {{k::Up}}, {{k::Down}}, inputY);

		gameState.addInput({{k::R}}, [this](float){ newGame(); }, t::Once);

		gameState.addInput({{k::Num1}}, [this](float){ factory.createWall(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num2}}, [this](float){ factory.createTestEnemy(getMousePosition()); });
		gameState.addInput({{k::Num3}}, [this](float){ factory.createTestEnemyBig(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num4}}, [this](float){ factory.createTestJuggernaut(getMousePosition()); }, t::Once);
		gameState.addInput({{k::Num5}}, [this](float){ factory.createTestGiant(getMousePosition()); }, t::Once);

		// Particle textures
		psPermTexture.create(320, 240);
		psTempTexture.create(320, 240);

		// Temp hud sprite
		hudSprite.setScale(2.f, 2.f);
		hudSprite.setPosition(0, 480 - getGlobalHeight(hudSprite));

		newGame();
	}

	void OBGame::newGame()
	{
		manager.clear();
		world.clear();

		psPerm = &factory.createParticleSystem(psPermTexture, false, 175).getComponent<OBCParticleSystem>().getParticleSystem();
		psTemp = &factory.createParticleSystem(psTempTexture, true, 255).getComponent<OBCParticleSystem>().getParticleSystem();

		auto getTilePos = [](int mX, int mY) -> Vec2i { return toCoords(Vec2i{mX * 10 + 5, mY * 10 + 5}); };
		constexpr int maxX{320 / 10}, maxY{240 / 10 - 2};

		for(int iX{0}; iX < maxX; ++iX)
			for(int iY{0}; iY < maxY; ++iY)
			{
				if(iX == 0 || iX == maxX - 1 || iY == 0 || iY == maxY - 1) factory.createWall(getTilePos(iX, iY));
				else factory.createFloor(getTilePos(iX, iY));
			}

		factory.createPlayer(getTilePos(5, 5));
	}

	void OBGame::update(float mFrameTime)
	{
		manager.update(mFrameTime);
		world.update(mFrameTime);

		updateDebugText(mFrameTime);
		camera.update<int>(mFrameTime);
	}
	void OBGame::updateDebugText(float mFrameTime)
	{
		ostringstream s;
		const auto& entities(manager.getEntities());
		const auto& bodies(world.getBodies());
		std::size_t componentCount{0}, dynamicBodiesCount{0};
		for(const auto& e : entities) componentCount += e->getComponents().size();
		for(const auto& b : bodies) if(!b->isStatic()) ++dynamicBodiesCount;

		s << "FPS: "				<< gameWindow.getFPS() << "\n";
		s << "FrameTime: "			<< mFrameTime << "\n";
		s << "Bodies(all): "		<< bodies.size() << "\n";
		s << "Bodies(static): "		<< bodies.size() - dynamicBodiesCount << "\n";
		s << "Bodies(dynamic): "	<< dynamicBodiesCount << "\n";
		s << "Sensors: "			<< world.getSensors().size() << "\n";
		s << "Entities: "			<< entities.size() << "\n";
		s << "Components: "			<< componentCount << endl;

		debugText.setString(s.str());
	}

	void OBGame::draw()
	{
		camera.apply<int>();
		manager.draw();
		camera.unapply();
		render(hudSprite);
		render(debugText);
	}

	void OBGame::createPBlood(unsigned int mCount, const Vec2f& mPosition, float mMult)	{ for(auto i(0u); i < mCount; ++i) ob::createPBlood(*psPerm, mPosition, mMult); }
	void OBGame::createPGib(unsigned int mCount, const Vec2f& mPosition)				{ for(auto i(0u); i < mCount; ++i) ob::createPGib(*psTemp, mPosition); }
	void OBGame::createPDebris(unsigned int mCount, const Vec2f& mPosition)				{ for(auto i(0u); i < mCount; ++i) ob::createPDebris(*psTemp, mPosition); }
	void OBGame::createPDebrisFloor(unsigned int mCount, const Vec2f& mPosition)		{ for(auto i(0u); i < mCount; ++i) ob::createPDebrisFloor(*psTemp, mPosition); }
	void OBGame::createPMuzzle(unsigned int mCount, const Vec2f& mPosition)				{ for(auto i(0u); i < mCount; ++i) ob::createPMuzzle(*psTemp, mPosition); }
	void OBGame::createPPlasma(unsigned int mCount, const Vec2f& mPosition)				{ for(auto i(0u); i < mCount; ++i) ob::createPPlasma(*psTemp, mPosition); }
	void OBGame::createPSmoke(unsigned int mCount, const Vec2f& mPosition)				{ for(auto i(0u); i < mCount; ++i) ob::createPSmoke(*psTemp, mPosition); }
}
