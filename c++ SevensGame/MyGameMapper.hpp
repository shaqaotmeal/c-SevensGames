#pragma once

#include "Generic_game_mapper.hpp"
#include "MyCardParser.hpp"
#include "MyGameParser.hpp"
#include "PlayerStrategy.hpp"

#include <memory>
#include <vector>
#include <unordered_map>
#include <random>

namespace sevens {

class MyGameMapper : public Generic_game_mapper {
public:
    MyGameMapper();

    void read_cards(const std::string& filename) override;
    void read_game(const std::string& filename) override;

    // (these are not in the base, so drop 'override')
    bool hasRegisteredStrategies() const;
    void registerStrategy(uint64_t playerID, std::shared_ptr<PlayerStrategy> strategy);

    std::vector<std::pair<uint64_t, uint64_t>>
    compute_game_progress(uint64_t numPlayers) override;

    std::vector<std::pair<uint64_t, uint64_t>>
    compute_and_display_game(uint64_t numPlayers) override;

private:
    MyCardParser                card_parser;
    MyGameParser                game_parser;
    std::unordered_map<uint64_t, std::shared_ptr<PlayerStrategy>> strategies;
    std::unordered_map<uint64_t, uint64_t> score_board;
    std::mt19937                rng;
};

} // namespace sevens
