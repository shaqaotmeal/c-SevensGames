#include "MyGameMapper.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>

namespace sevens {

// ─────────────────────────────────────────────────────────────────────────────
// Constructeur : initialise le générateur de nombres aléatoires
MyGameMapper::MyGameMapper()
    : rng(static_cast<unsigned long>(
          std::chrono::system_clock::now().time_since_epoch().count()))
{ }

// ─────────────────────────────────────────────────────────────────────────────
// Initialise le dictionnaire de cartes
void MyGameMapper::read_cards(const std::string&) {
    card_parser.read_cards("");
}

// ─────────────────────────────────────────────────────────────────────────────
// Initialise l'état initial de la table (avec 7♦ par défaut)
void MyGameMapper::read_game(const std::string&) {
    game_parser.read_game("");
}

// ─────────────────────────────────────────────────────────────────────────────
// Indique si des stratégies ont été enregistrées
bool MyGameMapper::hasRegisteredStrategies() const {
    return !strategies.empty();
}

// ─────────────────────────────────────────────────────────────────────────────
// Enregistre une stratégie pour un joueur donné
void MyGameMapper::registerStrategy(uint64_t playerID,
                                    std::shared_ptr<PlayerStrategy> strat)
{
    strategies[playerID] = std::move(strat);
    strategies[playerID]->initialize(playerID);
    score_board[playerID] = 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Simule le jeu jusqu'à ce qu'un joueur dépasse ou atteigne 50 points
std::vector<std::pair<uint64_t, uint64_t>>
MyGameMapper::compute_game_progress(uint64_t /*numPlayers*/) {
    // Liste des identifiants de joueurs
    std::vector<uint64_t> ids;
    for (auto& kv : strategies)
        ids.push_back(kv.first);

    // Création du paquet de 52 cartes
    std::vector<Card> deck;
    deck.reserve(card_parser.get_cards_hashmap().size());
    for (auto& kv : card_parser.get_cards_hashmap())
        deck.push_back(kv.second);

    // Simulation de manches successives
    while (true) {
        // Mélange et distribution des cartes
        std::shuffle(deck.begin(), deck.end(), rng);
        std::unordered_map<uint64_t, std::vector<Card>> hands;
        for (auto id : ids) hands[id] = {};
        for (size_t i = 0; i < deck.size(); ++i)
            hands[ids[i % ids.size()]].push_back(deck[i]);

        // Réinitialisation de la table avec uniquement le 7♦
        game_parser.read_game("");
        auto table_layout = game_parser.get_table_layout(); // copie modifiable

        // Suppression du 7♦ dans chaque main (déjà posé sur la table)
        for (auto& kv : hands) {
            auto& h = kv.second;
            h.erase(std::remove_if(h.begin(), h.end(), [](const Card& c){
                return c.suit == 1 && c.rank == 7;
            }), h.end());
        }

        // Tour par tour jusqu'à ce qu'aucun joueur ne puisse jouer
        bool anyMove = true;
        while (anyMove) {
            anyMove = false;
            for (auto id : ids) {
                auto& strat = strategies.at(id);
                int idx = strat->selectCardToPlay(hands[id], table_layout);
                bool moved = false;

                // Vérifie si l'index proposé est valide
                if (idx >= 0 && static_cast<size_t>(idx) < hands[id].size()) {
                    Card c = hands[id][idx];
                    // Teste la validité du coup (adjacent à une carte posée)
                    bool playable = (c.rank == 7)
                        || table_layout[c.suit][c.rank - 1]
                        || table_layout[c.suit][c.rank + 1];
                    if (playable) {
                        table_layout[c.suit][c.rank] = true;
                        hands[id].erase(hands[id].begin() + idx);
                        strat->observeMove(id, c);
                        moved = true;
                        anyMove = true;
                    }
                }

                // Si le joueur n’a pas joué, on le marque en “pass”
                if (!moved) {
                    strat->observePass(id);
                }
            }
        }

        // Comptage des cartes restantes → score
        std::vector<std::pair<uint64_t, uint64_t>> results;
        for (auto id : ids) {
            auto& pts = score_board[id];
            pts += hands[id].size();
            results.emplace_back(id, pts);
        }

        // Vérifie si un joueur a atteint ou dépassé 50 points
        for (auto& kv : score_board) {
            if (kv.second >= 50) {
                return results;
            }
        }
        // Sinon, une nouvelle manche est lancée
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Lance la simulation complète et affiche les scores finaux
std::vector<std::pair<uint64_t, uint64_t>>
MyGameMapper::compute_and_display_game(uint64_t n) {
    auto res = compute_game_progress(n);

    std::cout << "Final scores:\n";
    for (auto& pr : res) {
        std::cout << "Player " << pr.first << " -> "
                  << pr.second << " pts\n";
    }

    return res;
}

} // namespace sevens
