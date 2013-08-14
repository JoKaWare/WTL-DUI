#!/usr/bin/env python
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''The <structure> element.
'''

import os

from grit.node import base
from grit.node import variant

from grit import constants
from grit import exception
from grit import util

import grit.gather.admin_template
import grit.gather.igoogle_strings
import grit.gather.muppet_strings
import grit.gather.policy_json
import grit.gather.rc
import grit.gather.tr_html
import grit.gather.txt

import grit.format.rc
import grit.format.rc_header

# Type of the gatherer to use for each type attribute
_GATHERERS = {
  'accelerators' : grit.gather.rc.Accelerators,
  'admin_template' : grit.gather.admin_template.AdmGatherer,
  'dialog'  : grit.gather.rc.Dialog,
  'igoogle' : grit.gather.igoogle_strings.IgoogleStrings,
  'menu'    : grit.gather.rc.Menu,
  'muppet'  : grit.gather.muppet_strings.MuppetStrings,
  'rcdata'  : grit.gather.rc.RCData,
  'tr_html' : grit.gather.tr_html.TrHtml,
  'txt'     : grit.gather.txt.TxtFile,
  'version' : grit.gather.rc.Version,
  'policy_template_metafile' : grit.gather.policy_json.PolicyJson,
}


# Formatter instance to use for each type attribute
# when formatting .rc files.
_RC_FORMATTERS = {
  'accelerators' : grit.format.rc.RcSection(),
  'admin_template' : grit.format.rc.RcInclude('ADM'),
  'dialog'  : grit.format.rc.RcSection(),
  'igoogle' : grit.format.rc.RcInclude('XML'),
  'menu'    : grit.format.rc.RcSection(),
  'muppet'  : grit.format.rc.RcInclude('XML'),
  'rcdata'  : grit.format.rc.RcSection(),
  'tr_html' : grit.format.rc.RcInclude('HTML'),
  'txt'     : grit.format.rc.RcInclude('TXT'),
  'version' : grit.format.rc.RcSection(),
  'policy_template_metafile': None,
}


# TODO(joi) Print a warning if the 'variant_of_revision' attribute indicates
# that a skeleton variant is older than the original file.


class StructureNode(base.Node):
  '''A <structure> element.'''

  def __init__(self):
    base.Node.__init__(self)
    self.gatherer = None
    self.skeletons = {}  # expressions to skeleton gatherers

  def _IsValidChild(self, child):
    return isinstance(child, variant.SkeletonNode)

  def MandatoryAttributes(self):
    return ['type', 'name', 'file']

  def DefaultAttributes(self):
    return { 'encoding' : 'cp1252',
             'exclude_from_rc' : 'false',
             'line_end' : 'unix',
             'output_encoding' : 'utf-8',
             'generateid': 'true',
             'expand_variables' : 'false',
             'output_filename' : '',
             'fold_whitespace': 'false',
             'run_command' : '',
             # TODO(joi) this is a hack - should output all generated files
             # as SCons dependencies; however, for now there is a bug I can't
             # find where GRIT doesn't build the matching fileset, therefore
             # this hack so that only the files you really need are marked as
             # dependencies.
             'sconsdep' : 'false',
             }

  def IsExcludedFromRc(self):
    return self.attrs['exclude_from_rc'] == 'true'

  def GetLineEnd(self):
    '''Returns the end-of-line character or characters for files output because
    of this node ('\r\n', '\n', or '\r' depending on the 'line_end' attribute).
    '''
    if self.attrs['line_end'] == 'unix':
      return '\n'
    elif self.attrs['line_end'] == 'windows':
      return '\r\n'
    elif self.attrs['line_end'] == 'mac':
      return '\r'
    else:
      raise exception.UnexpectedAttribute(
        "Attribute 'line_end' must be one of 'linux' (default), 'windows' or 'mac'")

  def GetCliques(self):
    if self.gatherer:
      return self.gatherer.GetCliques()
    else:
      return []

  def GetTextualIds(self):
    if self.gatherer and self.attrs['type'] not in ['tr_html', 'admin_template', 'txt']:
      return self.gatherer.GetTextualIds()
    else:
      return [self.attrs['name']]

  def ItemFormatter(self, t):
    if t == 'rc_header':
      return grit.format.rc_header.Item()
    elif (t in ['rc_all', 'rc_translateable', 'rc_nontranslateable'] and
          self.SatisfiesOutputCondition()):
      return _RC_FORMATTERS[self.attrs['type']]
    else:
      return super(type(self), self).ItemFormatter(t)

  def RunGatherers(self, recursive=False, debug=False):
    if self.gatherer:
      return  # idempotent

    gathertype = _GATHERERS[self.attrs['type']]

    if debug:
      print 'Running gatherer %s for file %s' % (str(gathertype), self.FilenameToOpen())

    self.gatherer = gathertype.FromFile(self.FilenameToOpen(),
                                        self.attrs['name'],
                                        self.attrs['encoding'])
    self.gatherer.SetUberClique(self.UberClique())
    self.gatherer.SetAttributes(self.attrs)
    self.gatherer.Parse()

    for child in self.children:
      assert isinstance(child, variant.SkeletonNode)
      skel = gathertype.FromFile(child.FilenameToOpen(),
                                 self.attrs['name'],
                                 child.GetEncodingToUse())
      skel.SetUberClique(self.UberClique())
      skel.SetSkeleton(True)
      skel.Parse()
      self.skeletons[child.attrs['expr']] = skel

  def GetSkeletonGatherer(self):
    '''Returns the gatherer for the alternate skeleton that should be used,
    based on the expressions for selecting skeletons, or None if the skeleton
    from the English version of the structure should be used.
    '''
    for expr in self.skeletons:
      if self.EvaluateCondition(expr):
        return self.skeletons[expr]
    return None

  def GetFilePath(self):
    return self.ToRealPath(self.attrs['file'])

  def HasFileForLanguage(self):
    return self.attrs['type'] in [
        'tr_html', 'admin_template', 'txt', 'muppet', 'igoogle']

  def ExpandVariables(self):
    '''Variable expansion on structures is controlled by an XML attribute.

    However, old files assume that expansion is always on for Rc files.

    Returns:
      A boolean.
    '''
    attrs = self.GetRoot().attrs
    if 'grit_version' in attrs and attrs['grit_version'] > 1:
      return self.attrs['expand_variables'] == 'true'
    else:
      return (self.attrs['expand_variables'] == 'true' or
              self.attrs['file'].lower().endswith('.rc'))

  def FileForLanguage(self, lang, output_dir, create_file=True,
                      return_if_not_generated=True):
    '''Returns the filename of the file associated with this structure,
    for the specified language.

    Args:
      lang: 'fr'
      output_dir: 'c:\temp'
      create_file: True
    '''
    assert self.HasFileForLanguage()
    # If the source language is requested, and no extra changes are requested,
    # use the existing file.
    if (lang == self.GetRoot().GetSourceLanguage() and
        self.attrs['expand_variables'] != 'true' and
        not self.attrs['run_command']):
      if return_if_not_generated:
        return self.GetFilePath()
      else:
        return None

    if self.attrs['output_filename'] != '':
      filename = self.attrs['output_filename']
    else:
      filename = os.path.basename(self.attrs['file'])
    assert len(filename)
    filename = '%s_%s' % (lang, filename)
    filename = os.path.join(output_dir, filename)

    # Only create the output if it was requested by the call.
    if create_file:
      text = self.gatherer.Translate(
          lang,
          pseudo_if_not_available=self.PseudoIsAllowed(),
          fallback_to_english=self.ShouldFallbackToEnglish(),
          skeleton_gatherer=self.GetSkeletonGatherer())

      file_object = util.WrapOutputStream(file(filename, 'wb'),
                                          self._GetOutputEncoding())
      file_contents = util.FixLineEnd(text, self.GetLineEnd())
      if self.ExpandVariables():
        # Note that we reapply substitution a second time here.
        # This is because a) we need to look inside placeholders
        # b) the substitution values are language-dependent
        file_contents = self.GetRoot().substituter.Substitute(file_contents)
      if self._ShouldAddBom():
        file_object.write(constants.BOM)
      file_object.write(file_contents)
      file_object.close()

      if self.attrs['run_command']:
        # Run arbitrary commands after translation is complete so that it
        # doesn't interfere with what's in translation console.
        command = self.attrs['run_command'] % {'filename': filename}
        result = os.system(command)
        assert result == 0, '"%s" failed.' % command

    return filename

  def _GetOutputEncoding(self):
    '''Python doesn't natively support UTF encodings with a BOM signature,
    so we add support by allowing you to append '-sig' to the encoding name.
    This function returns the specified output encoding minus that part.
    '''
    enc = self.attrs['output_encoding']
    if enc.endswith('-sig'):
      return enc[0:len(enc) - len('-sig')]
    else:
      return enc

  def _ShouldAddBom(self):
    '''Returns true if output files should have the Unicode BOM prepended.
    '''
    return self.attrs['output_encoding'].endswith('-sig')

  @staticmethod
  def Construct(parent, name, type, file, encoding='cp1252'):
    '''Creates a new node which is a child of 'parent', with attributes set
    by parameters of the same name.
    '''
    node = StructureNode()
    node.StartParsing('structure', parent)
    node.HandleAttribute('name', name)
    node.HandleAttribute('type', type)
    node.HandleAttribute('file', file)
    node.HandleAttribute('encoding', encoding)
    node.EndParsing()
    return node

  def SubstituteMessages(self, substituter):
    '''Propagates substitution to gatherer.

    Args:
      substituter: a grit.util.Substituter object.
    '''
    assert self.gatherer
    if self.ExpandVariables():
      self.gatherer.SubstituteMessages(substituter)

