
all: build

app.css:
	sass css/app.scss ../static/_assets/app.css

app.js:
	esbuild js/app.js --outfile=../static/_assets/app.js

build: app.css app.js

css: app.css

js: app.js

.PHONY: all app.css app.js build css js
