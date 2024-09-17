
all: build

app.css:
	sass css/app.scss ../static/_assets/app.css

app.js:
	esbuild js/app.js --bundle --target=es2016 --outdir=../static/_assets --external:/_fonts/* --external:/_images/*

build: app.css app.js

css: app.css

js: app.js

.PHONY: all app.css app.js build css js
