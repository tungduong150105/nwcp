#!/usr/bin/env bash
set -euo pipefail

# --- Config ---
TEMPLATE_DIR='.template'   # folders named .template are “layers”
PARENT_FILE='$PARENT'      # literal filename $PARENT marks “continue climbing”
# Filenames containing this literal text will be renamed to the problem name:
FILENAME_PLACEHOLDER='$PROBLEM_NAME'
# Inside-file placeholder (envsubst style):
CONTENT_PLACEHOLDER='${PROBLEM_NAME}'

# --- Helpers ---
have() { command -v "$1" >/dev/null 2>&1; }

# Reverse lines (tac on GNU, tail -r on BSD/macOS)
rev_lines() {
  if have tac; then tac
  else tail -r
  fi
}

# Copy directory tree CONTENTS into dest (portable cp without -T)
copy_dir_into() {
  local src="$1" dest="$2"
  mkdir -p "$dest"
  # shellcheck disable=SC2044
  for item in "$(printf %s "$src")"/*; do
    # if template dir is empty, glob returns literal; guard it
    [[ "$item" == "$src/*" ]] && break
    cp -R "$item" "$dest/"
  done
}

# Rename files that contain the literal placeholder in their NAME
rename_files_placeholder() {
  local root="$1" name="$2"
  # find all files & dirs containing the placeholder, deepest first
  # so we rename children before parents
  while IFS= read -r -d '' path; do
    local newpath="${path//$FILENAME_PLACEHOLDER/$name}"
    if [[ "$newpath" != "$path" ]]; then
      mkdir -p "$(dirname "$newpath")"
      mv "$path" "$newpath"
    fi
  done < <(find "$root" -depth -name "*$FILENAME_PLACEHOLDER*" -print0)
}

# Replace ${PROBLEM_NAME} inside files (envsubst)
subst_file_contents() {
  local root="$1" name="$2"
  export PROBLEM_NAME="$name"
  # Replace only in regular files
  while IFS= read -r -d '' f; do
    # Use a temp to avoid truncation on error
    tmp="$(mktemp)"
    # Only pass our placeholder; prevents envsubst from touching other vars
    REPLACE_STRING="$CONTENT_PLACEHOLDER"
    cat "$f" > "$tmp"
    envsubst "$REPLACE_STRING" < "$tmp" > "$f"
    rm -f "$tmp"
  done < <(find "$root" -type f -print0)
}

# Search upward for .template layers.
# Emit the directories (without appending .template) from leaf→root; we'll reverse.
search_up_templates() {
  local here
  here="$(pwd)"
  while [[ "$PWD" != "/" ]]; do
    if [[ -e "$TEMPLATE_DIR" ]]; then
      # If PARENT_FILE exists inside this template, keep climbing; else, stop after including this one.
      printf '%s\n' "$PWD"
      if [[ ! -e "$TEMPLATE_DIR/$PARENT_FILE" ]]; then
        break
      fi
    fi
    cd .. >/dev/null
  done
  cd "$here" >/dev/null
}

# --- Main ---
if [[ "$#" -lt 1 ]]; then
  echo "Usage: $0 <problem-folder> [more-folders...]" >&2
  exit 1
fi

for filepath in "$@"; do
  PROBLEM_NAME="$(basename -- "$filepath")"
  echo "Creating: $PROBLEM_NAME"

  if [[ -e "$filepath" ]]; then
    echo "  $filepath already exists. Remove it and retry."
    continue
  fi

  # Collect template layers (root-most first)
  # mapfile -t TEMPLATE_HOSTS < <(search_up_templates | rev_lines)
  TEMPLATE_HOSTS=()
  while IFS= read -r line; do
    TEMPLATE_HOSTS+=("$line")
  done < <(search_up_templates | rev_lines)
  # Append /.template to each host dir
  TEMPLATE_DIRS=()
  for d in "${TEMPLATE_HOSTS[@]}"; do
    TEMPLATE_DIRS+=("$d/$TEMPLATE_DIR")
  done
  if [[ "${#TEMPLATE_DIRS[@]}" -eq 0 ]]; then
    echo "  No '$TEMPLATE_DIR' found upwards. Nothing to copy."
    mkdir -p "$filepath"
  else
    mkdir -p "$filepath"
    for td in "${TEMPLATE_DIRS[@]}"; do
      copy_dir_into "$td" "$filepath"
    done
    # Remove the marker file if it was copied
    rm -f "$filepath/$PARENT_FILE" || true
  fi

  # Rename filenames containing $PROBLEM_NAME → actual name
  rename_files_placeholder "$filepath" "$PROBLEM_NAME"

  # Replace ${PROBLEM_NAME} in file contents
  subst_file_contents "$filepath" "$PROBLEM_NAME"

  # Run optional setup
  if [[ -x "$filepath/setup" ]]; then
    echo "  Running setup ..."
    ( cd "$filepath" && ./setup)
  fi

  echo "  Done: $filepath"
done
