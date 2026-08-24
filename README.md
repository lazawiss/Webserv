*This project has been created as part of the 42 curriculum by ankim, dpaiva, lzannis.*

# Webserv

## Description

**Webserv** est un serveur HTTP écrit en **C++98**. L'objectif de ce projet est de créer un serveur web à l'image d'**NGINX**, serveur web très répandu, dont le rôle principal est de recevoir des requêtes HTTP de clients et d'y répondre. NGINX sert, ainsi, de modèle de référence pour le comportement général de ce projet.

Webserv repose notamment sur les fonctionnalités suivantes :

- lire un **fichier de configuration** inspiré de la syntaxe NGINX ;
- créer un ou plusieurs **sockets d'écoute** (une paire adresse:port par server) ;
- gèrer **plusieurs clients simultanément**, de manière **non bloquante**,
grâce à une boucle événementielle basée sur epoll() ;
- **parser** les requêtes HTTP reçues et répondre avec les méthodes **GET**, **POST** et **DELETE** ;
- servir des **fichiers statiques**, gèrer l'**upload** de fichiers,
le **listing de répertoire** (autoindex), les **redirections**
et les **pages d'erreur** personnalisées ;
- exécuter des scripts **CGI** (Python et PHP) en fonction de l'extension du fichier demandé ;

## Instructions

### Prérequis

- Compilateur C++ compatible **C++98** (`g++` ou `clang++`)
- Système **Linux** (la boucle événementielle repose sur `epoll`, spécifique à Linux)
- **Python 3** et/ou **PHP-CGI** si l'on souhaite utiliser les fonctionnalités CGI

### Compilation

```bash
git clone [url]
cd webserv
make
```

La compilation génère un exécutable `webserv` à la racine du projet. Le `Makefile` fournit également les règles classiques :

```bash
make clean          # supprime les fichiers objets
make fclean         # supprime les fichiers objets et l'exécutable
make re             # recompile entièrement le projet
```

### Exécution

```bash
./webserv [fichier de configuration]
```

Un fichier de configuration doit être passé en argument. Des exemples sont disponibles dans `data/config/`.

### Syntaxe de configuration

La configuration suit une syntaxe inspirée de NGINX, avec trois niveaux : `global`, `server` et `location`.

```nginx
client_max_body_size 10M;

server {
    listen       127.0.0.1:8080;
    server_name  tsuki;

    location / {
        root        data/www/tsuki;
        index       index.html;
        methods     GET;
        autoindex   off;
    }

    location /upload {
        root        data/upload;
        index       index.html;
        methods     GET POST DELETE;
        autoindex   off;
    }

    location /cgi-bin/python {
        root            data/cgi-bin;
        methods         POST;
        cgi_extension   .py  /usr/bin/python3;
        index           index.py;
    }
}
```

**Directives disponibles :**

| Directive | Contexte | Description |
|---|---|---|
| `client_max_body_size` | `global` / `server` / `location` | Taille maximale du corps de la requête |
| `root` | `global` / `server` / `location` | Répertoire racine des fichiers servis |
| `index` | `global` / `server` / `location` | Fichier servi par défaut |
| `error_page` | `global` / `server` / `location` | Page d'erreur personnalisée |
| `autoindex` | `global` / `server` / `location` | Activation du listing de répertoire |
| `listen` | `server` | Adresse IP et port d'écoute |
| `server_name` | `server` | Nom de domaine (virtual hosting) |
| `methods` | `location` | Méthodes HTTP autorisées |
| `return` | `location` | Redirection HTTP |
| `cgi_extension` | `location` | Association extension → interpréteur CGI |

### Tester le serveur

Une fois lancé, le serveur écoute sur les adresses/ports définis dans la configuration. Il peut être testé :

- avec un navigateur web :

```bash
http://localhost:8080
````

(ouvrir dans un second terminal)
- avec `curl` :

```bash
curl -v http://127.0.0.1:8080/
```

- avec telnet :
telnet localhost 8080

- avec un outil de stress test (ex : `siege`) pour vérifier la stabilité du serveur sous charge :
 siege -c 10 -t 1M http://127.0.0.1:8080/


## Architecture

**Architecture du projet :**

```
Lexer / Parser      →  lit le fichier de config  →  GlobalConfig / ServerConfig / LocationConfig

Server
├── SignalManager       gère SIGINT / SIGQUIT
├── ListenerManager     sockets d'écoute (un par `listen`)
├── EpollLoop           boucle principale (epoll) : reçoit la requête du client et envoie une réponse
│   ├── HTTPParser      parse la requête brute
│   ├── RequestHandler  construit la réponse
│   └── CGIHandler      exécute les scripts Python / PHP
```

**Flux d'une requête :**

```
Client
  │  TCP connect
  ▼
