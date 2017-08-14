#pragma once

#include <type_traits>
#include <utility>
#include <tuple>

#include "state/Types.h"
#include "state/Events/impl/HandlersContainer.h"
#include "state/Events/impl/CategorizedHandlersContainer.h"
#include "state/Events/EventTypes.h"

namespace state
{
	namespace Events
	{
		template <typename T> class Event;
		template <typename T> class CategorizedEvent;

		class Manager
		{
			template <typename T> friend class Event;
			template <typename T> friend class CategorizedEvent;

		private:
			struct CopyOnWriteHelper {
				template <std::size_t I = 0, typename Functor, typename... Tp>
				static typename std::enable_if<I == sizeof...(Tp), void>::type
					for_index(size_t, std::tuple<Tp...> &, std::tuple<Tp...> const&, Functor)
				{}

				template <std::size_t I = 0, typename Functor, typename... Tp>
				static typename std::enable_if<I < sizeof...(Tp), void>::type
					for_index(size_t index, std::tuple<Tp...> & t1, std::tuple<Tp...> const& t2, Functor f)
				{
					if (index == 0) f(std::get<I>(t1), std::get<I>(t2));
					for_index<I + 1, Functor, Tp...>(index-1, t1, t2, f);
				}

				template <typename Tuple>
				static void CopyOnWrite(Tuple & tuple, Tuple const& base) {
					constexpr size_t tuple_size = std::tuple_size<Tuple>::value;
					for (size_t idx = 0; idx < tuple_size; ++idx) {
						for_index(idx, tuple, base, [](auto & item, auto const& base) {
							item.FillWithBase(base);
						});
					}
				}
			};

		public:
			Manager() : event_trigger_recursive_count_(0), event_tuple_(), categorized_event_tuple_() {}

			void FillWithBase(Manager const& base) {
				event_trigger_recursive_count_ = base.event_trigger_recursive_count_;
				CopyOnWriteHelper::CopyOnWrite(this->event_tuple_, base.event_tuple_);
				CopyOnWriteHelper::CopyOnWrite(this->categorized_event_tuple_, base.categorized_event_tuple_);
			}

			template <typename EventType, typename T>
			void PushBack(T&& handler) {
				GetHandlersContainer<EventType>().PushBack(std::forward<T>(handler));
			}

			template <typename EventType, typename T>
			void PushBack(CardRef card_ref, T&& handler) {
				GetCategorizedHandlersContainer<EventType>().PushBack(
					card_ref, std::forward<T>(handler));
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerEvent(Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) throw std::runtime_error("reach maximum event trigger recursive");
				++event_trigger_recursive_count_;
				GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerCategorizedEvent(CardRef card_ref, Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) throw std::runtime_error("reach maximum event trigger recursive");
				++event_trigger_recursive_count_;
				GetCategorizedHandlersContainer<EventTriggerType>()
					.TriggerAll(card_ref, std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

		private:
			constexpr static int max_event_trigger_recursive_ = 100;
			int event_trigger_recursive_count_;

		private:
			using EvnetTypesTuple = std::tuple<
				impl::HandlersContainer<EventTypes::GetPlayCardCost>,
				impl::HandlersContainer<EventTypes::AfterMinionSummoned>,
				impl::HandlersContainer<EventTypes::BeforeMinionSummoned>,
				impl::HandlersContainer<EventTypes::AfterMinionPlayed>,
				impl::HandlersContainer<EventTypes::AfterMinionDied>,
				impl::HandlersContainer<EventTypes::PreparePlayCardTarget>,
				impl::HandlersContainer<EventTypes::OnPlay>,
				impl::HandlersContainer<EventTypes::CheckPlayCardCountered>,
				impl::HandlersContainer<EventTypes::OnTurnEnd>,
				impl::HandlersContainer<EventTypes::OnTurnStart>,
				impl::HandlersContainer<EventTypes::BeforeSecretReveal>,
				impl::HandlersContainer<EventTypes::PrepareAttackTarget>,
				impl::HandlersContainer<EventTypes::BeforeAttack>,
				impl::HandlersContainer<EventTypes::AfterAttack>,
				impl::HandlersContainer<EventTypes::CalculateHealDamageAmount>,
				impl::HandlersContainer<EventTypes::PrepareHealDamageTarget>,
				impl::HandlersContainer<EventTypes::OnTakeDamage>,
				impl::HandlersContainer<EventTypes::OnHeal>,
				impl::HandlersContainer<EventTypes::AfterTakenDamage>,
				impl::HandlersContainer<EventTypes::AfterHeroPower>,
				impl::HandlersContainer<EventTypes::AfterSpellPlayed>,
				impl::HandlersContainer<EventTypes::AfterSecretPlayed>
			>;
			EvnetTypesTuple event_tuple_;

			using CategorizedEventTypesTuple = std::tuple<
				impl::CategorizedHandlersContainer<EventTypes::CategorizedOnTakeDamage>
			>;
			CategorizedEventTypesTuple categorized_event_tuple_;

			template <typename EventType>
			impl::HandlersContainer<EventType> & GetHandlersContainer() {
				return std::get<impl::HandlersContainer<EventType>>(event_tuple_);
			}

			template<typename EventType>
			impl::CategorizedHandlersContainer<EventType> & GetCategorizedHandlersContainer() {
				return std::get<impl::CategorizedHandlersContainer<EventType>>(categorized_event_tuple_);
			}
		};
	}
}
