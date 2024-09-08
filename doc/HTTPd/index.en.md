# 1 KC3 HTTPd web server

The source files for the KC3 HTTPd web server are in `/httpd`.

## 1.1 Dependencies

The web server depends on :
 - KC3
   - libkc3
   - event
   - socket
   - http
 - [SASS](https://sass-lang.com/) for CSS rendering
 - [ESBuild](https://esbuild.github.io/) for Javascript rendering

## 1.2 Configuration

The web server must be started in a directory containing the following
files :

```
./app/controllers/     # controllers directory (.kc3)
./app/templates/       # templates directory (.ekc3)
./app/views/           # views directory (.kc3)
./assets/              # assets directory
./assets/css/          # css directory
./assets/css/app.scss  # application css, compiled to ./static/_assets/app.css
./assets/js/           # javascript directory
./assets/js/app.js     # application javascript, compiled to ./static/_assets/app.js
./assets/node_modules/ # from npm
./config/routes.kc3    # dynamic routes
./static/              # for static files
./static/_assets/      # for static asset files
./static/_fonts/       # for static font files
./static/_images/      # for static image files
```
