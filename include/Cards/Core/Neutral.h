#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardBase.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/EnchantmentCardBase.h"

namespace Cards
{
	class Card_CS2_189 : public MinionCardBase<Card_CS2_189>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_189;

		Card_CS2_189()
		{
			battlecry = [](auto& context) {
				state::CardRef ref = context.GetBattleCryTarget(Targets().Targetable().Enemy(context));
				Damage(context).Target(ref).Amount(1);
			};
		}
	};

	class Card_CS1_042 : public MinionCardBase<Card_CS1_042>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS1_042;

		Card_CS1_042()
		{
			Taunt();
		}
	};

	class Card_CS2_168 : public MinionCardBase<Card_CS2_168>
	{
	public:
		static constexpr int id = Cards::ID_CS2_168;

		Card_CS2_168() {}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_189)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS1_042)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_168)
