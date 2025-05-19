#pragma once

#include "PlayerStrategy.hpp"
#include <memory>
#include <string>
#include <dlfcn.h>
#include <stdexcept>

namespace sevens {

class StrategyLoader { //  charge dynamiquement une stratégie depuis une bibliothèque partagée (.so).
public:


    static std::shared_ptr<PlayerStrategy> load_from_library(const std::string& path) { //  Charge une stratégie depuis un fichier .so.
        // Étape 1 — Ouvrir la bibliothèque dynamique
        void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle) {
            throw std::runtime_error("dlopen failed: " + std::string(dlerror()));
        }

        // Étape 2 — Récupérer le symbole createStrategy
        dlerror();  // Réinitialise les erreurs précédentes
        void* symbol = dlsym(handle, "createStrategy");
        const char* error = dlerror();
        if (error || !symbol) {
            dlclose(handle);
            throw std::runtime_error("dlsym(createStrategy) failed: " + std::string(error ? error : "<null>"));
        }

        // Étape 3 — Cast POSIX légal de void* vers pointeur de fonction
        using FactoryFn = PlayerStrategy* (*)();
        union { void* ptr; FactoryFn fn; } caster;
        caster.ptr = symbol;
        FactoryFn factory = caster.fn;

        // Étape 4 — Appel de la fonction factory
        PlayerStrategy* raw_ptr = factory();
        if (!raw_ptr) {
            dlclose(handle);
            throw std::runtime_error("createStrategy() returned nullptr in " + path);
        }

        // Étape 5 — Emballer le pointeur brut dans un shared_ptr avec deleter personnalisé
        struct StrategyDeleter {
            void* handle;
            void operator()(PlayerStrategy* p) const {
                delete p;
                if (handle) dlclose(handle);
            }
        };

        return std::shared_ptr<PlayerStrategy>(raw_ptr, StrategyDeleter{handle});
    }
};

} // namespace sevens
