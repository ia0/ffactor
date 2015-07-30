head -c5000000 /dev/urandom | tr '<' '.' > "$FF_B/tmp1"
head -c5000000 /dev/urandom | tr '<' '.' > "$FF_B/tmp2"

{
  echo -n '<ffactor1><><if 1>'
  cat "$FF_B/tmp1"
  echo -n 'ok<endif>'
  cat "$FF_B/tmp2"
} > "$FF_B/in"

{
  cat "$FF_B/tmp1"
  echo -n 'ok'
  cat "$FF_B/tmp2"
} > "$FF_B/out"

rm "$FF_B/tmp1"
rm "$FF_B/tmp2"
