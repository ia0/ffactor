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

FF_D=$(readlink -f "$FF_D")

check_install()
{
  local src="$FF_D/$1" dst="$2"
  if [ ! -e "$src" ]
  then
    echo "[1;31mError:[m Install source does not exist"
    echo "Source: $src"
    exit 1
  fi
  # Do not remove installed links.
  [ -h "$dst" ] && [ "$(readlink "$dst")" = "$src" ] && return 0
  if [ -e "$dst" -o -h "$dst" ]
  then
    if [ -f "$dst" ] && diff "$dst" "$src" >/dev/null
    then
      echo "[1;33m  RM $dst (same content)[m"
      rm "$dst"
    else
      echo "[1;31mError:[m Install destination exists and differs"
      echo "Destination: $dst"
      echo "     Source: $src"
      exit 1
    fi
  fi
  echo "[34m  LN $dst[m"
  mkdir -p "$(dirname "$dst")"
  ln -s "$src" "$dst"
}
