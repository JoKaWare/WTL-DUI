#!/usr/bin/env python
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Miscellaneous node types.
"""

import os.path
import re
import sys

from grit import exception
from grit import constants
from grit import util
import grit.format.rc_header
from grit.node import base
from grit.node import message
from grit.node import io


# RTL languages
# TODO(jennyz): remove this fixed set of RTL language array
# now that generic expand_variable code exists.
_RTL_LANGS = (
    'ar',  # Arabic
    'fa',  # Farsi
    'iw',  # Hebrew
    'ks',  # Kashmiri
    'ku',  # Kurdish
    'ps',  # Pashto
    'ur',  # Urdu
    'yi',  # Yiddish
)


def _ReadFirstIdsFromFile(filename, defines):
  """Read the starting resource id values from |filename|.  We also
  expand variables of the form <(FOO) based on defines passed in on
  the command line.

  Returns a tuple, the absolute path of SRCDIR followed by the
  first_ids dictionary.
  """
  first_ids_dict = eval(open(filename).read())
  src_root_dir = os.path.abspath(os.path.join(os.path.dirname(filename),
                                              first_ids_dict['SRCDIR']))

  def ReplaceVariable(matchobj):
    for key, value in defines.iteritems():
      if matchobj.group(1) == key:
        value = os.path.abspath(value)[len(src_root_dir) + 1:]
        return value
    return ''

  renames = []
  for grd_filename in first_ids_dict:
    new_grd_filename = re.sub(r'<\(([A-Za-z_]+)\)', ReplaceVariable,
                              grd_filename)
    if new_grd_filename != grd_filename:
      new_grd_filename = new_grd_filename.replace('\\', '/')
      renames.append((grd_filename, new_grd_filename))

  for grd_filename, new_grd_filename in renames:
    first_ids_dict[new_grd_filename] = first_ids_dict[grd_filename]
    del(first_ids_dict[grd_filename])

  return (src_root_dir, first_ids_dict)


class IfNode(base.Node):
  """A node for conditional inclusion of resources.
  """

  def _IsValidChild(self, child):
    from grit.node import empty
    parent = self.parent
    while isinstance(parent, IfNode):
      parent = parent.parent
    assert parent, '<if> node should never be root.'
    if isinstance(child, IfNode):
      return True
    elif isinstance(parent, empty.IncludesNode):
      from grit.node import include
      return isinstance(child, include.IncludeNode)
    elif isinstance(parent, empty.MessagesNode):
      from grit.node import message
      return isinstance(child, message.MessageNode)
    elif isinstance(parent, empty.StructuresNode):
      from grit.node import structure
      return isinstance(child, structure.StructureNode)
    elif isinstance(parent, empty.OutputsNode):
      from grit.node import io
      return isinstance(child, io.OutputNode)
    elif isinstance(parent, empty.TranslationsNode):
      from grit.node import io
      return isinstance(child, io.FileNode)
    else:
      return False

  def MandatoryAttributes(self):
    return ['expr']

  def IsConditionSatisfied(self):
    """Returns true if and only if the Python expression stored in attribute
    'expr' evaluates to true.
    """
    return self.EvaluateCondition(self.attrs['expr'])

  def SatisfiesOutputCondition(self):
    """Returns true if its condition is satisfied, including on ancestors."""
    if not self.IsConditionSatisfied():
      return False
    else:
      return base.Node.SatisfiesOutputCondition(self)


class ReleaseNode(base.Node):
  """The <release> element."""

  def _IsValidChild(self, child):
    from grit.node import empty
    return isinstance(child, (empty.IncludesNode, empty.MessagesNode,
                              empty.StructuresNode, empty.IdentifiersNode))

  def _IsValidAttribute(self, name, value):
    return (
      (name == 'seq' and int(value) <= self.GetRoot().GetCurrentRelease()) or
      name == 'allow_pseudo'
    )

  def MandatoryAttributes(self):
    return ['seq']

  def DefaultAttributes(self):
    return { 'allow_pseudo' : 'true' }

  def GetReleaseNumber():
    """Returns the sequence number of this release."""
    return self.attribs['seq']

  def ItemFormatter(self, t):
    if t == 'data_package':
      from grit.format import data_pack
      return data_pack.DataPack()
    else:
      return super(type(self), self).ItemFormatter(t)

class GritNode(base.Node):
  """The <grit> root element."""

  def __init__(self):
    base.Node.__init__(self)
    self.output_language = ''
    self.defines = {}
    self.substituter = util.Substituter()

  def _IsValidChild(self, child):
    from grit.node import empty
    return isinstance(child, (ReleaseNode, empty.TranslationsNode,
                              empty.OutputsNode))

  def _IsValidAttribute(self, name, value):
    if name not in ['base_dir', 'first_ids_file', 'source_lang_id',
                    'latest_public_release', 'current_release',
                    'enc_check', 'tc_project', 'grit_version',
                    'output_all_resource_defines']:
      return False
    if name in ['latest_public_release', 'current_release'] and value.strip(
      '0123456789') != '':
      return False
    return True

  def MandatoryAttributes(self):
    return ['latest_public_release', 'current_release']

  def DefaultAttributes(self):
    return {
      'base_dir' : '.',
      'first_ids_file': '',
      'grit_version': 1,
      'source_lang_id' : 'en',
      'enc_check' : constants.ENCODING_CHECK,
      'tc_project' : 'NEED_TO_SET_tc_project_ATTRIBUTE',
      'output_all_resource_defines': 'true'
    }

  def EndParsing(self):
    base.Node.EndParsing(self)
    if (int(self.attrs['latest_public_release'])
        > int(self.attrs['current_release'])):
      raise exception.Parsing('latest_public_release cannot have a greater '
                              'value than current_release')

    self.ValidateUniqueIds()

    # Add the encoding check if it's not present (should ensure that it's always
    # present in all .grd files generated by GRIT). If it's present, assert if
    # it's not correct.
    if 'enc_check' not in self.attrs or self.attrs['enc_check'] == '':
      self.attrs['enc_check'] = constants.ENCODING_CHECK
    else:
      assert self.attrs['enc_check'] == constants.ENCODING_CHECK, (
        'Are you sure your .grd file is in the correct encoding (UTF-8)?')

  def ValidateUniqueIds(self):
    """Validate that 'name' attribute is unique in all nodes in this tree
    except for nodes that are children of <if> nodes.
    """
    unique_names = {}
    duplicate_names = []
    for node in self:
      if isinstance(node, message.PhNode):
        continue  # PhNode objects have a 'name' attribute which is not an ID
      if node.attrs.get('generateid', 'true') == 'false':
        continue  # Duplication not relevant in that case

      node_ids = node.GetTextualIds()
      if node_ids:
        for node_id in node_ids:
          if util.SYSTEM_IDENTIFIERS.match(node_id):
            continue  # predefined IDs are sometimes used more than once

          # Don't complain about duplicate IDs if they occur in a node that is
          # inside an <if> node.
          if (node_id in unique_names and node_id not in duplicate_names and
              (not node.parent or not isinstance(node.parent, IfNode))):
            duplicate_names.append(node_id)
          unique_names[node_id] = 1

    if len(duplicate_names):
      raise exception.DuplicateKey(', '.join(duplicate_names))


  def GetCurrentRelease(self):
    """Returns the current release number."""
    return int(self.attrs['current_release'])

  def GetLatestPublicRelease(self):
    """Returns the latest public release number."""
    return int(self.attrs['latest_public_release'])

  def GetSourceLanguage(self):
    """Returns the language code of the source language."""
    return self.attrs['source_lang_id']

  def GetTcProject(self):
    """Returns the name of this project in the TranslationConsole, or
    'NEED_TO_SET_tc_project_ATTRIBUTE' if it is not defined."""
    return self.attrs['tc_project']

  def SetOwnDir(self, dir):
    """Informs the 'grit' element of the directory the file it is in resides.
    This allows it to calculate relative paths from the input file, which is
    what we desire (rather than from the current path).

    Args:
      dir: r'c:\bla'

    Return:
      None
    """
    assert dir
    self.base_dir = os.path.normpath(os.path.join(dir, self.attrs['base_dir']))

  def GetBaseDir(self):
    """Returns the base directory, relative to the working directory.  To get
    the base directory as set in the .grd file, use GetOriginalBaseDir()
    """
    if hasattr(self, 'base_dir'):
      return self.base_dir
    else:
      return self.GetOriginalBaseDir()

  def GetOriginalBaseDir(self):
    """Returns the base directory, as set in the .grd file.
    """
    return self.attrs['base_dir']

  def ShouldOutputAllResourceDefines(self):
    """Returns true if all resource defines should be output, false if
    defines for resources not emitted to resource files should be
    skipped.
    """
    return self.attrs['output_all_resource_defines'] == 'true'

  def GetInputFiles(self):
    """Returns the list of files that are read to produce the output."""
    input_nodes = []

    # Importing this here avoids a circular dependency in the imports.
    # pylint: disable-msg=C6204
    from grit.node import include
    from grit.node import structure
    from grit.node import variant

    input_nodes.extend(self.GetChildrenOfType(io.FileNode))
    input_nodes.extend(self.GetChildrenOfType(include.IncludeNode))
    input_nodes.extend(self.GetChildrenOfType(structure.StructureNode))
    input_nodes.extend(self.GetChildrenOfType(variant.SkeletonNode))

    # Collect all possible output languages.
    langs = set()
    for output in self.GetOutputFiles():
      if output.attrs['lang']:
        langs.add(output.attrs['lang'])

    # Check if inputs is required for output in any language.
    # By default SatisfiesOutputCondition() check only for one language.
    result = []
    for node in input_nodes:
      insert = node.SatisfiesOutputCondition()
      old_output_language = self.output_language
      for lang in langs:
        self.SetOutputContext(lang, self.defines)
        if node.SatisfiesOutputCondition():
          insert = True
          break;
      self.SetOutputContext(old_output_language, self.defines)
      if insert:
        result.append(node)
    return result

  def GetFirstIdsFile(self):
    """Returns a usable path to the first_ids file, if set, otherwise
    returns None.

    The first_ids_file attribute is by default relative to the
    base_dir of the .grd file, but may be prefixed by GRIT_DIR/,
    which makes it relative to the directory of grit.py
    (e.g. GRIT_DIR/../gritsettings/resource_ids).
    """
    if not self.attrs['first_ids_file']:
      return None

    path = self.attrs['first_ids_file']
    GRIT_DIR_PREFIX = 'GRIT_DIR/'
    if path.startswith(GRIT_DIR_PREFIX):
      return util.PathFromRoot(path[len(GRIT_DIR_PREFIX):])
    else:
      return self.ToRealPath(path)

  def _CollectOutputFiles(self, nodes, output_files):
    """Recursively filters the list of nodes that may contain other lists
    in <if> nodes, and collects all the nodes that are not enclosed by
    unsatisfied <if> conditionals and not <if> nodes themselves.

    Args:
      nodes: The list of nodes to filter.
      output_files: The list of satisfying nodes.
    """
    for node in nodes:
      if node.name == 'if':
        if node.IsConditionSatisfied():
          self._CollectOutputFiles(node.children, output_files)
      else:
        output_files.append(node)

  def GetOutputFiles(self):
    """Returns the list of <output> nodes that are descendants of this node's
    <outputs> child and are not enclosed by unsatisfied <if> conditionals.
    """
    for child in self.children:
      if child.name == 'outputs':
        output_files = []
        self._CollectOutputFiles(child.children, output_files)
        return output_files
    raise exception.MissingElement()

  def GetSubstitutionMessages(self):
    """Returns the list of <message sub_variable="true"> nodes."""
    msg_nodes = self.GetChildrenOfType(message.MessageNode)
    return [n for n in msg_nodes if
            n.attrs['sub_variable'] == 'true' and n.SatisfiesOutputCondition()]

  def ItemFormatter(self, t):
    if t == 'rc_header':
      from grit.format import rc_header  # import here to avoid circular dep
      return rc_header.TopLevel()
    elif t in ['rc_all', 'rc_translateable', 'rc_nontranslateable']:
      from grit.format import rc  # avoid circular dep
      return rc.TopLevel()
    elif t == 'c_format':
      from grit.format import c_format
      return c_format.TopLevel()
    elif t == 'resource_map_header':
      from grit.format import resource_map
      return resource_map.HeaderTopLevel()
    elif t in ('resource_map_source', 'resource_file_map_source'):
      from grit.format import resource_map
      return resource_map.SourceTopLevel()
    elif t == 'js_map_format':
      from grit.format import js_map_format
      return js_map_format.TopLevel()
    elif t in ('adm', 'plist', 'plist_strings', 'admx', 'adml', 'doc', 'json',
        'reg'):
      from grit.format.policy_templates import template_formatter
      return template_formatter.TemplateFormatter(t)
    else:
      return super(type(self), self).ItemFormatter(t)

  def SetOutputContext(self, output_language, defines):
    """Set the output context: language and defines. Prepares substitutions.

    The substitutions are reset every time the OutputContext is changed.
    They include messages designated as variables, and language codes for html
    and rc files.

    Args:
      output_language: a two-letter language code (eg: 'en', 'ar'...) or ''
      defines: a map of names to values (strings or booleans.)
    """
    # We do not specify the output language for .grh files; so we get an empty
    # string as the default. The value should match
    # grit.clique.MessageClique.source_language.
    self.output_language = output_language or self.GetSourceLanguage()
    self.defines = defines
    self.substituter.AddMessages(self.GetSubstitutionMessages(),
                                 self.output_language)
    if self.output_language in _RTL_LANGS:
      direction = 'dir="RTL"'
    else:
      direction = 'dir="LTR"'
    self.substituter.AddSubstitutions({
        'GRITLANGCODE': self.output_language,
        'GRITDIR': direction,
    })
    from grit.format import rc  # avoid circular dep
    rc.RcSubstitutions(self.substituter, self.output_language)

  def SetDefines(self, defines):
    self.defines = defines

  def AssignFirstIds(self, filename_or_stream, defines):
    """Assign first ids to each grouping node based on values from the
    first_ids file (if specified on the <grit> node).
    """
    # If the input is a stream, then we're probably in a unit test and
    # should skip this step.
    if type(filename_or_stream) not in (str, unicode):
      return

    # Nothing to do if the first_ids_filename attribute isn't set.
    first_ids_filename = self.GetFirstIdsFile()
    if not first_ids_filename:
      return

    src_root_dir, first_ids = _ReadFirstIdsFromFile(first_ids_filename,
                                                    defines)
    from grit.node import empty
    for node in self.inorder():
      if isinstance(node, empty.GroupingNode):
        filename = os.path.abspath(filename_or_stream)[
          len(src_root_dir) + 1:]
        filename = filename.replace('\\', '/')

        if node.attrs['first_id'] != '':
          raise Exception(
              "Don't set the first_id attribute when using the first_ids_file "
              "attribute on the <grit> node, update %s instead." %
              first_ids_filename)

        try:
          id_list = first_ids[filename][node.name]
        except KeyError, e:
          print '-' * 78
          print 'Resource id not set for %s (%s)!' % (filename, node.name)
          print ('Please update %s to include an entry for %s.  See the '
                 'comments in resource_ids for information on why you need to '
                 'update that file.' % (first_ids_filename, filename))
          print '-' * 78
          raise e

        try:
          node.attrs['first_id'] = str(id_list.pop(0))
        except IndexError, e:
          raise Exception('Please update %s and add a first id for %s (%s).'
                          % (first_ids_filename, filename, node.name))

  def RunGatherers(self, recursive=0, debug=False):
    """Gathers information for the top-level nodes, then apply substitutions,
    then gather the <translations> node (all substitutions must be done before
    it is gathered so that they can match up correctly).

    The substitutions step requires that the OutputContext has been set.
    Locally, get the Substitution messages
    and add them to the substituter. Also add substitutions for language codes
    in the Rc.

    Gatherers for <translations> child nodes will always be run after all other
    child nodes have been gathered.

    Args:
      recursive: will call RunGatherers() recursively on all child nodes first.
      debug: will print information while running gatherers.
    """
    if recursive:
      process_last = []
      for child in self.children:
        if child.name == 'translations':
          process_last.append(child)
        else:
          child.RunGatherers(recursive=recursive, debug=debug)

      assert self.output_language
      self.SubstituteMessages(self.substituter)

      for child in process_last:
        child.RunGatherers(recursive=recursive, debug=debug)


class IdentifierNode(base.Node):
  """A node for specifying identifiers that should appear in the resource
  header file, and be unique amongst all other resource identifiers, but don't
  have any other attributes or reference any resources.
  """

  def MandatoryAttributes(self):
    return ['name']

  def DefaultAttributes(self):
    return { 'comment' : '', 'id' : '', 'systemid': 'false' }

  def ItemFormatter(self, t):
    if t == 'rc_header':
      return grit.format.rc_header.Item()

  def GetId(self):
    """Returns the id of this identifier if it has one, None otherwise
    """
    if 'id' in self.attrs:
      return self.attrs['id']
    return None

  def EndParsing(self):
    """Handles system identifiers."""
    base.Node.EndParsing(self)
    if self.attrs['systemid'] == 'true':
      util.SetupSystemIdentifiers((self.attrs['name'],))

  @staticmethod
  def Construct(parent, name, id, comment, systemid='false'):
    """Creates a new node which is a child of 'parent', with attributes set
    by parameters of the same name.
    """
    node = IdentifierNode()
    node.StartParsing('identifier', parent)
    node.HandleAttribute('name', name)
    node.HandleAttribute('id', id)
    node.HandleAttribute('comment', comment)
    node.HandleAttribute('systemid', systemid)
    node.EndParsing()
    return node
