// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_CHOICESHUTTER
#define SSVOB_GUI_CONTROLS_CHOICESHUTTER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/Label.hpp"
#include "SSVBloodshed/GUI/Controls/Button.hpp"
#include "SSVBloodshed/GUI/Controls/Strip.hpp"
#include "SSVBloodshed/GUI/Controls/Shutter.hpp"

namespace ob
{
	namespace GUI
	{
		class ChoiceShutter : public Shutter
		{
			private:
				std::vector<std::string> choices;
				int idxOffset{0};

				std::size_t currentChoiceIdx{0}, choiceBtnsMax{5};
				Strip& wsChoices;
				Strip& wsScroll;
				Button& btnUp;
				Button& btnDown;
				Label& lblCount;
				std::vector<Button*> btnsChoices;

				inline void refreshChoices()
				{
					if(choices.empty()) { getLabel().setString("null"); return; }

					const auto& lb(ssvu::getWrapIdx(idxOffset, choices.size()));
					const auto& ub(ssvu::getWrapIdx(idxOffset + choiceBtnsMax, choices.size()));
					lblCount.setString("(" + ssvu::toStr(lb) + ":" + ssvu::toStr(ub) + ")/" + ssvu::toStr(choices.size()));

					for(auto i(0u); i < choiceBtnsMax; ++i)
					{
						const auto& choiceIdx(ssvu::getWrapIdx(i + idxOffset, choices.size()));
						btnsChoices[i]->getLabel().setString(choiceIdx == currentChoiceIdx ? ">" + ssvu::toUpper(choices[choiceIdx]) + "<" : choices[choiceIdx]);
					}

					getLabel().setString(choices[currentChoiceIdx]);
				}

			public:
				ssvu::Delegate<void()> onChoiceSelected;

				ChoiceShutter(Context& mContext, const std::vector<std::string>& mChoices, const Vec2f& mSize)
						: Shutter{mContext, "", mSize}, choices{mChoices},
						wsChoices(getShutter().create<Strip>(At::Top, At::Bottom, At::Bottom)),
						wsScroll(getShutter().create<Strip>(At::Right, At::Left, At::Left)),
						btnUp(wsScroll.create<Button>("^", getStyle().getBtnSquareSize())),
						btnDown(wsScroll.create<Button>("v", getStyle().getBtnSquareSize())),
						lblCount(wsScroll.create<Label>(""))
				{
					btnUp.onLeftClick += [this]{ --idxOffset; refreshChoices(); };
					btnDown.onLeftClick += [this]{ ++idxOffset; refreshChoices(); };

					for(auto i(0u); i < choiceBtnsMax; ++i)
					{
						auto& btn(wsChoices.create<Button>("", getStyle().getBtnSizePerChar(7)));
						btn.onLeftClick += [this, i]
						{
							if(choices.empty()) return;
							setChoiceIdx(ssvu::getWrapIdx(i + idxOffset, choices.size())); onChoiceSelected();
						};
						btnsChoices.emplace_back(&btn);
					}

					refreshChoices();
				}

				inline void addChoice(std::string mStr)	{ choices.emplace_back(std::move(mStr)); refreshChoices(); }
				inline void clearChoices()				{ choices.clear(); refreshChoices(); }
				inline std::size_t getChoiceCount()		{ return choices.size(); }

				inline void setChoiceIdx(std::size_t mIdx)				{ currentChoiceIdx = mIdx; refreshChoices(); }
				inline int getChoiceIdx() const noexcept				{ return currentChoiceIdx; }
				inline const std::string& getChoice() const noexcept	{ return choices[currentChoiceIdx]; }
		};
	}
}

#endif