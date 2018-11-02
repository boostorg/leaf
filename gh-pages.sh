set -e
asciidoctor README.adoc -o index1.html -a source-highlighter=coderay
git checkout gh-pages
git pull
rm -f index.html
mv index1.html index.html
git add -u index.html
git commit -m "documentation update"
git push
git checkout master
