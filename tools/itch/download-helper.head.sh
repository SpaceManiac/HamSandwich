#!/bin/bash
set -euo pipefail
mkdir -p installers
do_it() {
    declare filename link sha256sum file_id  # inputs
    if test ! -f "installers/$filename"; then
        curl -o "installers/$filename" "$(curl -s -X POST "$link/file/$file_id" | json_xs -e '$_ = %$_{"url"}' -t string)"
    fi
    sha256sum -c <<<"$sha256sum *installers/$filename"
}
