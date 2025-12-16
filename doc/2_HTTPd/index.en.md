# 2 KC3 HTTPd web server

The source files for the KC3 HTTPd web server are in `/httpd`.

## 2.1 Dependencies

The web server depends on :
 - KC3
   - libkc3
   - event
   - socket
   - http
 - [SASS](https://sass-lang.com/) for CSS rendering
 - [ESBuild](https://esbuild.github.io/) for Javascript rendering

## 2.2 Configuration

The web server must be started in a directory containing the following
files :

```sh
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

## 2.3 Example

Run `make test_httpd` to run the KC3 HTTPd web server (`httpd/kc3_httpd`)
starting in the `test/httpd` working directory.

This directory contains an example of configuring and using `kc3_httpd`.

Static files are served from ./static and is mapped to root URL (`/`).

Documentation in Markdown and HTML format is served from the `/doc/` URL.
The documentation has a custom route defined in `./config/routes.kc3`,
a controller defined in `./app/controllers/doc_controller.kc3` and
a few templates defined in `./app/templates/doc/`.

The main layout for the application is stated in
`./app/templates/layout.html.ekc3` and `./app/templates/nav.html.ekc3`.
Notice the `.html.ekc3` extension, these are HTML files with embedded KC3.
See the [EKC3](/doc/EKC3) documentation for more information on these files.

---

Top : [KC3 Documentation](/doc/)

Previous : [1 KC3](/doc/1_KC3/)

Next : [3 Guides](/doc/3_Guides/)
