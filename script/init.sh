# ffactor expands factored files according to an environment.
# Copyright (C) 2015  ia0
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

valid_key()
{
  [ -z "$(echo -n "$1" | tr -d 'A-Za-z0-9/_-')" ] &&
  [ -z "$(echo -n "$1" | head -c1 | tr -d 'A-Za-z')" ]
}

set_cat()
{
  local i t
  echo "- $1"
  echo -n "$1:" >&2
  while read i t
  do
    if ! valid_key "$2$i"
    then
      echo -n " [33m$i[m" >&2
      continue
    fi
    if $t 2>/dev/null >/dev/null
    then
      echo "set $2$i"
      echo -n " [32m$i[m" >&2
    else
      echo -n " [31m$i[m" >&2
    fi
  done
  echo >&2
}

set_bin()
{
  local i n
  n="$1"
  shift
  for i in "$@"
  do
    echo "$i which $i"
  done |
  set_cat "$n" bin/
}
