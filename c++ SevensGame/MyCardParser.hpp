#pragma once

#include "Generic_card_parser.hpp"
#include <string>

namespace sevens {

/**
 * Generates a standard 52-card deck.
 */
class MyCardParser : public Generic_card_parser {
public:
    MyCardParser() = default;
    ~MyCardParser() = default;
    // Implements Generic_card_parser::read_cards
    void read_cards(const std::string& /*filename*/) override;
};

} // namespace sevens
