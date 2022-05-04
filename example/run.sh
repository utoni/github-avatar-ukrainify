#!/usr/bin/env sh

set -e
set -x

USERNAME="${1:-utoni}"
MYPATH="$(dirname ${0})"
cd "${MYPATH}"
make -C ..
../github-avatar-ukrainify "${USERNAME}"
