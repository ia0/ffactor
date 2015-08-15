{
  echo -n '<ffactor1><>'
  n=0
  while [ $n -lt 500000 ]
  do
    echo -n '<if 1>'
    n=$((n + 1))
  done
  echo -n .
  while [ $n -gt 0 ]
  do
    echo -n '<endif>'
    n=$((n - 1))
  done
} > "$FF_B/in"

echo -n . > "$FF_B/out"
