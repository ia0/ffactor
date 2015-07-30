#!/bin/sh
set -e

. "$FF_R/prepare.sh"

ref=$(time -f'%U %M' "$FF_BENCH_REF" 1 < "$FF_B/in" 2>&1 > "$FF_B/tmp")
$FF_DIFF "$FF_B/in" "$FF_B/tmp"

cur=$(time -f'%U %M' "$FF_BIN" < "$FF_B/in" 2>&1 > "$FF_B/tmp")
$FF_DIFF "$FF_B/out" "$FF_B/tmp"

get_tim() {
  local i x
  x=$(printf '%.3f' $1 | tr -d .)
  for i in 1 2 3
  do x=${x#0}
  done
  echo $x
}
get_mem() { echo $2; }

compare()
{
  local msg=$1 unit=$2 cur=$3 ref=$4 rat
  echo -n "$msg ($unit): ["
  rat=$((cur * 100 / ref))
  if [ $rat -lt 200 ]
  then echo -n "32"
  elif [ $rat -lt 500 ]
  then echo -n "33"
  else echo -n "31"
  fi
  echo "m$rat%[m ($cur/$ref)"
}

tim_ref=$(get_tim $ref)
mem_ref=$(get_mem $ref)
tim_cur=$(get_tim $cur)
mem_cur=$(get_mem $cur)

compare Time ms $tim_cur $tim_ref
compare Memory kB $mem_cur $mem_ref
