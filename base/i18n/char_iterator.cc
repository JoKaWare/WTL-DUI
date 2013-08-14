// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/i18n/char_iterator.h"
//
//#include "unicode/utf8.h"
//#include "unicode/utf16.h"

namespace base {
namespace i18n {

/**
 * Get a code point from a string at a code point boundary offset,
 * and advance the offset to the next code point boundary.
 * (Post-incrementing forward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The offset may point to the lead byte of a multi-byte sequence,
 * in which case the macro will read the whole sequence.
 * If the offset points to a trail byte or an illegal UTF-8 sequence, then
 * c is set to a negative value.
 *
 * @param s const uint8_t * string
 * @param i string offset, must be i<length
 * @param length string length
 * @param c output UChar32 variable, set to <0 in case of an error
 * @see U8_NEXT_UNSAFE
 * @stable ICU 2.4
 */
#define UChar int32
#define U_SENTINEL -1
#define U8_NEXT(s, i, length, c) { \
    (c)=(uint8_t)(s)[(i)++]; \
    if((c)>=0x80) { \
        uint8_t __t1, __t2; \
        if( /* handle U+1000..U+CFFF inline */ \
            (0xe0<(c) && (c)<=0xec) && \
            (((i)+1)<(length)) && \
            (__t1=(uint8_t)((s)[i]-0x80))<=0x3f && \
            (__t2=(uint8_t)((s)[(i)+1]-0x80))<= 0x3f \
        ) { \
            /* no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (UChar) */ \
            (c)=(UChar)(((c)<<12)|(__t1<<6)|__t2); \
            (i)+=2; \
        } else if( /* handle U+0080..U+07FF inline */ \
            ((c)<0xe0 && (c)>=0xc2) && \
            ((i)<(length)) && \
            (__t1=(uint8_t)((s)[i]-0x80))<=0x3f \
        ) { \
            (c)=(UChar)((((c)&0x1f)<<6)|__t1); \
            ++(i); \
        }  else { \
            (c)=U_SENTINEL; \
        } \
    } \
}

void  U16_NEXT( const char16* s, int32& i, int32& length, int32& c )
{
	 c=s[i++]; 
}

UTF8CharIterator::UTF8CharIterator(const std::string* str)
    : str_(reinterpret_cast<const uint8_t*>(str->data())),
      len_(str->size()),
      array_pos_(0),
      next_pos_(0),
      char_pos_(0),
      char_(0) {
  if (len_)
    U8_NEXT(str_, next_pos_, len_, char_);
}

UTF8CharIterator::~UTF8CharIterator() {
}

bool UTF8CharIterator::Advance() {
  if (array_pos_ >= len_)
    return false;

  array_pos_ = next_pos_;
  char_pos_++;
  if (next_pos_ < len_)
    U8_NEXT(str_, next_pos_, len_, char_);

  return true;
}

UTF16CharIterator::UTF16CharIterator(const string16* str)
    : str_(reinterpret_cast<const char16*>(str->data())),
      len_(str->size()),
      array_pos_(0),
      next_pos_(0),
      char_pos_(0),
      char_(0) {
  if (len_)
    ReadChar();
}

UTF16CharIterator::UTF16CharIterator(const char16* str, size_t str_len)
    : str_(str),
      len_(str_len),
      array_pos_(0),
      next_pos_(0),
      char_pos_(0),
      char_(0) {
  if (len_)
    ReadChar();
}

UTF16CharIterator::~UTF16CharIterator() {
}

bool UTF16CharIterator::Advance() {
  if (array_pos_ >= len_)
    return false;

  array_pos_ = next_pos_;
  char_pos_++;
  if (next_pos_ < len_)
    ReadChar();

  return true;
}

void UTF16CharIterator::ReadChar() {
  // This is actually a huge macro, so is worth having in a separate function.
  U16_NEXT(str_, next_pos_, len_, char_);
}

}  // namespace i18n
}  // namespace base
