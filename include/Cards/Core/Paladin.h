#pragma once

namespace Cards
{
	struct Card_EX1_360e : Enchantment<SetAttack<1>> {
		constexpr static FlowControl::enchantment::EnchantmentTiers tier = FlowControl::enchantment::kEnchantmentTier1;
	};
	struct Card_EX1_360 : SpellCardBase<Card_EX1_360> {
		Card_EX1_360() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator().Minion().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.Minion(target).Enchant().Add<Card_EX1_360e>();
			});
		}
	};
}

REGISTER_CARD(EX1_360)