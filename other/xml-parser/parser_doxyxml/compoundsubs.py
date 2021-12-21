#!/usr/bin/env python

#
# Generated Sun Jun 16 17:04:33 2019 by generateDS.py version 2.32.0.
# Python 3.6.8 (default, Jan 14 2019, 11:02:34)  [GCC 8.0.1 20180414 (experimental) [trunk revision 259383]]
#
# Command line options:
#   ('-o', 'compound.py')
#   ('-s', 'compoundsubs.py')
#
# Command line arguments:
#   compound.xsd
#
# Command line:
#   /usr/local/bin/generateDS -o "compound.py" -s "compoundsubs.py" compound.xsd
#
# Current working directory (os.getcwd()):
#   parser_doxyxml
#

import os
import sys
from lxml import etree as etree_

import ??? as supermod

def parsexml_(infile, parser=None, **kwargs):
    if parser is None:
        # Use the lxml ElementTree compatible parser so that, e.g.,
        #   we ignore comments.
        parser = etree_.ETCompatXMLParser()
    try:
        if isinstance(infile, os.PathLike):
            infile = os.path.join(infile)
    except AttributeError:
        pass
    doc = etree_.parse(infile, parser=parser, **kwargs)
    return doc

#
# Globals
#

ExternalEncoding = ''

#
# Data representation classes
#


class DoxygenTypeSub(supermod.DoxygenType):
    def __init__(self, version=None, compounddef=None, **kwargs_):
        super(DoxygenTypeSub, self).__init__(version, compounddef,  **kwargs_)
supermod.DoxygenType.subclass = DoxygenTypeSub
# end class DoxygenTypeSub


class compounddefTypeSub(supermod.compounddefType):
    def __init__(self, id=None, kind=None, language=None, prot=None, final=None, sealed=None, abstract=None, compoundname=None, title=None, basecompoundref=None, derivedcompoundref=None, includes=None, includedby=None, incdepgraph=None, invincdepgraph=None, innerdir=None, innerfile=None, innerclass=None, innernamespace=None, innerpage=None, innergroup=None, templateparamlist=None, sectiondef=None, tableofcontents=None, briefdescription=None, detaileddescription=None, inheritancegraph=None, collaborationgraph=None, programlisting=None, location=None, listofallmembers=None, **kwargs_):
        super(compounddefTypeSub, self).__init__(id, kind, language, prot, final, sealed, abstract, compoundname, title, basecompoundref, derivedcompoundref, includes, includedby, incdepgraph, invincdepgraph, innerdir, innerfile, innerclass, innernamespace, innerpage, innergroup, templateparamlist, sectiondef, tableofcontents, briefdescription, detaileddescription, inheritancegraph, collaborationgraph, programlisting, location, listofallmembers,  **kwargs_)
supermod.compounddefType.subclass = compounddefTypeSub
# end class compounddefTypeSub


class listofallmembersTypeSub(supermod.listofallmembersType):
    def __init__(self, member=None, **kwargs_):
        super(listofallmembersTypeSub, self).__init__(member,  **kwargs_)
supermod.listofallmembersType.subclass = listofallmembersTypeSub
# end class listofallmembersTypeSub


class memberRefTypeSub(supermod.memberRefType):
    def __init__(self, refid=None, prot=None, virt=None, ambiguityscope=None, scope=None, name=None, **kwargs_):
        super(memberRefTypeSub, self).__init__(refid, prot, virt, ambiguityscope, scope, name,  **kwargs_)
supermod.memberRefType.subclass = memberRefTypeSub
# end class memberRefTypeSub


class docHtmlOnlyTypeSub(supermod.docHtmlOnlyType):
    def __init__(self, block=None, valueOf_=None, **kwargs_):
        super(docHtmlOnlyTypeSub, self).__init__(block, valueOf_,  **kwargs_)
supermod.docHtmlOnlyType.subclass = docHtmlOnlyTypeSub
# end class docHtmlOnlyTypeSub


class compoundRefTypeSub(supermod.compoundRefType):
    def __init__(self, refid=None, prot=None, virt=None, valueOf_=None, **kwargs_):
        super(compoundRefTypeSub, self).__init__(refid, prot, virt, valueOf_,  **kwargs_)
supermod.compoundRefType.subclass = compoundRefTypeSub
# end class compoundRefTypeSub


