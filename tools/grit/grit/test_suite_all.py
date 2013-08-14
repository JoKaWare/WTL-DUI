#!/usr/bin/env python
# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''Unit test suite that collects all test cases for GRIT.'''

import os
import sys
if __name__ == '__main__':
  sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))

import unittest


# TODO(joi) Use unittest.defaultTestLoader to automatically load tests
# from modules. Iterating over the directory and importing could then
# automate this all the way, if desired.


class TestSuiteAll(unittest.TestSuite):
  def __init__(self):
    super(type(self), self).__init__()
    # Imports placed here to prevent circular imports.
    # pylint: disable-msg=C6204
    from grit import clique_unittest
    from grit import grd_reader_unittest
    from grit import grit_runner_unittest
    from grit import lazy_re_unittest
    from grit import shortcuts_unittests
    from grit import tclib_unittest
    from grit import util_unittest
    from grit import xtb_reader_unittest
    from grit.gather import admin_template_unittest
    from grit.gather import igoogle_strings_unittest
    from grit.gather import muppet_strings_unittest
    from grit.gather import policy_json_unittest
    from grit.gather import rc_unittest
    from grit.gather import tr_html_unittest
    from grit.gather import txt_unittest
    from grit.node import base_unittest
    from grit.node import io_unittest
    from grit.node import message_unittest
    from grit.node import misc_unittest
    from grit.node.custom import filename_unittest
    from grit.tool import build_unittest
    from grit.tool import buildinfo_unittest
    from grit.tool import postprocess_unittest
    from grit.tool import preprocess_unittest
    from grit.tool import rc2grd_unittest
    from grit.tool import transl2tc_unittest
    import grit.format.c_format_unittest
    import grit.format.data_pack_unittest
    import grit.format.js_map_format_unittest
    import grit.format.rc_header_unittest
    import grit.format.rc_unittest

    test_classes = [
        admin_template_unittest.AdmGathererUnittest,
        base_unittest.NodeUnittest,
        build_unittest.BuildUnittest,
        buildinfo_unittest.BuildInfoUnittest,
        clique_unittest.MessageCliqueUnittest,
        filename_unittest.WindowsFilenameUnittest,
        grd_reader_unittest.GrdReaderUnittest,
        grit.format.c_format_unittest.CFormatUnittest,
        grit.format.data_pack_unittest.FormatDataPackUnittest,
        grit.format.js_map_format_unittest.JsMapFormatUnittest,
        grit.format.rc_header_unittest.RcHeaderFormatterUnittest,
        grit.format.rc_unittest.FormatRcUnittest,
        grit_runner_unittest.OptionArgsUnittest,
        igoogle_strings_unittest.IgoogleStringsUnittest,
        io_unittest.FileNodeUnittest,
        lazy_re_unittest.LazyReUnittest,
        message_unittest.MessageUnittest,
        misc_unittest.GritNodeUnittest,
        misc_unittest.IfNodeChildrenUnittest,
        misc_unittest.IfNodeUnittest,
        misc_unittest.ReleaseNodeUnittest,
        muppet_strings_unittest.MuppetStringsUnittest,
        policy_json_unittest.PolicyJsonUnittest,
        postprocess_unittest.PostProcessingUnittest,
        preprocess_unittest.PreProcessingUnittest,
        rc2grd_unittest.Rc2GrdUnittest,
        rc_unittest.RcUnittest,
        shortcuts_unittests.ShortcutsUnittest,
        tclib_unittest.TclibUnittest,
        tr_html_unittest.ParserUnittest,
        tr_html_unittest.TrHtmlUnittest,
        transl2tc_unittest.TranslationToTcUnittest,
        txt_unittest.TxtUnittest,
        util_unittest.UtilUnittest,
        xtb_reader_unittest.XtbReaderUnittest,
        # add test classes here...
    ]

    for test_class in test_classes:
      self.addTest(unittest.makeSuite(test_class))


if __name__ == '__main__':
  test_result = unittest.TextTestRunner(verbosity=2).run(TestSuiteAll())
  sys.exit(len(test_result.errors) + len(test_result.failures))
