
# 2 Serveur Web KC3 HTTPd

Les fichiers sources du serveur web KC3 HTTPd se trouvent dans le répertoire `/httpd`.

## 2.1 Dépendances

Le serveur web dépend des éléments suivants :

- **KC3**
  - `libkc3`
  - `event`
  - `socket`
  - `http`
- **SASS** pour le rendu CSS
- **ESBuild** pour le rendu JavaScript

## 2.2 Configuration

Le serveur web doit être lancé dans un répertoire contenant les fichiers suivants :

```
./app/controllers/     # Répertoire des contrôleurs (.kc3)
./app/templates/       # Répertoire des templates (.ekc3)
./app/views/           # Répertoire des vues (.kc3)
./assets/              # Répertoire des ressources
./assets/css/          # Répertoire CSS
./assets/css/app.scss  # CSS de l'application, compilé en ./static/_assets/app.css
./assets/js/           # Répertoire JavaScript
./assets/js/app.js     # JavaScript de l'application, compilé en ./static/_assets/app.js
./assets/node_modules/ # Modules npm
./config/routes.kc3    # Routes dynamiques
./static/              # Fichiers statiques
./static/_assets/      # Fichiers de ressources statiques
./static/_fonts/       # Fichiers de polices statiques
./static/_images/      # Fichiers d'images statiques
```

## 2.3 Exemple

Exécutez `make test_httpd` pour lancer le serveur web KC3 HTTPd (`httpd/kc3_httpd`) en utilisant le répertoire de travail `test/httpd`.

Ce répertoire contient un exemple de configuration et d'utilisation de `kc3_httpd`.

Les fichiers statiques sont servis depuis `./static` et sont accessibles à la racine de l'URL (`/`).

La documentation au format Markdown et HTML est servie depuis l'URL `/doc/`. La documentation dispose d'une route personnalisée définie dans `./config/routes.kc3`, d'un contrôleur défini dans `./app/controllers/doc_controller.kc3` et de quelques templates définis dans `./app/templates/doc/`.

La mise en page principale de l'application est définie dans `./app/templates/layout.html.ekc3` et `./app/templates/nav.html.ekc3`. Notez l'extension `.html.ekc3`, ce sont des fichiers HTML avec du code KC3 intégré. Consultez la documentation EKC3 pour plus d'informations sur ces fichiers.
