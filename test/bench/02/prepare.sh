head -c10000000 /dev/urandom | tr '<' '.' > "$FF_B/out"
{ echo -n '<ffactor1><>'; cat "$FF_B/out"; } > "$FF_B/in"