class reimplementTypeSub(supermod.reimplementType):
    def __init__(self, refid=None, valueOf_=None, **kwargs_):
        super(reimplementTypeSub, self).__init__(refid, valueOf_,  **kwargs_)
supermod.reimplementType.subclass = reimplementTypeSub
# end class reimplementTypeSub


class incTypeSub(supermod.incType):
    def __init__(self, refid=None, local=None, valueOf_=None, **kwargs_):
        super(incTypeSub, self).__init__(refid, local, valueOf_,  **kwargs_)
supermod.incType.subclass = incTypeSub
# end class incTypeSub


class refTypeSub(supermod.refType):
    def __init__(self, refid=None, prot=None, valueOf_=None, **kwargs_):
        super(refTypeSub, self).__init__(refid, prot, valueOf_,  **kwargs_)
supermod.refType.subclass = refTypeSub
# end class refTypeSub


class refTextTypeSub(supermod.refTextType):
    def __init__(self, refid=None, kindref=None, external=None, tooltip=None, valueOf_=None, **kwargs_):
        super(refTextTypeSub, self).__init__(refid, kindref, external, tooltip, valueOf_,  **kwargs_)
supermod.refTextType.subclass = refTextTypeSub
# end class refTextTypeSub


class sectiondefTypeSub(supermod.sectiondefType):
    def __init__(self, kind=None, header=None, description=None, memberdef=None, **kwargs_):
        super(sectiondefTypeSub, self).__init__(kind, header, description, memberdef,  **kwargs_)
supermod.sectiondefType.subclass = sectiondefTypeSub
# end class sectiondefTypeSub


class memberdefTypeSub(supermod.memberdefType):
    def __init__(self, kind=None, id=None, prot=None, static=None, strong=None, const=None, explicit=None, inline=None, refqual=None, virt=None, volatile=None, mutable=None, noexcept=None, constexpr=None, readable=None, writable=None, initonly=None, settable=None, privatesettable=None, protectedsettable=None, gettable=None, privategettable=None, protectedgettable=None, final=None, sealed=None, new=None, add=None, remove=None, raise_=None, optional=None, required=None, accessor=None, attribute=None, property=None, readonly=None, bound=None, removable=None, contrained=None, transient=None, maybevoid=None, maybedefault=None, maybeambiguous=None, templateparamlist=None, type_=None, definition=None, argsstring=None, name=None, read=None, write=None, bitfield=None, reimplements=None, reimplementedby=None, param=None, enumvalue=None, initializer=None, exceptions=None, briefdescription=None, detaileddescription=None, inbodydescription=None, location=None, references=None, referencedby=None, **kwargs_):
        super(memberdefTypeSub, self).__init__(kind, id, prot, static, strong, const, explicit, inline, refqual, virt, volatile, mutable, noexcept, constexpr, readable, writable, initonly, settable, privatesettable, protectedsettable, gettable, privategettable, protectedgettable, final, sealed, new, add, remove, raise_, optional, required, accessor, attribute, property, readonly, bound, removable, contrained, transient, maybevoid, maybedefault, maybeambiguous, templateparamlist, type_, definition, argsstring, name, read, write, bitfield, reimplements, reimplementedby, param, enumvalue, initializer, exceptions, briefdescription, detaileddescription, inbodydescription, location, references, referencedby,  **kwargs_)
supermod.memberdefType.subclass = memberdefTypeSub
# end class memberdefTypeSub