ListenerManager  ──accept()──►  nouveau fd client
  │
  ▼
EpollLoop  (epoll_wait)
  │
  ├─ EPOLLIN  ──►  HTTPParser       parse méthode / headers / body
  │                    │
  │                    ▼
  │               RequestHandler    décide de la réponse
  │                    ├── fichier statique  →  lecture disque
  │                    ├── upload            →  écriture disque
  │                    └── CGI (.py/.php)    →  CGIHandler (fork + pipe)
  │
  └─ EPOLLOUT ──►  envoie la réponse  ──►  Client
```

## Lexique

### Client / Serveur
Un **serveur** est un programme qui attend des connexions (requête HTTP) et répond aux demandes. Un **client** (navigateur, `curl`, etc.) est celui qui initie la connexion et envoie une requête HTTP. La communication passe par un réseau TCP/IP : le client ouvre une connexion vers l'adresse IP et le port du serveur.

### HTTP (HyperText Transfer Protocol)
Protocole texte au-dessus de TCP qui définit le format des échanges entre client et serveur. Une **requête** HTTP contient :
- une **ligne de requête** (request-line) : méthode + chemin + version (`GET / HTTP/1.1`)
- des **headers** : métadonnées (`Host:`, `Content-Type:`, `Content-Length:`…)
- un **body** (optionnel) : données envoyées (formulaire, fichier uploadé…)

Une **réponse** contient un code de statut (`200 OK`, `404 Not Found`…), des headers, et le contenu.

### Socket
Point d'entrée réseau représenté par un descripteur de fichier (fd). Le serveur crée un socket d'écoute par directive `listen`, accepte les connexions entrantes (`accept()`), puis chaque client obtient son propre fd pour lire/écrire.

### Méthodes HTTP
- **GET** : demander une ressource (page, image…)
- **POST** : envoyer des données au serveur (formulaire, upload)
- **DELETE** : supprimer une ressource

### epoll()
Table Linux pour surveiller de nombreux fds simultanément **sans bloquer**. Au lieu d'attendre sur un seul fd, `epoll_wait()` retourne la liste des fds prêts à lire ou à écrire. C'est ce qui permet de gérer des dizaines de clients en parallèle dans **un seul thread**, sans créer un thread par connexion.

### Non-bloquant (I/O non-bloquante)
Par défaut un appel `read()` ou `write()` attend que des données soient disponibles. En mode non-bloquant, il retourne immédiatement si rien n'est prêt. Couplé à epoll, cela évite qu'un client lent bloque tous les autres.

### CGI (Common Gateway Interface)
Interface standard pour qu'un serveur web exécute un script externe (Python, PHP…). Le serveur crée un processus fils via `fork()` + `execve()`, lui passe la requête via des variables d'environnement et un pipe, et lit la réponse générée sur stdout. Ici, c'est `CGIHandler` qui orchestre ça.

### Parsing / Lexer / Parser
Transformer du texte brut en données structurées. Le **Lexer** découpe le fichier de config en tokens. Le **Parser** lit ces tokens et construit les objets `GlobalConfig` / `ServerConfig` / `LocationConfig`. Même principe pour `HTTPParser` sur les requêtes HTTP brutes.

### Virtual hosting
Faire tourner plusieurs sites sur le même serveur (même IP/port) en différenciant par le header `Host:`. Chaque bloc `server` avec un `server_name` différent correspond à un site distinct.

## Ressources

### Documentation et articles

- [Documentation officielle NGINX](https://nginx.org/en/docs/) — modèle de référence pour la syntaxe de configuration et le comportement général du serveur
- [RFC 9110 – HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110.html)
- [RFC 9112 – HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — référence classique pour la programmation socket en C/C++
- [man epoll(7)](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [The Common Gateway Interface (CGI) — RFC 3875](https://www.rfc-editor.org/rfc/rfc3875)
- [Simple HTTP webserver in C – bruinsslot.jp](https://bruinsslot.jp/post/simple-http-webserver-in-c/) — tutoriel utilisé pour comprendre la structure générale d'un serveur HTTP en C
- [Codes de statut HTTP – MDN](https://developer.mozilla.org/fr/docs/Web/HTTP/Status)

### Utilisation de l'IA

L'IA (Claude, Anthropic) a été utilisée ponctuellement comme outil d'accompagnement, notamment pour :

- reformuler et structurer les notes de travail prises pendant la phase de recherche en une documentation claire.

