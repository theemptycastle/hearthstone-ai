#pragma once

#include "state/Types.h"
#include "state/Cards/Manager.h"
#include "state/board/Board.h"
#include "state/Events/Manager.h"
#include "state/IRandomGenerator.h"
#include "state/ZoneChanger.h"

namespace state
{
	class State
	{
	public:
		State() : turn_(0) {}

		board::Board const& GetBoard() const { return board_; }
		board::Board & GetBoard() { return board_; }

		Cards::Manager const& GetCardsManager() const { return cards_mgr_; }

		Events::Manager const& GetEventsManager() const { return event_mgr_; }

		PlayerIdentifier const& GetCurrentPlayerId() const { return current_player_; }
		PlayerIdentifier & GetMutableCurrentPlayerId() { return current_player_; }
		board::Player & GetCurrentPlayer() { return board_.Get(current_player_); }
		const board::Player & GetCurrentPlayer() const { return board_.Get(current_player_); }
		board::Player & GetOppositePlayer() { return board_.Get(current_player_.Opposite()); }
		const board::Player & GetOppositePlayer() const { return board_.Get(current_player_.Opposite()); }

		int GetTurn() const { return turn_; }
		void SetTurn(int turn) { turn = turn_; }
		void IncreaseTurn() { ++turn_; }

	public: // bridge to cards manager
		Cards::Card const& GetCard(CardRef ref) const { return cards_mgr_.Get(ref); }
		Cards::Card & GetMutableCard(CardRef ref) { return cards_mgr_.GetMutable(ref); }
		CardRef AddCard(Cards::Card&& card) { return cards_mgr_.PushBack(std::move(card)); }

	public: // bridge to event manager
		template <typename EventType, typename T>
		void AddEvent(T&& handler) {
			return event_mgr_.PushBack<EventType, T>(std::forward<T>(handler));
		}
		template <typename EventType, typename T>
		void AddEvent(CardRef card_ref, T&& handler) {
			return event_mgr_.PushBack<EventType, T>(card_ref, std::forward<T>(handler));
		}

		template <typename EventTriggerType, typename... Args>
		void TriggerEvent(Args&&... args) {
			return event_mgr_.TriggerEvent<EventTriggerType, Args...>(std::forward<Args>(args)...);
		}
		template <typename EventTriggerType, typename... Args>
		void TriggerCategorizedEvent(CardRef card_ref, Args&&... args) {
			return event_mgr_.TriggerCategorizedEvent<EventTriggerType, Args...>(card_ref, std::forward<Args>(args)...);
		}

	public: // zone changer
		ZoneChangerWithUnknownZoneUnknownType GetZoneChanger(IRandomGenerator& random, CardRef ref) {
			return ZoneChangerWithUnknownZoneUnknownType(board_, cards_mgr_, random, ref, cards_mgr_.GetMutable(ref));
		}

		template <state::CardZone KnownZone>
		ZoneChangerWithUnknownType<KnownZone> GetZoneChanger(IRandomGenerator& random,  CardRef ref) {
			return ZoneChangerWithUnknownType<KnownZone>(board_, cards_mgr_, random, ref, cards_mgr_.GetMutable(ref));
		}

		template <state::CardType KnownType>
		ZoneChangerWithUnknownZone<KnownType> GetZoneChanger(IRandomGenerator& random,  CardRef ref) {
			return ZoneChangerWithUnknownZone<KnownType>(board_, cards_mgr_, random, ref, cards_mgr_.GetMutable(ref));
		}

		template <state::CardType KnownType, state::CardZone KnownZone>
		ZoneChanger<KnownZone, KnownType> GetZoneChanger(IRandomGenerator& random,  CardRef ref) {
			return ZoneChanger<KnownZone, KnownType>(board_, cards_mgr_, random, ref, cards_mgr_.GetMutable(ref));
		}

		template <state::CardZone KnownZone, state::CardType KnownType>
		ZoneChanger<KnownZone, KnownType> GetZoneChanger(IRandomGenerator& random,  CardRef ref) {
			return ZoneChanger<KnownZone, KnownType>(board_, cards_mgr_, random, ref, cards_mgr_.GetMutable(ref));
		}

	private:
		board::Board board_;
		Cards::Manager cards_mgr_;
		Events::Manager event_mgr_;

		PlayerIdentifier current_player_;
		int turn_;
	};
}