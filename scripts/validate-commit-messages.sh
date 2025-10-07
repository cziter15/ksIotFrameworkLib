#!/usr/bin/env bash
set -euo pipefail

# Support both (type) format and type: or type(scope): format
PATTERN='^(\(feat|\(fix|\(docs|\(style|\(refactor|\(perf|\(test|\(chore\) .+|(feat|fix|docs|style|refactor|perf|test|chore)(\([^)]+\))?: .+)'

if [ -z "${1-}" ]; then
  echo "Usage: $0 <commit-range-or-sha1..sha2>"
  echo "Example: $0 origin/main..HEAD"
  exit 2
fi

range="$1"
bad=0
for sha in $(git rev-list "$range"); do
  header=$(git log --format=%B -n1 "$sha" | sed -n '1p')
  if ! grep -Eq "$PATTERN" <<< "$header"; then
    echo "Invalid commit message in $sha:" 
    git log --format=%B -n1 "$sha" | sed -n '1,5p'
    echo
    bad=1
  fi
done

exit "$bad"
