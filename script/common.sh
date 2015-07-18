# FFactor processes factored file into developed files.
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

get_B()
{
  echo build
}

get_C()
{
  local c
  for c in clang gcc
  do
    basename "$(which $c)"
  done
}

get_E()
{
  echo n
  echo y
}

get_O()
{
  seq 0 3
  echo s
}

get_V()
{
  echo n
  echo y
}
