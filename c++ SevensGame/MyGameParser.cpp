#include "MyGameParser.hpp"

namespace sevens {


void MyGameParser::read_game(const std::string&) {// Initialise la table de jeu avec uniquement le 7♦ placé.
    // Réinitialise chaque case de la table
    for (uint64_t suit = 0; suit < 4; ++suit)
        for (uint64_t rank = 1; rank <= 13; ++rank)
            table_layout[suit][rank] = false;

    // Place le 7♦ au centre (suit=1 pour ♦, rank=7)
    table_layout[1][7] = true;
}

} // namespace sevens
