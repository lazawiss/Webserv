*This project has been created as part of the 42 curriculum by ankim, dpaiva, lzannis*

# Webserv

## Description

**Webserv** est un serveur HTTP écrit en **C++98**. L'objectif de ce projet est de créer un serveur web à l'image de **NGINX**, serveur web très répandu, dont le rôle principal est de recevoir des requêtes HTTP de clients et d'y répondre. NGINX sert ainsi de modèle de référence pour le comportement général de ce projet.

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
make clean   # supprime les fichiers objets
make fclean  # supprime les fichiers objets et l'exécutable
make re      # recompile entièrement le projet
```

### Exécution

```bash
./webserv [fichier de configuration]
```

Un fichier de configuration doit être passé en argument. Des exemples sont disponibles dans `data/config/`.

### Syntaxe de configuration

La configuration suit une syntaxe inspirée de NGINX, avec trois niveaux : `global`, `server` et `location`.

```nginx
server {
    listen       0.0.0.0:8080;
    server_name  localhost;

    client_max_body_size 10M;

    location / {
        root        data/www/;
        methods     GET POST DELETE;
        index       index.html;
        autoindex   on;
    }

    location /upload {
        root        data/upload/;
        methods     POST DELETE;
    }

    location /cgi-bin {
        root            data/cgi-bin;
        methods         GET POST;
        cgi_extension   .py  /usr/bin/python3;
        cgi_extension   .php /usr/bin/php-cgi;
    }
}
```

**Directives disponibles :**

| Directive | Contexte | Description |
|---|---|---|
| `listen` | `server` | Adresse IP et port d'écoute |
| `server_name` | `server` | Nom de domaine (virtual hosting) |
| `client_max_body_size` | `global` / `server` / `location` | Taille maximale du corps de la requête |
| `root` | `global` / `server` / `location` | Répertoire racine des fichiers servis |
| `index` | `global` / `server` / `location` | Fichier servi par défaut |
| `methods` | `location` | Méthodes HTTP autorisées |
| `autoindex` | `global` / `server` / `location` | Activation du listing de répertoire |
| `return` | `location` | Redirection HTTP |
| `error_page` | `global` / `server` / `location` | Page d'erreur personnalisée |
| `cgi_extension` | `location` | Association extension → interpréteur CGI |

### Tester le serveur

Une fois lancé, le serveur écoute sur les adresses/ports définis dans la configuration. Il peut être testé :

- avec un navigateur web :

```bash
http://localhost:8080
````


- avec `curl` :

```bash
curl -v http://127.0.0.1:8080/
```

- avec un outil de stress test (ex : `siege`) pour vérifier la stabilité du serveur sous charge.

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
