// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PPLATE
#define SSVOB_COMPONENTS_PPLATE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBWeightable.h"

namespace ob
{
	class OBCPPlate : public OBCActorBase, public OBWeightable
	{
		private:
			int id;
			PPlateType type;
			OBIdAction idAction;
			bool triggered{false};

			inline void activate()
			{
				for(auto& e : manager.getEntities(OBGroup::GIdReceiver))
				{
					auto& c(e->getComponent<OBCIdReceiver>());
					if(c.getId() == id) c.activate(idAction);
				}
			}

			inline void triggerNeighbors(bool mTrigger)
			{
				auto query(cPhys.getWorld().getQuery<ssvsc::QueryType::Distance>(cPhys.getPosI(), 1000));

				Body* body;
				while((body = query.next()) != nullptr)
				{
					if(body->hasGroup(OBGroup::GPPlate))
					{
						auto& cPPlate(getEntityFromBody(*body).getComponent<OBCPPlate>());
						if(cPPlate.cPhys.getPosI().x == cPhys.getPosI().x || cPPlate.cPhys.getPosI().y == cPhys.getPosI().y)
							if(cPPlate.id == id && cPPlate.type == type)
							{
								if(mTrigger) cPPlate.trigger(); else cPPlate.unTrigger();
							}
					}
				}
			}

			inline void trigger()	{ if(!triggered) { triggered = true; triggerNeighbors(true); } }
			inline void unTrigger()	{ if(triggered) { triggered = false; triggerNeighbors(false); } }

		public:
			OBCPPlate(OBCPhys& mCPhys, OBCDraw& mCDraw, int mId, PPlateType mType, OBIdAction mIdAction, bool mPlayerOnly) noexcept
				: OBCActorBase{mCPhys, mCDraw}, OBWeightable{mCPhys, mPlayerOnly}, id{mId}, type{mType}, idAction{mIdAction} { }

			inline void init() override
			{
				if(type == PPlateType::Single) triggered = false;
				OBWeightable::init();
			}
			inline void update(float) override
			{
				if(hasBeenWeighted() && !triggered)
				{
					if(type == PPlateType::Single || type == PPlateType::Multi) { trigger(); activate(); }
				}

				if(hasBeenUnweighted())
				{
					if(type == PPlateType::Multi) unTrigger();
				}

				OBWeightable::refresh();
			}
			inline void draw() override { cDraw[0].setColor(triggered ? sf::Color(100, 100, 100, 255) : sf::Color::White); }

			inline void setId(int mId) noexcept	{ id = mId; }
			inline int getId() const noexcept	{ return id; }
	};
}

#endif

