#!/bin/bash
set -e
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
asciidoctor README.adoc -o ../index1.html
cd ..
git checkout gh-pages
git pull
rm -f index.html
mv index1.html index.html
git add -u index.html
git commit -m "documentation update"
git push
git checkout master
