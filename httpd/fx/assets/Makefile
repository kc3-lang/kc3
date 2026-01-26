## kc3
## Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

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
