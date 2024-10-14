
all: build

app.css:
	sass css/app.scss ../static/_assets/app.css

app.js:
	esbuild js/app.js --bundle --target=es2016 --outdir=../static/_assets --external:/_fonts/* --external:/_images/*

build: npm app.css app.js

css: npm app.css

js: npm app.js

npm:
	npm i

.PHONY: all app.css app.js build css js npm
