set -e
asciidoctor README.adoc -o index1.html
git checkout gh-pages
rm index.html
mv index1.html index.html
git add -u index.html
git commit -m "documentation update"
git push
git checkout master
