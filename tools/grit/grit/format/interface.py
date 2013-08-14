#!/usr/bin/env python
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''Base classes for item formatters and file formatters.
'''


import re


class ItemFormatter(object):
  """Base class for a formatter that knows how to format a single item."""

  def Format(self, item, lang='', output_dir='.'):
    """Format the start of this item.

    Returns a Unicode string representing 'item' in the format known by this
    item formatter, for the language 'lang'.

    Args:
      item: anything.
      lang: 'en'
      output_dir: '.'

    Returns:
      A unicode string.
    """
    return ''

  def FormatEnd(self, item, lang='', output_dir='.'):
    """Format the end of this item.

    Returns a Unicode string representing the closure of 'item' in the
    format known by this item formatter, for the language 'lang'.
    Called (optionally) after the children of item have been formatted.

    Args:
      item: anything
      lang: 'en'
      output_dir: '.'

    Returns:
      A unicode string.
    """
    return ''
