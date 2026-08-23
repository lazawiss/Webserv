*This project has been created as part of the 42 curriculum by .*

# Webserv

## Description

**Webserv** est un serveur HTTP écrit en **C++98**. L'objectif de ce projet est de créer un serveur web et de comprendre son fonctionnement, à l'image de **NGINX**.
 
Nginx est un serveur web très répandu. Son rôle principal est de recevoir des requêtes HTTP de clients et d'y répondre, en redirigeant vers d'autres serveurs ou en exécutant des scripts (CGI, etc.).
 
Dans le cadre de **webserv**, NGINX sert de modèle de référence : le programme doit se comporter de manière similaire, en lisant un fichier de configuration du même style.


Le programme :

- lit un **fichier de configuration** inspiré de la syntaxe NGINX ;
- crée un ou plusieurs **sockets d'écoute** (une paire IP:port par `server`) ;
- gère **plusieurs clients simultanément**, de manière **non bloquante**, 
grâce à une boucle événementielle basée sur `epoll()` ;
- **parse** les requêtes HTTP reçues ;
- répond avec les méthodes **GET**, **POST** et **DELETE** ;
- sert des **fichiers statiques**, gère l'**upload** de fichiers, 
le **listing de répertoire** (autoindex), les **redirections** (`return`) 
et les **pages d'erreur** personnalisées ;
- exécute des scripts **CGI**  (Python et PHP) en fonction de l'extension du fichier demandé ;
- reste stable quel que soit le trafic reçu (jamais de crash, 
jamais de blocage indéfini, jamais de fuite de file descriptors).

## Instructions

### Compilation

```bash
git clone [url]
cd webserv
make
```

La compilation génère un exécutable `webserv` à la racine du projet. Le `Makefile` fournit également les règles classiques :

```bash
make clean   # supprime les fichiers objets
make fclean  # supprime les fichiers objets et l'exécutable
make re      # recompile entièrement le projet
```

### Exécution

```bash
./webserv [configuration file]
```

### Tester le serveur

Une fois lancé, le serveur écoute sur les adresses/ports définis dans la configuration. Il peut être testé :

- avec un navigateur web (`http://localhost:8080`) ;
- avec `curl` :

```bash
curl 

```

- avec un outil de stress test (ex : `siege`) pour vérifier la stabilité du serveur sous charge.

## Fonctionnalités principales

- Fichier de configuration façon NGINX (contexte global → `server` → `location`, avec héritage des directives)
- Plusieurs `server` possibles, y compris sur une même IP:port (mécanisme de serveur par défaut / virtual host via `server_name`)
- Boucle d'événements unique et non bloquante (un seul `poll()` pour tous les sockets, lecture/écriture jamais bloquante)
- Méthodes HTTP : `GET`, `POST`, `DELETE`
- Upload de fichiers
- Listing de répertoire (`autoindex`)
- Pages d'erreur par défaut et personnalisées
- Redirections HTTP (`return`)
- Support CGI (Python et PHP), avec gestion correcte des variables d'environnement et du body transmis en `stdin`
- Limite de taille de body configurable (`client_max_body_size`)

## Ressources

### Documentation et articles

- [Documentation officielle NGINX](https://nginx.org/en/docs/) — modèle de référence pour la syntaxe de configuration et le comportement général du serveur
- [RFC 9110 – HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110.html)
- [RFC 9112 – HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — référence classique pour la programmation socket en C/C++
- [man poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html)
- [The Common Gateway Interface (CGI) — RFC 3875](https://www.rfc-editor.org/rfc/rfc3875)
- [Simple HTTP webserver in C – bruinsslot.jp](https://bruinsslot.jp/post/simple-http-webserver-in-c/) — tutoriel utilisé pour comprendre la structure générale d'un serveur HTTP en C
- [Codes de statut HTTP – MDN](https://developer.mozilla.org/fr/docs/Web/HTTP/Status)

### Utilisation de l'IA

L'IA (Claude, Anthropic) a été utilisée ponctuellement comme outil d'accompagnement, notamment pour :

- reformuler et structurer les notes de travail prises pendant la phase de recherche en une documentation claire.