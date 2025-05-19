#include "PlayerStrategy.hpp"
#include <unordered_map>
#include <vector>
#include <chrono>
#include <random>
#include <bitset>
#include <iostream>
#include <climits>

namespace sevens {

// ─── Helpers ─────────────────────────────────────────────────────────────
static const char* COLORS = "CDHS";  // Clubs, Diamonds, Hearts, Spades

// Convertit un rang numérique en caractère (ex : 1 → 'A', 11 → 'J', etc.)
static char valueToChar(int value) {
    if (value == 1) return 'A';
    if (value >= 2 && value <= 9) return char('0' + value);
    return "TJQK"[value - 10];
}

// Donne la valeur d'une carte (As = 14)
static int cardPower(int value) {
    return (value == 1 ? 14 : value);
}

// ─── Règles de jeu ───────────────────────────────────────────────────────

// Vérifie si une carte peut être jouée (adjacente à une carte déjà posée ou 7)
static bool isPlayable(const Card& card,
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& table)
{
    if (card.rank == 7) return true;
    bool left  = (card.rank > 1  && table.at(card.suit).at(card.rank - 1));
    bool right = (card.rank < 13 && table.at(card.suit).at(card.rank + 1));
    return left || right;
}

// Vérifie si la carte ouvre une nouvelle couleur (aucune adjacente sur la table)
static bool opensNewColor(const Card& card,
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& table)
{
    return !isPlayable(card, table);
}

// ─── La stratégie intelligente SmartSevens ───────────────────────────────
class SmartSevensStrategy : public PlayerStrategy {
public:
    SmartSevensStrategy() {
        auto seed = static_cast<unsigned long>(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        rng_.seed(seed);
    }

    ~SmartSevensStrategy() override = default;

    void initialize(uint64_t id) override {
        myID_ = id;
        cardsSeen_.reset();
        passes_.clear();
    }

    int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& table
    ) override {
        std::vector<int> playable;

        // 1) Trouver les indices jouables
        for (int i = 0; i < static_cast<int>(hand.size()); ++i)
            if (isPlayable(hand[i], table))
                playable.push_back(i);

        if (playable.empty()) return -1;

        // 2) Séparer safe vs risky
        std::vector<int> safe, risky;
        for (int idx : playable) {
            (opensNewColor(hand[idx], table) ? risky : safe).push_back(idx);
        }
        if (!safe.empty()) playable.swap(safe);
        else               playable.swap(risky);

        // 3) Évaluer les coups et choisir le meilleur
        int bestIdx = playable[0];
        int bestScore = INT_MIN;

        std::cout << "-- SmartSevens scoring --\n";
        for (int idx : playable) {
            const Card& card = hand[idx];
            int score = evaluate(card, hand, table);
            std::cout << "   [" << valueToChar(card.rank) << COLORS[card.suit]
                      << "] -> " << score << "\n";
            if (score > bestScore) {
                bestScore = score;
                bestIdx = idx;
            }
        }

        const Card& chosen = hand[bestIdx];
        std::cout << "  -> choosing ["
                  << valueToChar(chosen.rank) << COLORS[chosen.suit]
                  << "]\n\n";
        return bestIdx;
    }

    void observeMove(uint64_t, const Card& card) override {
        cardsSeen_.set(cardKey(card));
    }

    void observePass(uint64_t playerID) override {
        ++passes_[playerID];
    }

    std::string getName() const override {
        return "SmartSevens";
    }

private:
    // Calcule un identifiant unique entre 0 et 51 pour une carte
    static uint64_t cardKey(const Card& card) {
        return card.suit * 13 + (card.rank - 1);
    }

    // Évalue l'intérêt stratégique de jouer une carte donnée
    int evaluate(const Card& card,
                 const std::vector<Card>& hand,
                 const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& table) const
    {
        int score = 0;

        score += 10;  // bonus par défaut (jouer = mieux)

        // Compte combien de cartes on a dans la même couleur
        int sameColorCount = 0;
        for (const auto& h : hand)
            if (h.suit == card.suit)
                ++sameColorCount;

        // Bonus : cartes de même couleur qui deviendraient jouables après ce coup
        {
            auto simulatedTable = table;
            simulatedTable[card.suit][card.rank] = true;
            int future = 0;
            for (const auto& h : hand) {
                if (h.suit == card.suit && h.rank != card.rank &&
                    isPlayable(h, simulatedTable)) {
                    future += cardPower(h.rank);
                }
            }
            score += future / 2;
        }

        // Bonus ou malus selon le contrôle de la couleur
        if (opensNewColor(card, table)) {
            if (sameColorCount >= 8)       score += 20;
            else if (sameColorCount >= 4)  score += 5;
            else                           score -= 40;
        }

        // Bonus si on a les voisins (ex: 6 et 8 autour du 7)
        for (const auto& h : hand) {
            if (h.suit == card.suit) {
                if (h.rank == card.rank - 1) score += 4;
                if (h.rank == card.rank + 1) score += 4;
            }
        }

        // Malus si on ouvre un chemin risqué (5 ou 9)
        if ((card.rank == 6 && !table.at(card.suit).at(5)) ||
            (card.rank == 8 && !table.at(card.suit).at(9))) {
            score -= 6;
        }

        // Bonus si on a plusieurs cartes de cette couleur, sinon malus
        if (sameColorCount <= 1) score -= 8;
        else                     score += sameColorCount;

        // Malus si on débloque des cartes que l'on ne possède pas
        int opponentUnlocks = 0;
        for (int delta : {-1, +1}) {
            int neighborRank = card.rank + delta;
            if (neighborRank < 1 || neighborRank > 13) continue;
            if (!table.at(card.suit).at(neighborRank)) {
                bool iOwnIt = false;
                for (const auto& h : hand)
                    if (h.suit == card.suit && h.rank == neighborRank) {
                        iOwnIt = true;
                        break;
                    }
                if (!iOwnIt) ++opponentUnlocks;
            }
        }
        score -= opponentUnlocks * 3;

        // Malus léger si des adversaires ont déjà passé
        for (const auto& p : passes_)
            if (p.second > 0)
                score -= 1;

        return score;
    }

    uint64_t myID_{0};
    std::bitset<52> cardsSeen_; // Carte déjà vues/posées
    std::unordered_map<uint64_t, int> passes_;
    std::mt19937 rng_;
};

} // namespace sevens

// Création dynamique de la stratégie (si build en .so)
#ifdef BUILD_SHARED_LIB
extern "C" sevens::PlayerStrategy* createStrategy() {
    return new sevens::SmartSevensStrategy();
}
#endif
