head -c10000000 /dev/urandom | tr r R > "$FF_B/out"
{ cat "$FF_B/out"; echo -n '<ffactor1><>'; } > "$FF_B/in"
