#pragma once

#include "Generic_card_parser.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace sevens
{

/**
 * Interface every strategy must implement.
 * We keep the original camelCase method names so legacy code builds.
 */
class PlayerStrategy
{
public:
    virtual ~PlayerStrategy() = default;

    // life-cycle --------------------------------------------------------------
    virtual void initialize(std::uint64_t playerID) = 0;

    // choose a card -----------------------------------------------------------
    /// return index in hand to play, or -1 to pass
    virtual int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<std::uint64_t,
              std::unordered_map<std::uint64_t,bool>>& tableLayout) = 0;

    // notifications -----------------------------------------------------------
    virtual void observeMove (std::uint64_t playerID, const Card& playedCard) = 0;
    virtual void observePass (std::uint64_t playerID)                        = 0;

    // meta -------------------------------------------------------------------
    virtual std::string getName() const = 0;
};

// C-ABI factory signature looked up by StrategyLoader
using CreateStrategyFn = PlayerStrategy*();

} // namespace sevens
