{
  echo -n '<ffactor1><>o<- '
  head -c10000000 /dev/urandom | tr '>' '.'
  echo -n '>k'
} > "$FF_B/in"

echo -n 'ok' > "$FF_B/out"
