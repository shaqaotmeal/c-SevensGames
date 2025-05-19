#pragma once

#include "Generic_game_parser.hpp"
#include <string>

namespace sevens {

/**
 * Sets up initial table for Sevens:
 * only 7♦ (suit=1, rank=7) is on the table.
 */
class MyGameParser : public Generic_game_parser {
public:
    // No-op to satisfy Generic_card_parser base
    void read_cards(const std::string& /*filename*/) override { }

    // Initialize table_layout[suit][rank]
    void read_game(const std::string& /*filename*/) override;
};

} // namespace sevens
