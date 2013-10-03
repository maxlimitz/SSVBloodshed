// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PROJECTILE
#define SSVOB_COMPONENTS_PROJECTILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCFloor.h"
#include "SSVBloodshed/Components/OBCHealth.h"

namespace ob
{
	class OBCProjectile : public OBCActorBase
	{
		private:
			ssvs::Ticker tckLife{150.f};
			float speed{125.f}, degrees{0.f}, curveSpeed{0.f}, damage{1};
			int pierceOrganic{0};
			OBGroup targetGroup{OBGroup::GEnemy};

			inline void destroy() { getEntity().destroy(); onDestroy(); }

		public:
			ssvu::Delegate<void()> onDestroy;

			OBCProjectile(OBCPhys& mCPhys, OBCDraw& mCDraw, float mSpeed, float mDegrees) : OBCActorBase{mCPhys, mCDraw}, speed{mSpeed}, degrees{mDegrees} { }

			inline void init() override
			{
				body.addGroup(OBGroup::GProjectile);
				body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GSolidAir);
				body.setResolve(false);
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!mDI.body.hasGroup(targetGroup))
					{
						if(!mDI.body.hasGroup(OBGroup::GOrganic) && mDI.body.hasGroup(OBGroup::GSolidAir))
						{
							game.createPDebris(6, cPhys.getPosPixels());
							assets.playSound("Sounds/bulletHitWall.wav");
							destroy();
						}
					}
					else if(getEntityFromBody(mDI.body).getComponent<OBCHealth>().damage(damage) && pierceOrganic-- == 0) destroy();
				};
			}
			inline void update(float mFT) override
			{
				degrees += curveSpeed * mFT;
				body.setVelocity(ssvs::getVecFromDeg(degrees, speed));
				if(tckLife.update(mFT)) destroy();
			}
			inline void draw() override { cDraw.setRotation(degrees); }

			inline OBCProjectile& createChild(Entity& mEntity)
			{
				auto& pj(mEntity.getComponent<OBCProjectile>());
				pj.setTargetGroup(targetGroup);
				return pj;
			}

			inline void setLife(float mValue) noexcept			{ tckLife.restart(mValue); }
			inline void setSpeed(float mValue) noexcept			{ speed = mValue; }
			inline void setDegrees(float mValue) noexcept		{ degrees = mValue; }
			inline void setCurveSpeed(float mValue) noexcept	{ curveSpeed = mValue; }
			inline void setDamage(float mValue) noexcept		{ damage = mValue; }
			inline void setPierceOrganic(int mValue) noexcept	{ pierceOrganic = mValue; }
			inline void setTargetGroup(OBGroup mValue) noexcept	{ targetGroup = mValue; }

			inline float getLife() const noexcept				{ return tckLife.getCurrent(); }
			inline float getSpeed() const noexcept				{ return speed; }
			inline float getDeg() const noexcept				{ return degrees; }
			inline float getCurveSpeed() const noexcept			{ return curveSpeed; }
			inline float getDamage() const noexcept				{ return damage; }
			inline int getPierceOrganic() const noexcept		{ return pierceOrganic; }
			inline OBGroup getTargetGroup() const noexcept		{ return targetGroup; }
	};
}

#endif
