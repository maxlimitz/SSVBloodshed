// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_BARCOUNTER
#define SSVOB_BARCOUNTER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"

namespace ob
{
	class OBBarCounter : public sf::Transformable, public sf::Drawable
	{
		private:
			float barWidth, barHeight;
			unsigned int barCount;
			float value, minValue, maxValue;
			sf::VertexArray vertices{sf::PrimitiveType::Quads};
			sf::FloatRect bounds;
			sf::Color color{sf::Color::White};
			int tracking{0};
			bool mustRefreshGeometry{true}, mustRefreshColor{true};

			void refreshGeometry()
			{
				unsigned int iX{0}, iY{0};
				float xMin{0}, xMax{0}, yMin{0}, yMax{0};

				vertices.clear();

				float dist{maxValue - minValue};
				float barValue{dist / barCount};
				unsigned int barsToDraw(barValue == 0 ? 0 : std::floor(value / barValue));

				for(auto i(0u); i < barsToDraw; ++i)
				{
					unsigned int spacing{tracking * iX};

					float gLeft{iX * barWidth + spacing};			if(xMin > gLeft) xMin = gLeft;
					float gRight{(iX + 1) * barWidth + spacing};	if(xMax < gRight) xMax = gRight;
					float gTop{iY * barHeight};						if(yMin > gTop) yMin = gTop;
					float gBottom{(iY + 1) * barHeight};			if(yMax < gBottom) yMax = gBottom;

					vertices.append({{gLeft,	gTop},		color});
					vertices.append({{gRight,	gTop},		color});
					vertices.append({{gRight,	gBottom},	color});
					vertices.append({{gLeft,	gBottom},	color});

					++iX;
				}

				bounds = {xMin, yMin, xMax - xMin, yMax - yMin};
				mustRefreshGeometry = false;
			}
			void refreshColor() { for(auto i(0u); i < vertices.getVertexCount(); ++i) vertices[i].color = color; mustRefreshColor = false; }
			void refresh() const
			{
				if(mustRefreshGeometry) const_cast<OBBarCounter*>(this)->refreshGeometry();
				if(mustRefreshColor) const_cast<OBBarCounter*>(this)->refreshColor();
			}

		public:
			inline OBBarCounter(float mBarWidth, float mBarHeight, unsigned int mBarCount, float mValue = 0.f, float mMinValue = 0.f, float mMaxValue = 0.f)
				: barWidth{mBarWidth}, barHeight{mBarHeight}, barCount{mBarCount}, value{mValue}, minValue{mMinValue}, maxValue{mMaxValue} { }

			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override
			{
				refresh();
				mRenderStates.transform *= getTransform();
				mRenderTarget.draw(vertices, mRenderStates);
			}

			inline void setValue(float mValue)		{ if(value != mValue) { value = mValue; mustRefreshGeometry = true; } }
			inline void setMinValue(float mValue)	{ if(minValue != mValue) { minValue = mValue; mustRefreshGeometry = true; } }
			inline void setMaxValue(float mValue)	{ if(maxValue != mValue) { maxValue = mValue; mustRefreshGeometry = true; } }
			inline void setColor(sf::Color mColor)	{ color = std::move(mColor); mustRefreshColor = true; }
			inline void setTracking(int mTracking)	{ tracking = mTracking; mustRefreshGeometry = true; }

			inline float getValue() const noexcept					{ return value; }
			inline float getMaxValue() const noexcept				{ return maxValue; }
			inline float getMinValue() const noexcept				{ return minValue; }
			inline const sf::Color& getColor() const noexcept		{ return color; }
			inline int getTracking() const noexcept					{ return tracking; }
			inline sf::FloatRect getLocalBounds() const				{ refresh(); return bounds; }
			inline sf::FloatRect getGlobalBounds() const			{ return getTransform().transformRect(getLocalBounds()); }
	};
}

#endif