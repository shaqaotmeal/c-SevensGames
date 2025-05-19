# SmartSevens – Strategy for the Sevens Game

## information sur l'équipe

- nom de l'équipe: OSCAR

  étudiant:
  Abbas Mohamad
  Abdallah Sarah


## vue du projet : 

Ce projet consiste à implémenter une stratégie dynamique pour un joueur du jeu de cartes Sevens, intégrée à un moteur de jeu en C++.
Le jeu oppose jusqu’à quatre joueurs qui s’affrontent pour être les premiers à poser toutes leurs cartes, en respectant des règles spécifiques liées aux valeurs (rangs) et aux couleurs (enseignes) des cartes jouables.
Chaque stratégie est chargée dynamiquement via une bibliothèque partagée (.so sous Linux, .dylib sous macOS), ce qui permet un développement et des tests indépendants.




## Strategie implémentée – `SmartSevensStrategy`

Notre stratégie est implémentée dans le fichier SmartSevensStrategy.cpp, compilée en tant que bibliothèque partagée et chargée dynamiquement par le moteur de jeu.

## Principes Clés de Conception :

- Jouer légalement mais stratégiquement : Toujours privilégier les coups légaux qui n’ouvrent pas une nouvelle couleur, sauf si cela est inévitable.

- Réduction de la main : Favoriser les cartes qui, une fois jouées, débloquent d’autres cartes de notre propre main.

- Domination de couleur : Encourager les coups dans les couleurs où le joueur possède beaucoup de cartes, et pénaliser l’ouverture de couleurs où il en a peu.

- Support des voisins : Privilégier les cartes adjacentes à d’autres cartes dans la même couleur afin de maintenir les possibilités de jeu ouvertes.

- Blocage des adversaires : Éviter de jouer des cartes qui offrent des opportunités aux adversaires, à moins que ces cartes soient également en notre possession.

- Évaluation dynamique : Chaque carte jouable est notée dynamiquement en fonction du contexte (main + table), et la meilleure carte est sélectionnée.

-  Cela rend la stratégie adaptative, contrairement à une stratégie aléatoire.


## Justification du choix stratégique :

- Surpasser les stratégies naïves (aléatoire, gloutonne) qui ne tiennent pas compte du contexte de la partie.
- S’adapter dynamiquement à la configuration du jeu (état de la table, composition de la main) afin de prendre des décisions plus intelligentes.
- Maximiser la réduction de la main tout en limitant les opportunités offertes aux adversaires.
- Favoriser un jeu prévisionnel : chaque décision tient compte non seulement de l’action immédiate, mais aussi de son impact potentiel sur les coups futurs.
- être prudent quand il faut, offensif quand c’est avantageux, et toujours conforme aux règles du jeu.


## Exemple de Performance

Exemple de Résultat de Classement :
[main] Résultats de la compétition :
  SmartSevensV10-1 -> Rang final 1
  SmartSevensV10-0 -> Rang final 2
  RandomStrategy-2 -> Rang final 3
  GreedyStrategy-3 -> Rang final 4

## limitations 

- La stratégie n’intègre pas encore de mécanismes de tromperie.
- Le jeu est un peu basé sur de la chance
- Absence d'apprentissage automatique durant les parties
- Chaque décision est prise sans tenir compte des autres joueurs 
- Il n’existe aucun modèle prédictif des adversaires 


## comment compiler et exécuter : ( MAC )
- 1. Compiler le moteur du jeu (sevens_game) : g++ -std=c++17 -Wall -Wextra -Werror -pedantic -pedantic-errors -O3 *.cpp -o sevens_game

- 2. Compiler les stratégies en bibliothèques partagées (.so) : 

        # SmartSevensStrategy
        g++ -std=c++17 -O3 -fPIC -DBUILD_SHARED_LIB -shared SmartSevensStrategy.cpp -o smart_strategy.so

        # Random and greedy 
        g++ -std=c++17 -O3 -fPIC -DBUILD_SHARED_LIB -shared RandomStrategy.cpp -o random_strategy.so
        g++ -std=c++17 -O3 -fPIC -DBUILD_SHARED_LIB -shared GreedyStrategy.cpp -o greedy_strategy.so

- 3. Lancer une compétition (competition) :

    ./sevens_game competition smart_strategy.so random_strategy.so greedy_strategy.so smart_strategy.so 

    (Ici, on a 4 stratégies .so, donc le nombre de joueurs est équivalent au nombre de stratégies incluses dans cette commande, donc 4 joueurs. Le joueur 0 utilise smart_strategy.so, le joueur 2 utilise random_strategy.so, etc...)

Explication de chaque élément :


./sevens_game	Lance l’exécutable principal que tu as compilé à l’étape 1.

competition	Mode de jeu : compétition entre plusieurs stratégies dynamiques (fichiers .so).

smart_strategy.so	Un joueur utilise notre stratégie.
random_strategy.so	Un joueur utilise la stratégie random.
greedy_strategy.so	Un joueur utilise la stratégie greddy.
smart_strategy.so	Un deuxième joueur utilise aussi notre stratégie.


## Mode demo :

- ./sevens_game demo :  lance une partie automatique avec des stratégies internes (déjà codées dans le moteur, 2 random vs 2 greddy).

## Mode internal :

- ./sevens_game internal : Lance une partie avec des stratégies "internes" codées en dur dans le moteur du jeu ( 4 joueus avec 4 stratégie random).





## Crédits, Remerciements et Références : 

- Squelette du projet : fourni par les enseignants du cours, incluant les fichiers de base comme main.cpp, Card.hpp, PlayerStrategy.hpp, etc.
- Documentation technique 


Remerciements particuliers :

À notre enseignant·e(s) pour la clarté des consignes du projet, et d'avoir assuré les cours de cette UE


- https://en.cppreference.com/w/cpp
- "Effective Modern C++" – Scott Meyers
- https://formulae.brew.sh/formula/gcc
- https://stackoverflow.com/questions

