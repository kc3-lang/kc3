# Contributing to KC3

## Adding or removing files from the source tree
```sh
make gen
./update_sources
```

The last command will show the path to `sources.mk` and
`sources.sh` if they were modified.

Don't forget to commit your files and the modified `sources.mk` and
`sources.sh` files.

After updating `sources.sh` and `sources.mk` you need to run
`./configure` again.

## Pull requests

Please send pull requests on the
[KC3 Github repository](https://github.com/kc3-lang/kc3)
or
[KC3 Codeberg repository](https://codeberg.org/kc3-lang/kc3)
.

Please note that according to the copyright notice in all KC3 source
files, you are not allowed to fork and redistribute the project, but
still you can send pull request for incorporation.

After careful reviewing the pull request will be accepted into master
or rejected.
