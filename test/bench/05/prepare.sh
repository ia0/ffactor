n=0

{
  echo -n '<ffactor1><>'
  while [ $n -lt 500000 ]
  do
    echo -n '<if '$((n % 2))'>.<endif>'
    n=$((n + 1))
  done
} > "$FF_B/in"

{
  while [ $n -gt 0 ]
  do
    echo -n .
    n=$((n - 2))
  done
} > "$FF_B/out"
