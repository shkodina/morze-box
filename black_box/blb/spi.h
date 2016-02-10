/*
  Copyright 2009  Mathieu SONET (contact [at] elasticsheep [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef MYSPIH
#define MYSPIH

#include "integer.h"

void spi_init(void);
void spi_send_byte(BYTE byte);
BYTE spi_receive_byte(void);
BYTE spi_loopback(BYTE byte);
void spi_send(BYTE* data, USHORT length);
void spi_receive(BYTE* data, USHORT length);

#endif
