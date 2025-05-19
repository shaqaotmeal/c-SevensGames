#include "MyCardParser.hpp"

namespace sevens {

void MyCardParser::read_cards(const std::string&) // Initialise le dictionnaire des 52 cartes du jeu.
{
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 1; rank <= 13; ++rank) {
            uint64_t id = static_cast<uint64_t>(suit) * 13ULL + (rank - 1);
            cards_hashmap[id] = Card{ suit, rank };
        }
    }
}

} 
