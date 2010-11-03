/*
 * aJson
 * streamhelper.h
 *
 *  http://interactive-matter.org/
 *
 *  This file is part of aJson.
 *
 *  aJson is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  aJson is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with aJson.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Created on: 10.10.2010
 *      Author: marcus
 */

#ifndef STREAMHELPER_H_
#define STREAMHELPER_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
  FILE*
  openStringInputStream(char* string);
  void
  closeStringInputStream(FILE* stream);
  FILE*
  openStringOutputStream(void);
  char*
  closeStringOutputStream(FILE* stream);

#ifdef __cplusplus
}
#endif
#endif /* STREAMHELPER_H_ */
