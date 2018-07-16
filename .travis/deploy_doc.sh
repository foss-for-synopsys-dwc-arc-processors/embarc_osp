#!/bin/sh

REPO_NAME="github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp.git"
REPO_LINK="https://""${GH_TOKEN}""@""${REPO_NAME}"

die()
{
    echo " *** ERROR: " $*
    exit 1
}

set -x

# Make documentation
echo 'Generating documentation ...'
cd ../doc/documents/example || die
ln -s ../../../example example || die
# Generate xml by doxygen
cd ../..
mkdir -p build/doxygen/xml || die
make doxygen > build_doxygen.log || die
# Generate by sphinx
make html > build_html.log || die

echo $TRAVIS_BRANCH 
echo $TRAVIS_PULL_REQUEST
# Check if this is a pull request
if [ "$TRAVIS_PULL_REQUEST" == "true" ]; then
    echo "Don't push built docs to gh-pages for pull request "
    exit 0
fi

# Check if this is master branch
# Only push doc changes to gh-pages when this is master branch
if [ "$TRAVIS_BRANCH" != "master" ]; then
    echo "Don't push built docs to gh-pages for non master branch "
    exit 0
fi

echo 'Push generated documentation to gh-pages branch...'

# Only commit changes when it is not a pull request
# tar doc
tar czvf doc.tar.gz build || die

git fetch origin || die
git branch -a || die
mkdir gh-pages || die
cd gh-pages || die
git init . || die
git remote add origin ${REPO_LINK} || die
git fetch origin -t || die
git checkout -b gh-pages origin/gh-pages || die
cd doc || die
rm -rf embARC_Document.html embARC_Document || rm -rf build
cp ../../doc.tar.gz . || die
tar xzvf doc.tar.gz || die
rm -rf doc.tar.gz || die

git add --all || die
# git commit -s -a -m "Update gh-pages branch, Travis build: $TRAVIS_BUILD_NUMBER, commit: "
git commit -s -a -m "doc: Push updated generated sphinx documentation of commit ${TRAVIS_COMMIT}" || die
if [ $? -eq 0 ]; then
    echo 'Push changes to gh-pages branch.'
    git push ${REPO_LINK} gh-pages:gh-pages > /dev/null 2>&1 || die
else
    echo 'No update in gh-pages branch, no need to push changes!'
fi

exit 0
