#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "MyGameMapper.hpp"
#include "RandomStrategy.hpp"
#include "GreedyStrategy.hpp"
#include "StrategyLoader.hpp"
#include "PlayerStrategy.hpp"

using namespace sevens;

// Affiche les instructions d'utilisation si appel incorrect
static void usage(const char* bin)
{
    std::cerr << "Usage:\n"
              << "  " << bin << " internal\n"
              << "  " << bin << " demo\n"
              << "  " << bin << " competition lib1.so [lib2.so …]\n";
}

/* --------------------------------------------------------------------- */
int main(int argc, char* argv[])
{
    // Vérification du nombre d'arguments
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const std::string mode = argv[1];
    MyGameMapper mapper;

    // Chargement initial du paquet et de la table
    mapper.read_cards("");
    mapper.read_game("");

    /* -------------------- MODE INTERNAL ------------------------------ */
    if (mode == "internal")
    {
        std::vector<std::string> pname;

        // Tous les joueurs utilisent la stratégie Random
        for (int pid = 0; pid < 4; ++pid)
        {
            auto s = std::make_shared<RandomStrategy>();
            mapper.registerStrategy(pid, s);
            pname.push_back(s->getName() + '-' + std::to_string(pid));
        }

        std::cout << "[main] Running INTERNAL :\n";
        for (std::size_t i = 0; i < pname.size(); ++i)
            std::cout << "  P" << i << " → " << pname[i] << '\n';

        auto res = mapper.compute_and_display_game(4);

        std::cout << "\n[main] Results (lower pts = better):\n";
        std::sort(res.begin(), res.end(),
                  [](auto& a, auto& b){ return a.second < b.second; });
        for (auto& p : res)
            std::cout << "  " << pname[p.first] << " -> "
                      << p.second << " pts\n";

        return 0;
    }

    /* -------------------- MODE DEMO ---------------------------------- */
    if (mode == "demo")
    {
        std::vector<std::string> pname;

        // 2 joueurs Random, 2 joueurs Greedy
        for (int pid = 0; pid < 4; ++pid)
        {
            std::shared_ptr<PlayerStrategy> s;
            if (pid < 2)
                s = std::make_shared<RandomStrategy>();
            else
                s = std::make_shared<GreedyStrategy>();

            mapper.registerStrategy(pid, s);
            pname.push_back(s->getName() + '-' + std::to_string(pid));
        }

        std::cout << "[main] Running DEMO :\n";
        for (std::size_t i = 0; i < pname.size(); ++i)
            std::cout << "  P" << i << " → " << pname[i] << '\n';

        auto res = mapper.compute_and_display_game(4);

        std::cout << "\n[main] Results (lower pts = better):\n";
        std::sort(res.begin(), res.end(),
                  [](auto& a, auto& b){ return a.second < b.second; });
        for (auto& p : res)
            std::cout << "  " << pname[p.first] << " -> "
                      << p.second << " pts\n";

        return 0;
    }

    /* -------------------- MODE COMPETITION --------------------------- */
    if (mode == "competition")
    {
        // Vérifie que des stratégies ont été passées en arguments
        if (argc < 3) {
            usage(argv[0]);
            return 1;
        }

        std::vector<std::string> pname;

        // Chargement dynamique des .so
        for (int i = 2; i < argc; ++i)
        {
            const std::string lib = argv[i];
            std::cout << "Loading strategy from " << lib << "...\n";

            auto s = StrategyLoader::load_from_library(lib);
            const std::string label = s->getName() + '-' + std::to_string(i - 2);

            std::cout << "Registered " << label << " successfully.\n";

            mapper.registerStrategy(i - 2, s);
            pname.push_back(label);
        }

        std::cout << "\nStarting competition with " << pname.size() << " players...\n";

        auto res = mapper.compute_and_display_game(pname.size());

        // Trie les résultats selon les scores croissants
        std::vector<std::pair<uint64_t, uint64_t>> sorted_res = res;
        std::sort(sorted_res.begin(), sorted_res.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });

        // Attribue les rangs à chaque joueur
        std::unordered_map<int, int> pid_to_rank;
        for (size_t rank = 0; rank < sorted_res.size(); ++rank) {
            pid_to_rank[sorted_res[rank].first] = static_cast<int>(rank + 1);
        }

        // Affiche le classement final (propre)
        std::cout << "\n[MyGameMapper] Final Rankings:\n";
        for (const auto& [pid, score] : sorted_res) {
            std::cout << "  " << pname[pid] << " -> Rank " << pid_to_rank[pid] << "\n";
        }

        // Affiche le résumé final lisible
        std::cout << "\n[main] Competition Results:\n";
        for (size_t i = 0; i < pname.size(); ++i) {
            std::cout << "  " << pname[i]
                      << " -> Final Rank " << pid_to_rank[i] << "\n";
        }

        return 0;
    }

    // Mode inconnu
    usage(argv[0]);
    return 1;
}