class descriptionTypeSub(supermod.descriptionType):
    def __init__(self, title=None, para=None, internal=None, sect1=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(descriptionTypeSub, self).__init__(title, para, internal, sect1, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.descriptionType.subclass = descriptionTypeSub
# end class descriptionTypeSub


class enumvalueTypeSub(supermod.enumvalueType):
    def __init__(self, id=None, prot=None, name=None, initializer=None, briefdescription=None, detaileddescription=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(enumvalueTypeSub, self).__init__(id, prot, name, initializer, briefdescription, detaileddescription, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.enumvalueType.subclass = enumvalueTypeSub
# end class enumvalueTypeSub


class templateparamlistTypeSub(supermod.templateparamlistType):
    def __init__(self, param=None, **kwargs_):
        super(templateparamlistTypeSub, self).__init__(param,  **kwargs_)
supermod.templateparamlistType.subclass = templateparamlistTypeSub
# end class templateparamlistTypeSub


class paramTypeSub(supermod.paramType):
    def __init__(self, type_=None, declname=None, defname=None, array=None, defval=None, typeconstraint=None, briefdescription=None, **kwargs_):
        super(paramTypeSub, self).__init__(type_, declname, defname, array, defval, typeconstraint, briefdescription,  **kwargs_)
supermod.paramType.subclass = paramTypeSub
# end class paramTypeSub


class linkedTextTypeSub(supermod.linkedTextType):
    def __init__(self, ref=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(linkedTextTypeSub, self).__init__(ref, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.linkedTextType.subclass = linkedTextTypeSub
# end class linkedTextTypeSub


class graphTypeSub(supermod.graphType):
    def __init__(self, node=None, **kwargs_):
        super(graphTypeSub, self).__init__(node,  **kwargs_)
supermod.graphType.subclass = graphTypeSub
# end class graphTypeSub


class nodeTypeSub(supermod.nodeType):
    def __init__(self, id=None, label=None, link=None, childnode=None, **kwargs_):
        super(nodeTypeSub, self).__init__(id, label, link, childnode,  **kwargs_)
supermod.nodeType.subclass = nodeTypeSub
# end class nodeTypeSub


class childnodeTypeSub(supermod.childnodeType):
    def __init__(self, refid=None, relation=None, edgelabel=None, **kwargs_):
        super(childnodeTypeSub, self).__init__(refid, relation, edgelabel,  **kwargs_)
supermod.childnodeType.subclass = childnodeTypeSub
# end class childnodeTypeSub


class linkTypeSub(supermod.linkType):
    def __init__(self, refid=None, external=None, **kwargs_):
        super(linkTypeSub, self).__init__(refid, external,  **kwargs_)
supermod.linkType.subclass = linkTypeSub
# end class linkTypeSub


class listingTypeSub(supermod.listingType):
    def __init__(self, filename=None, codeline=None, **kwargs_):
        super(listingTypeSub, self).__init__(filename, codeline,  **kwargs_)
supermod.listingType.subclass = listingTypeSub
# end class listingTypeSub


class codelineTypeSub(supermod.codelineType):
    def __init__(self, lineno=None, refid=None, refkind=None, external=None, highlight=None, **kwargs_):
        super(codelineTypeSub, self).__init__(lineno, refid, refkind, external, highlight,  **kwargs_)
supermod.codelineType.subclass = codelineTypeSub
# end class codelineTypeSub


class highlightTypeSub(supermod.highlightType):
    def __init__(self, class_=None, sp=None, ref=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(highlightTypeSub, self).__init__(class_, sp, ref, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.highlightType.subclass = highlightTypeSub
# end class highlightTypeSub


class spTypeSub(supermod.spType):
    def __init__(self, value=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(spTypeSub, self).__init__(value, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.spType.subclass = spTypeSub
# end class spTypeSub


class referenceTypeSub(supermod.referenceType):
    def __init__(self, refid=None, compoundref=None, startline=None, endline=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(referenceTypeSub, self).__init__(refid, compoundref, startline, endline, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.referenceType.subclass = referenceTypeSub
# end class referenceTypeSub


class locationTypeSub(supermod.locationType):
    def __init__(self, file=None, line=None, column=None, declfile=None, declline=None, declcolumn=None, bodyfile=None, bodystart=None, bodyend=None, **kwargs_):
        super(locationTypeSub, self).__init__(file, line, column, declfile, declline, declcolumn, bodyfile, bodystart, bodyend,  **kwargs_)
supermod.locationType.subclass = locationTypeSub
# end class locationTypeSub


class docSect1TypeSub(supermod.docSect1Type):
    def __init__(self, id=None, title=None, para=None, internal=None, sect2=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docSect1TypeSub, self).__init__(id, title, para, internal, sect2, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docSect1Type.subclass = docSect1TypeSub
# end class docSect1TypeSub


class docSect2TypeSub(supermod.docSect2Type):
    def __init__(self, id=None, title=None, para=None, sect3=None, internal=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docSect2TypeSub, self).__init__(id, title, para, sect3, internal, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docSect2Type.subclass = docSect2TypeSub
# end class docSect2TypeSub


class docSect3TypeSub(supermod.docSect3Type):
    def __init__(self, id=None, title=None, para=None, sect4=None, internal=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docSect3TypeSub, self).__init__(id, title, para, sect4, internal, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docSect3Type.subclass = docSect3TypeSub
# end class docSect3TypeSub


class docSect4TypeSub(supermod.docSect4Type):
    def __init__(self, id=None, title=None, para=None, internal=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docSect4TypeSub, self).__init__(id, title, para, internal, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docSect4Type.subclass = docSect4TypeSub
# end class docSect4TypeSub


class docInternalTypeSub(supermod.docInternalType):
    def __init__(self, para=None, sect1=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docInternalTypeSub, self).__init__(para, sect1, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docInternalType.subclass = docInternalTypeSub
# end class docInternalTypeSub


class docInternalS1TypeSub(supermod.docInternalS1Type):
    def __init__(self, para=None, sect2=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docInternalS1TypeSub, self).__init__(para, sect2, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docInternalS1Type.subclass = docInternalS1TypeSub
# end class docInternalS1TypeSub


class docInternalS2TypeSub(supermod.docInternalS2Type):
    def __init__(self, para=None, sect3=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docInternalS2TypeSub, self).__init__(para, sect3, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docInternalS2Type.subclass = docInternalS2TypeSub
# end class docInternalS2TypeSub


class docInternalS3TypeSub(supermod.docInternalS3Type):
    def __init__(self, para=None, sect3=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docInternalS3TypeSub, self).__init__(para, sect3, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docInternalS3Type.subclass = docInternalS3TypeSub
# end class docInternalS3TypeSub


class docInternalS4TypeSub(supermod.docInternalS4Type):
    def __init__(self, para=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docInternalS4TypeSub, self).__init__(para, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docInternalS4Type.subclass = docInternalS4TypeSub
# end class docInternalS4TypeSub


class docTitleTypeSub(supermod.docTitleType):
    def __init__(self, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docTitleTypeSub, self).__init__(ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docTitleType.subclass = docTitleTypeSub
# end class docTitleTypeSub


class docParaTypeSub(supermod.docParaType):
    def __init__(self, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, nonbreakablespace=None, linebreak=None, hruler=None, preformatted=None, programlisting=None, verbatim=None, indexentry=None, orderedlist=None, itemizedlist=None, simplesect=None, title=None, variablelist=None, table=None, heading=None, image=None, dotfile=None, mscfile=None, diafile=None, toclist=None, language=None, parameterlist=None, xrefsect=None, copydoc=None, blockquote=None, parblock=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docParaTypeSub, self).__init__(ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, dot, msc, plantuml, anchor, copy, emoji, nonbreakablespace, linebreak, hruler, preformatted, programlisting, verbatim, indexentry, orderedlist, itemizedlist, simplesect, title, variablelist, table, heading, image, dotfile, mscfile, diafile, toclist, language, parameterlist, xrefsect, copydoc, blockquote, parblock, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docParaType.subclass = docParaTypeSub
# end class docParaTypeSub


class docMarkupTypeSub(supermod.docMarkupType):
    def __init__(self, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, nonbreakablespace=None, linebreak=None, hruler=None, preformatted=None, programlisting=None, verbatim=None, indexentry=None, orderedlist=None, itemizedlist=None, simplesect=None, title=None, variablelist=None, table=None, heading=None, image=None, dotfile=None, mscfile=None, diafile=None, toclist=None, language=None, parameterlist=None, xrefsect=None, copydoc=None, blockquote=None, parblock=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docMarkupTypeSub, self).__init__(ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, dot, msc, plantuml, anchor, copy, emoji, nonbreakablespace, linebreak, hruler, preformatted, programlisting, verbatim, indexentry, orderedlist, itemizedlist, simplesect, title, variablelist, table, heading, image, dotfile, mscfile, diafile, toclist, language, parameterlist, xrefsect, copydoc, blockquote, parblock, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docMarkupType.subclass = docMarkupTypeSub
# end class docMarkupTypeSub


class docURLLinkSub(supermod.docURLLink):
    def __init__(self, url=None, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docURLLinkSub, self).__init__(url, ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docURLLink.subclass = docURLLinkSub
# end class docURLLinkSub


class docAnchorTypeSub(supermod.docAnchorType):
    def __init__(self, id=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docAnchorTypeSub, self).__init__(id, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docAnchorType.subclass = docAnchorTypeSub
# end class docAnchorTypeSub


class docFormulaTypeSub(supermod.docFormulaType):
    def __init__(self, id=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docFormulaTypeSub, self).__init__(id, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docFormulaType.subclass = docFormulaTypeSub
# end class docFormulaTypeSub


class docIndexEntryTypeSub(supermod.docIndexEntryType):
    def __init__(self, primaryie=None, secondaryie=None, **kwargs_):
        super(docIndexEntryTypeSub, self).__init__(primaryie, secondaryie,  **kwargs_)
supermod.docIndexEntryType.subclass = docIndexEntryTypeSub
# end class docIndexEntryTypeSub


class docListTypeSub(supermod.docListType):
    def __init__(self, listitem=None, **kwargs_):
        super(docListTypeSub, self).__init__(listitem,  **kwargs_)
supermod.docListType.subclass = docListTypeSub
# end class docListTypeSub


class docListItemTypeSub(supermod.docListItemType):
    def __init__(self, para=None, **kwargs_):
        super(docListItemTypeSub, self).__init__(para,  **kwargs_)
supermod.docListItemType.subclass = docListItemTypeSub
# end class docListItemTypeSub


class docSimpleSectTypeSub(supermod.docSimpleSectType):
    def __init__(self, kind=None, title=None, para=None, **kwargs_):
        super(docSimpleSectTypeSub, self).__init__(kind, title, para,  **kwargs_)
supermod.docSimpleSectType.subclass = docSimpleSectTypeSub
# end class docSimpleSectTypeSub


class docVarListEntryTypeSub(supermod.docVarListEntryType):
    def __init__(self, term=None, **kwargs_):
        super(docVarListEntryTypeSub, self).__init__(term,  **kwargs_)
supermod.docVarListEntryType.subclass = docVarListEntryTypeSub
# end class docVarListEntryTypeSub


class docVariableListTypeSub(supermod.docVariableListType):
    def __init__(self, varlistentry=None, listitem=None, **kwargs_):
        super(docVariableListTypeSub, self).__init__(varlistentry, listitem,  **kwargs_)
supermod.docVariableListType.subclass = docVariableListTypeSub
# end class docVariableListTypeSub


class docRefTextTypeSub(supermod.docRefTextType):
    def __init__(self, refid=None, kindref=None, external=None, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docRefTextTypeSub, self).__init__(refid, kindref, external, ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docRefTextType.subclass = docRefTextTypeSub
# end class docRefTextTypeSub


class docTableTypeSub(supermod.docTableType):
    def __init__(self, rows=None, cols=None, caption=None, row=None, **kwargs_):
        super(docTableTypeSub, self).__init__(rows, cols, caption, row,  **kwargs_)
supermod.docTableType.subclass = docTableTypeSub
# end class docTableTypeSub


class docRowTypeSub(supermod.docRowType):
    def __init__(self, entry=None, **kwargs_):
        super(docRowTypeSub, self).__init__(entry,  **kwargs_)
supermod.docRowType.subclass = docRowTypeSub
# end class docRowTypeSub


class docEntryTypeSub(supermod.docEntryType):
    def __init__(self, thead=None, para=None, **kwargs_):
        super(docEntryTypeSub, self).__init__(thead, para,  **kwargs_)
supermod.docEntryType.subclass = docEntryTypeSub
# end class docEntryTypeSub


class docCaptionTypeSub(supermod.docCaptionType):
    def __init__(self, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docCaptionTypeSub, self).__init__(ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docCaptionType.subclass = docCaptionTypeSub
# end class docCaptionTypeSub


class docHeadingTypeSub(supermod.docHeadingType):
    def __init__(self, level=None, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docHeadingTypeSub, self).__init__(level, ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docHeadingType.subclass = docHeadingTypeSub
# end class docHeadingTypeSub


class docImageTypeSub(supermod.docImageType):
    def __init__(self, type_=None, name=None, width=None, height=None, inline=None, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docImageTypeSub, self).__init__(type_, name, width, height, inline, ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docImageType.subclass = docImageTypeSub
# end class docImageTypeSub


class docFileTypeSub(supermod.docFileType):
    def __init__(self, name=None, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docFileTypeSub, self).__init__(name, ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docFileType.subclass = docFileTypeSub
# end class docFileTypeSub


class docTocItemTypeSub(supermod.docTocItemType):
    def __init__(self, id=None, ulink=None, ref=None, formula=None, latexonly=None, htmlonly=None, umlaut=None, acute=None, circ=None, tilde=None, cedil=None, bold=None, strike=None, underline=None, emphasis=None, computeroutput=None, subscript=None, superscript=None, center=None, small=None, szlig=None, manonly=None, xmlonly=None, rtfonly=None, image=None, dot=None, msc=None, plantuml=None, anchor=None, copy=None, emoji=None, linebreak=None, nonbreakablespace=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docTocItemTypeSub, self).__init__(id, ulink, ref, formula, latexonly, htmlonly, umlaut, acute, circ, tilde, cedil, bold, strike, underline, emphasis, computeroutput, subscript, superscript, center, small, szlig, manonly, xmlonly, rtfonly, image, dot, msc, plantuml, anchor, copy, emoji, linebreak, nonbreakablespace, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docTocItemType.subclass = docTocItemTypeSub
# end class docTocItemTypeSub


class docTocListTypeSub(supermod.docTocListType):
    def __init__(self, tocitem=None, **kwargs_):
        super(docTocListTypeSub, self).__init__(tocitem,  **kwargs_)
supermod.docTocListType.subclass = docTocListTypeSub
# end class docTocListTypeSub


class docLanguageTypeSub(supermod.docLanguageType):
    def __init__(self, langid=None, para=None, **kwargs_):
        super(docLanguageTypeSub, self).__init__(langid, para,  **kwargs_)
supermod.docLanguageType.subclass = docLanguageTypeSub
# end class docLanguageTypeSub


class docParamListTypeSub(supermod.docParamListType):
    def __init__(self, kind=None, parameteritem=None, **kwargs_):
        super(docParamListTypeSub, self).__init__(kind, parameteritem,  **kwargs_)
supermod.docParamListType.subclass = docParamListTypeSub
# end class docParamListTypeSub


class docParamListItemSub(supermod.docParamListItem):
    def __init__(self, parameternamelist=None, parameterdescription=None, **kwargs_):
        super(docParamListItemSub, self).__init__(parameternamelist, parameterdescription,  **kwargs_)
supermod.docParamListItem.subclass = docParamListItemSub
# end class docParamListItemSub


class docParamNameListSub(supermod.docParamNameList):
    def __init__(self, parametertype=None, parametername=None, **kwargs_):
        super(docParamNameListSub, self).__init__(parametertype, parametername,  **kwargs_)
supermod.docParamNameList.subclass = docParamNameListSub
# end class docParamNameListSub


class docParamTypeSub(supermod.docParamType):
    def __init__(self, ref=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docParamTypeSub, self).__init__(ref, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docParamType.subclass = docParamTypeSub
# end class docParamTypeSub


class docParamNameSub(supermod.docParamName):
    def __init__(self, direction=None, ref=None, valueOf_=None, mixedclass_=None, content_=None, **kwargs_):
        super(docParamNameSub, self).__init__(direction, ref, valueOf_, mixedclass_, content_,  **kwargs_)
supermod.docParamName.subclass = docParamNameSub
# end class docParamNameSub


class docXRefSectTypeSub(supermod.docXRefSectType):
    def __init__(self, id=None, xreftitle=None, xrefdescription=None, **kwargs_):
        super(docXRefSectTypeSub, self).__init__(id, xreftitle, xrefdescription,  **kwargs_)
supermod.docXRefSectType.subclass = docXRefSectTypeSub
# end class docXRefSectTypeSub


class docCopyTypeSub(supermod.docCopyType):
    def __init__(self, link=None, para=None, sect1=None, internal=None, **kwargs_):
        super(docCopyTypeSub, self).__init__(link, para, sect1, internal,  **kwargs_)
supermod.docCopyType.subclass = docCopyTypeSub
# end class docCopyTypeSub


class docBlockQuoteTypeSub(supermod.docBlockQuoteType):
    def __init__(self, para=None, **kwargs_):
        super(docBlockQuoteTypeSub, self).__init__(para,  **kwargs_)
supermod.docBlockQuoteType.subclass = docBlockQuoteTypeSub
# end class docBlockQuoteTypeSub


class docParBlockTypeSub(supermod.docParBlockType):
    def __init__(self, para=None, **kwargs_):
        super(docParBlockTypeSub, self).__init__(para,  **kwargs_)
supermod.docParBlockType.subclass = docParBlockTypeSub
# end class docParBlockTypeSub


class docEmptyTypeSub(supermod.docEmptyType):
    def __init__(self, **kwargs_):
        super(docEmptyTypeSub, self).__init__( **kwargs_)
supermod.docEmptyType.subclass = docEmptyTypeSub
# end class docEmptyTypeSub


class tableofcontentsTypeSub(supermod.tableofcontentsType):
    def __init__(self, tocsect=None, **kwargs_):
        super(tableofcontentsTypeSub, self).__init__(tocsect,  **kwargs_)
supermod.tableofcontentsType.subclass = tableofcontentsTypeSub
# end class tableofcontentsTypeSub


class tableofcontentsKindTypeSub(supermod.tableofcontentsKindType):
    def __init__(self, name=None, reference=None, tableofcontents=None, **kwargs_):
        super(tableofcontentsKindTypeSub, self).__init__(name, reference, tableofcontents,  **kwargs_)
supermod.tableofcontentsKindType.subclass = tableofcontentsKindTypeSub
# end class tableofcontentsKindTypeSub


class docEmojiTypeSub(supermod.docEmojiType):
    def __init__(self, name=None, unicode=None, **kwargs_):
        super(docEmojiTypeSub, self).__init__(name, unicode,  **kwargs_)
supermod.docEmojiType.subclass = docEmojiTypeSub
# end class docEmojiTypeSub


def get_root_tag(node):
    tag = supermod.Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = None
    rootClass = supermod.GDSClassesMapping.get(tag)
    if rootClass is None and hasattr(supermod, tag):
        rootClass = getattr(supermod, tag)
    return tag, rootClass


def parse(inFilename, silence=False):
    parser = None
    doc = parsexml_(inFilename, parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'DoxygenType'
        rootClass = supermod.DoxygenType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    if not silence:
        sys.stdout.write('<?xml version="1.0" ?>\n')
        rootObj.export(
            sys.stdout, 0, name_=rootTag,
            namespacedef_='',
            pretty_print=True)
    return rootObj


def parseEtree(inFilename, silence=False):
    parser = None
    doc = parsexml_(inFilename, parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'DoxygenType'
        rootClass = supermod.DoxygenType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    mapping = {}
    rootElement = rootObj.to_etree(None, name_=rootTag, mapping_=mapping)
    reverse_mapping = rootObj.gds_reverse_node_mapping(mapping)
    if not silence:
        content = etree_.tostring(
            rootElement, pretty_print=True,
            xml_declaration=True, encoding="utf-8")
        sys.stdout.write(content)
        sys.stdout.write('\n')
    return rootObj, rootElement, mapping, reverse_mapping


def parseString(inString, silence=False):
    if sys.version_info.major == 2:
        from StringIO import StringIO
    else:
        from io import BytesIO as StringIO
    parser = None
    doc = parsexml_(StringIO(inString), parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'DoxygenType'
        rootClass = supermod.DoxygenType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    if not silence:
        sys.stdout.write('<?xml version="1.0" ?>\n')
        rootObj.export(
            sys.stdout, 0, name_=rootTag,
            namespacedef_='')
    return rootObj


def parseLiteral(inFilename, silence=False):
    parser = None
    doc = parsexml_(inFilename, parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'DoxygenType'
        rootClass = supermod.DoxygenType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    if not silence:
        sys.stdout.write('#from ??? import *\n\n')
        sys.stdout.write('import ??? as model_\n\n')
        sys.stdout.write('rootObj = model_.rootClass(\n')
        rootObj.exportLiteral(sys.stdout, 0, name_=rootTag)
        sys.stdout.write(')\n')
    return rootObj


USAGE_TEXT = """
Usage: python ???.py <infilename>
"""


def usage():
    print(USAGE_TEXT)
    sys.exit(1)


def main():
    args = sys.argv[1:]
    if len(args) != 1:
        usage()
    infilename = args[0]
    parse(infilename)


if __name__ == '__main__':
    #import pdb; pdb.set_trace()
    main()
