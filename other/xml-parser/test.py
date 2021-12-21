#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright Sangfor. All rights reserved

"""
doxygenxml2md module
~~~~~~~~~~~~~~~~~~~~

Parses doxygen XML tree looking for Common modules,  convert xml to markdown,
generate API documents.

usage:

   python3 test.py [option] <doxygen_xml_dir>

The other methods are supported - see `README.md`.

:copyright: (c) 2019 by Sangfor Kangxing.
"""


import sys
import os
import argparse
from parser_doxyxml import DoxygenParserFactory
import markdown as md
import gen_type

__version__ = 'V1.0.0'


class GroupDict:
    """ 文档组字典类，用来保存文档层次结构。

    根据doxygen生成xml中的compoundname和innergroup相关字段建立文档层次关系。
    """

    def __init__(self):
        self.group_dict = {}

    def _find_group_node(self, group_dict, compound_refid):
        """ 从组字典group_dict中寻找key为compound_refid的子节点 """
        group_node = group_dict.get(compound_refid, None)
        if group_node is not None:
            return group_node
        else:
            if group_dict:
                for chirld_group_dict_key in group_dict:
                    group_node = self._find_group_node(group_dict[chirld_group_dict_key], compound_refid)
                    if group_node is not None:
                        return group_node
        return group_node

    def insert(self, parent_compound_refid, chirld_compound_refid):
        """ 向组节点中插入子节点

        向父节点parent_compound_refid中插入子节点chirld_compound_refid，若没有父节点parent_compound_refid，
        则向根节点插入chirld_compound_refid
        """
        chirld_compound_dict = self.group_dict.get(chirld_compound_refid, None)
        if parent_compound_refid is None:
            # print(chirld_compound_refid)
            chirld_compound_dict = self._find_group_node(self.group_dict, chirld_compound_refid)
            if chirld_compound_dict is not None:
                return
            else:
                self.group_dict[chirld_compound_refid] = {}
                return

        parent_node = self._find_group_node(self.group_dict, parent_compound_refid)
        if not parent_node:
            self.group_dict[parent_compound_refid] = {}
            parent_node = self.group_dict[parent_compound_refid]

        if chirld_compound_dict is not None:
            self.group_dict.pop(chirld_compound_refid)

        parent_node[chirld_compound_refid] = {}

    def print_(self, group_dict=None, level=1):
        """ 打印组字典 """
        if group_dict is None:
            print("---------------  文档结构 -----------------")
            group_dict = self.group_dict
        
        for group_refid in group_dict:
            print(f'{"  " * level}{group_refid}')
            self.print_(group_dict[group_refid], level+1)


def creat_dir(path):
    """ 创建目录 """

    if os.path.exists(path) and os.path.isdir(path):
        return
    os.mkdir(path)
    print("creat_dir: %s" % path)


def creat_file(path):
    """ 创建文件 """

    if os.path.exists(path) and not os.path.isdir(path):
        return
    
    if os.name == "nt":
        f = open(path, "w")
        f.close
    else:
        os.mknod(path)
    
    print("creat_file: %s" % path)


def parser_program(program):
    """解析programlisting节点中的代码. 
    
    :param program: xml文件的programlisting节点对象(class listingType).  
    """
    content = ""
    codeline = program.codeline
    if not codeline:
        return content

    for code in codeline:
        one_line_code = ""
        if code.highlight:
            for highlight in code.highlight:
                # class MixedContainer
                for mixed_container in highlight.content_:
                    if mixed_container.name == "sp":
                        one_line_code = f'{one_line_code} '
                    elif mixed_container.name == "":
                        one_line_code = f'{one_line_code}{mixed_container.value}'         
        content = f'{content}{one_line_code}\n'
    return content





# def parser_detaileddescription(detaileddescription):
def parser_description(description):
    """解析detaileddescription或briefdescription节点中的相关内容. 
    
    :param description: xml文件的detaileddescription或briefdescription节点对象(class descr
    iptionType).    
    """
    content = ""
    if description and description.para:
        for para in description.para:
            para_content = ""
            
            # 获取描述文本
            if para.valueOf_:
                para_content = f'{para_content}{para.valueOf_}  \n'
            
            # 获取程序示例代码(class listingType)
            if para.programlisting:
                program_content = ""
                for program in para.programlisting:
                    program_content = f'{program_content}{parser_program(program)}'
                para_content = f'{para_content}  \n```cpp\n{program_content}\n```\n'
            
            # 获取参数(class docParamListType)
            if para.parameterlist:
                param_content = ""
                param_content += "#### 参数\n\n"
                param_content += "参数|说明\n"
                param_content += "--------------------------------------|--------------------------------------|\n"
                
                for parameter in para.parameterlist:
                    parameteritem_list = parameter.parameteritem
                    for parameteritem in parameteritem_list:
                        param_content += "`%s`|%s|\n" % (
                            parameteritem.parameternamelist[0].parametername[0].valueOf_,
                            parser_sim_description(parameteritem.parameterdescription))
                para_content += param_content
            content = f'{content}{para_content}'
    return content


def parser_sim_description(description):
    """ 解析xml的文本注释. 
    
    :param description: xml文件的briefdescription和detaileddescription节点对象(class descriptionType).    
    """
    content = ""
    if description and description.para:
        for para in description.para:
            if hasattr(para, "valueOf_") and para.valueOf_:
                content = f'{content}{para.valueOf_} '
    content = f'{content}'
    return content


def parser_description_all(compounddef):
    """ 解析compounddef节点中的注释： 
    即briefdescription和detaileddescription节点(class descriptionType). 
    
    :param compounddef: xml文件的compounddef节点对象(class compounddefType).    
    """
    content = ""
    briefdescription = parser_description(compounddef.briefdescription)
    if briefdescription:
        content = f'{content}{briefdescription}  \n\n'
    
    detaileddescription = parser_description(compounddef.detaileddescription)
    if detaileddescription:
        content = f'{content}{detaileddescription}  \n'
    
    return content


def generate_index_md(compounddef, index_path):
    """ 生成一个组的index文件.

    :param compounddef: index文件的compounddef节点(class compounddefType).
    :param index_path: 将要生成的index.md的路径.
    """
    index_fd = open(index_path, "a")
    content = ""

    # 组名
    content = content + "# %s\n" % compounddef.title
    # content +=  md.title(compounddef.title, 1)

    # 组的描述
    content = content + "## 描述\n"
    if len(compounddef.detaileddescription.para) != 0:
        group_detaileddescription_str = compounddef.detaileddescription.para[0].valueOf_
        content = content + "  %s\n\n" % group_detaileddescription_str

    if len(compounddef.briefdescription.para) != 0:
        group_briefdescription_str = compounddef.briefdescription.para[0].valueOf_
        content = content + "  %s\n\n" % group_briefdescription_str
    
    # 组内成员
    content = content + "## 模块\n"
    innergroup_list = compounddef.innergroup
    content = content + "Moudle|\n--------------------------------------|\n"
    for innergroup in innergroup_list:   # refType
        content = content + "`%s` |\n" % str(innergroup.valueOf_)
        pass
    
    content = content + "\n\n"
    index_fd.write(content)
    index_fd.close()


def generate_struct(compounddef):

    content = "## `struct %s`\n" % compounddef.compoundname
    content = content + "成员|描述|\n"
    content = content + "--------------------------------------|--------------------------------------|\n"
    
    memberdef_list = compounddef.sectiondef[0].memberdef
    for memberdef in memberdef_list:
        member_definition = "%s %s" % (memberdef.type_.valueOf_, memberdef.name)
        if hasattr(memberdef, "argsstring"):
            member_definition = "%s%s" % (member_definition, memberdef.argsstring)

        content = content + "`%s`|%s|\n" % (member_definition, parser_sim_description(memberdef.detaileddescription))
    
    return content


def generate_union(compounddef):
    
    content = "### `union %s`\n" % compounddef.compoundname
    content = content + "成员|描述|\n"
    content = content + "--------------------------------------|--------------------------------------|\n"
    
    memberdef_list = compounddef.sectiondef[0].memberdef
    for memberdef in memberdef_list:
        member_definition = "%s %s" % (memberdef.type_.valueOf_, memberdef.name)
        if hasattr(memberdef, "argsstring"):
            member_definition = "%s%s" % (member_definition, memberdef.argsstring)
        content = content + "`%s`|%s|\n" % (member_definition, parser_description(memberdef.detaileddescription))

    return content


def parser_member_description(memberdef):    
    return parser_description_all(memberdef)    

def parser_class_func(memberdef_list):
    content = ""
    for memberdef in memberdef_list:
        definition = ""
        definition += "%s " % (memberdef.prot)
        if memberdef.static == "yes":
            definition += "static " 
        
        if memberdef.const == "yes":
            definition += "const " 

        if memberdef.explicit == "yes":
            definition += "explicit " 

        if memberdef.inline == "yes":
            definition += "inline " 
        
        if memberdef.virt == "virtual":
            definition += "virtual "
        
        definition += "%s %s %s" % (memberdef.type_.valueOf_, memberdef.name, memberdef.argsstring)
        description = parser_member_description(memberdef)

        content = content + "`%s`|%s|\n" % (definition, description)
    
    return content


def parser_class_attrib(memberdef_list):

    content = ""
    for memberdef in memberdef_list:
        definition = ""
        definition += "%s " % (memberdef.prot)
        if memberdef.static == "yes":
            definition += "static "
        
        definition += "%s %s %s" % (memberdef.type_.valueOf_, memberdef.name, memberdef.argsstring)
        description = parser_member_description(memberdef)

        content = content + "`%s`|%s|\n" % (definition, description)
        pass

    return content

def parser_global_var(memberdef_list):

    content = ""
    for memberdef in memberdef_list:
        definition = ""
        if memberdef.static == "yes":
            definition += "static "
        
        definition += "%s %s %s" % (memberdef.type_.valueOf_, memberdef.name, memberdef.argsstring)
        description = parser_member_description(memberdef)

        content = content + "`%s`|%s|\n" % (definition, description)
        pass

    return content


def generate_class(compounddef):
    
    content = "### `class %s`\n" % compounddef.compoundname
    func_content = ""
    attrib_content = ""
    
    for sectiondef in compounddef.sectiondef:
        if sectiondef.kind == "public-type":
            
            pass
    
        elif sectiondef.kind == "public-func":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
        
        elif sectiondef.kind == "public-attrib":
            class_attrib_content = parser_class_attrib(sectiondef.memberdef)
            attrib_content += class_attrib_content
            pass

        elif sectiondef.kind == "public-static-func":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
            pass

        elif sectiondef.kind == "public-static-attrib":
            class_attrib_content = parser_class_attrib(sectiondef.memberdef)
            attrib_content += class_attrib_content
            pass

        elif sectiondef.kind == "protected-type":

            pass

        elif sectiondef.kind == "protected-func":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
            pass

        elif sectiondef.kind == "protected-attrib":
            class_attrib_content = parser_class_attrib(sectiondef.memberdef)
            attrib_content += class_attrib_content
            pass

        elif sectiondef.kind == "protected-static-func":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
            pass

        elif sectiondef.kind == "protected-static-attrib":
            class_attrib_content = parser_class_attrib(sectiondef.memberdef)
            attrib_content += class_attrib_content
            pass

        elif sectiondef.kind == "private-type":

            pass

        elif sectiondef.kind == "private-func":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
            pass

        elif sectiondef.kind == "private-attrib":
            class_attrib_content = parser_class_attrib(sectiondef.memberdef)
            attrib_content += class_attrib_content
            pass

        elif sectiondef.kind == "private-static-func":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
            pass

        elif sectiondef.kind == "private-static-attrib":
            class_attrib_content = parser_class_attrib(sectiondef.memberdef)
            attrib_content += class_attrib_content
            pass
        
        elif sectiondef.kind == "friend":
            class_func_content = parser_class_func(sectiondef.memberdef)
            func_content += class_func_content
            pass

        elif sectiondef.kind == "public-slot":

            pass

        elif sectiondef.kind == "protected-slot":

            pass

        elif sectiondef.kind == "private-slot":

            pass

        elif sectiondef.kind == "signal":

            pass
                                              
        elif sectiondef.kind == "user-defined":

            pass

        else:
            pass
    
    if func_content != "":
        content += "\n### 成员函数\n"
        content += "声明|描述|\n"
        content = content + "--------------------------------------|--------------------------------------|\n"
        content += func_content
    
    if attrib_content != "":
        content += "\n### 成员变量\n"
        content = content + "声明|描述|\n"
        content = content + "--------------------------------------|--------------------------------------|\n"
        content += attrib_content

    return content


def parser_innerclass(innerclass_list, compound_parser):
    content = ""
    struct_md_content_str = ""
    class_md_content_str = ""
    union_md_content_str = ""
    
    for innerclass in innerclass_list:
        innerclass_compounddef = compound_parser.parse(innerclass.refid).compounddef[0]
        if innerclass_compounddef.kind == "struct":
            struct_md_content = generate_struct(innerclass_compounddef)
            struct_md_content_str = struct_md_content_str + struct_md_content
        
        elif innerclass_compounddef.kind == "class":
            class_md_content = generate_class(innerclass_compounddef)
            class_md_content_str = class_md_content_str + class_md_content
        
        elif innerclass_compounddef.kind == "union":
            union_md_content = generate_union(innerclass_compounddef)
            union_md_content_str = union_md_content_str + union_md_content

        elif innerclass_compounddef.kind == "interface":

            pass

        elif innerclass_compounddef.kind == "category":

            pass

        elif innerclass_compounddef.kind == "exception":

            pass

        elif innerclass_compounddef.kind == "namespace":

            pass

        else:
            pass

    if union_md_content_str != "":
        content = content + "## 联合体\n\n" + union_md_content_str

    if struct_md_content_str != "":
        content = content + "## 结构体\n\n" + struct_md_content_str
    
    if class_md_content_str != "":
        content = content + "## 类\n\n" + class_md_content_str

    return content


def parser_define(memberdef_list):
    content = ""
    for memberdef in memberdef_list:
        initializer = ""
        if hasattr(memberdef, initializer):
            initializer = str(memberdef.initializer.valueOf_).replace("\n", "").replace("\r\\n", "").replace("\\", "")
        description = parser_member_description(memberdef)
        content = content + "`%s`|%s|%s|\n" % (memberdef.name, initializer, description)
        pass

    return content


def parser_section_enum(sectiondef):

    content = ""

    for memberdef in sectiondef.memberdef:
        enum_content = ""
        enum_content += "### %s\n\n" %  (memberdef.name)
        enum_content += "枚举名|初始值|含义\n"
        enum_content += "-------------------|-----------------|--------------------------------------|\n"
        
        for enumvalue in memberdef.enumvalue:
            if hasattr(enumvalue, "initializer") and enumvalue.initializer != None:
                enum_content += "`%s`|%s|%s\n" % (enumvalue.name, enumvalue.initializer.valueOf_, 
                    parser_description(enumvalue.detaileddescription))
            else:
                enum_content += "`%s`|%s|%s\n" % (enumvalue.name, "", parser_description(enumvalue.detaileddescription))

        content += enum_content

    return content


def parser_section_func(sectiondef, func_type):

    content = ""

    for member in sectiondef.memberdef:
        func_type.name = member.name
        func_type.definition = member.definition
        func_type.argsstring = member.argsstring
        func_type.briefdescription = parser_sim_description(member.briefdescription)
        func_type.detaileddescription = parser_sim_description(member.detaileddescription)
        

        func_md_content = ""
        func_md_content += "### `%s`\n" % (member.name)
        func_type.name = member.name
        func_md_content += "#### 原型\n\n"
        func_md_content += "\t%s%s\n\n" % (member.definition, member.argsstring)
        
        func_md_content += "#### 描述\n\n"
        func_md_content += "\t%s\n" % (parser_sim_description(member.detaileddescription))
        
        func_md_content += "#### 参数\n\n"
        func_md_content += "参数|说明\n"
        func_md_content += "--------------------------------------|--------------------------------------|\n"
        
        if (hasattr(member, "detaileddescription") and member.detaileddescription.para and 
            member.detaileddescription.para[0].parameterlist):
            for parameteritem in member.detaileddescription.para[0].parameterlist[0].parameteritem:
                func_md_content += "`%s`|%s\n" % (
                    parameteritem.parameternamelist[0].parametername[0].valueOf_,
                    parser_sim_description(parameteritem.parameterdescription))
                
                param_name = parameteritem.parameternamelist[0].parametername[0].valueOf_
                param_desc = parser_sim_description(parameteritem.parameterdescription)
                param_type = gen_type.FuncParamType("", param_name, param_desc)
                func_type.param_list.append(param_type)
        
        func_md_content += "\n"
        func_md_content += "#### 返回\n\n"
        if (member.detaileddescription.para and member.detaileddescription.para[0].simplesect):
            func_md_content += "\t%s\n\n" % member.detaileddescription.para[0].simplesect[0].para[0].valueOf_
        
        content += func_md_content

    return content


def parser_section_var(sectiondef):
    content = ""
    content += "变量名称|说明\n"
    content += "--------------------------------------|--------------------------------------|\n"
    content += parser_global_var(sectiondef.memberdef)
    return content


def parser_section_define(sectiondef):

    content = ""
    content += "宏定义名称|值|说明\n"
    content += "------------------|-------------------|--------------------------------------|\n"
    content += parser_define(sectiondef.memberdef)
    return content


def parser_sectiondef(sectiondef_list, compound_parser):
    content = ""
    enum_md_content_str = ""
    func_md_content_str = ""
    var_md_content_str = ""
    define_md_content_str = ""
    func_type = gen_type.FuncType()



    for sectiondef in sectiondef_list:
        if sectiondef.kind == "enum":
            enum_md_content = parser_section_enum(sectiondef)
            enum_md_content_str += enum_md_content
            pass

        elif sectiondef.kind == "func":
            func_md_content = parser_section_func(sectiondef, func_type)
            func_md_content_str += func_md_content
            pass

        elif sectiondef.kind == "var":
            var_md_content = parser_section_var(sectiondef)
            var_md_content_str += var_md_content
            pass
        
        elif sectiondef.kind == "define":
            define_md_content = parser_section_define(sectiondef)
            define_md_content_str += define_md_content
            pass

        else:
            pass
    
    if func_md_content_str != "":
        content = content + "## 函数接口\n\n" + func_md_content_str + "\n"

    if enum_md_content_str != "":
        content = content + "## 枚举\n\n" + enum_md_content_str + "\n"

    if var_md_content_str != "":
        content += "## 全局变量\n\n" + var_md_content_str + "\n"

    if define_md_content_str != "":
        content += "## 宏定义\n\n" + define_md_content_str + "\n"

    return content


# def parser_detaileddescription(detaileddescription):
#     content = ""

#     for para in detaileddescription.para:
#         para_content = ""
#         if hasattr(para, "valueOf_") and para.valueOf_:
#             para_content += "## %s\n\n" % (para.valueOf_)
        
#         if hasattr(para, "programlisting") and len(para.programlisting) != 0:
#             codeline = para.programlisting[0].codeline
#             para_content += "\n```cpp\n"
#             for code in codeline:
#                 one_line_code = ""

#                 # test_fd = open("test.txt", "a")
#                 # code.export(test_fd, 1)
#                 # test_fd.close()
#                 if hasattr(code, "highlight") and len(code.highlight) != 0:
#                     highlight = code.highlight[0]

#                     if highlight.valueOf_:
#                         one_line_code += "%s" % (highlight.valueOf_)

#                     # if hasattr(highlight, "sp") and len(highlight.sp) != 0:
#                     #     for sp in highlight.sp:
#                     #         # print(sp.valueOf_)
#                     #         one_line_code += ' '
#                     #         # print(sp.value)
#                     #         # print(sp.valueOf_)
#                     #         if sp.valueOf_:
#                     #             one_line_code += "%s" % (sp.valueOf_)

#                 one_line_code += "\n"
#                 # print(one_line_code)
#                 para_content += one_line_code
#             para_content += "```\n" 
#         content += para_content
#     return content


def generate_md(compounddef, path, compound_parser):
    """ 生成markdown. 
    
    :param compounddef: xml文件的compounddef节点对象(class compounddefType).   
    :param path: 目标md文件路径.  
    :param compound_parser: compound节点解析器.  
    """
    print(f'generate_md:  {path}')
    
    content = ""
    # defgroup desc 组名
    content = f'{content}{md.title(compounddef.title, 1)}\n'
    
    # 组描述
    content = f'{content}{md.title("简介", 2)}'\
                f'{parser_description_all(compounddef)}'
    
    # sectiondef 函数、枚举等
    if hasattr(compounddef, "sectiondef"):
        sectiondef_content = parser_sectiondef(compounddef.sectiondef, compound_parser)
        content = content + sectiondef_content

    # innerclass 类、结构体、联合等
    if hasattr(compounddef, "innerclass"):
        innerclass_content = parser_innerclass(compounddef.innerclass, compound_parser)
        content = content + innerclass_content

    mdfile_fd = open(path, "a")
    mdfile_fd.write(content)
    mdfile_fd.close()
    pass


def generate_doc(args, group_dict, compound_parser, parent_path):
    """生成文档. 
    
    :param args: 命令行参数. 
    :param group_dict: 保存文档结构的字典.   
    :param compound_parser: compound节点解析器.  
    :param parent_path: 父节点文档保存路径.  
    """
    # 遍历字典获取所有组
    for chirld_group_dict_key in group_dict:  
        chirld_group_dict = group_dict[chirld_group_dict_key]

        # 获取组文件的compounddef节点对象(class compounddefType)
        group_compounddef = compound_parser.parse(chirld_group_dict_key).compounddef    
        path = os.path.join(parent_path, chirld_group_dict_key[7:])
        
        # 若没有二级子group，生成对应md文件
        if not chirld_group_dict:
            dest_file_path = f'{path}.md'
            creat_file(dest_file_path)

            # 获取文件compounddef节点(一个文件一个compounddef节点)
            file_compounddef = group_compounddef[0]     
            generate_md(file_compounddef, dest_file_path, compound_parser)

        # 有子group，创建目录并生成index.md，继续递归
        else:                       
            creat_dir(path)
            dest_index_path = os.path.join(path, "index.md")
            creat_file(dest_index_path)
            for compounddef in group_compounddef:
                generate_index_md(compounddef, dest_index_path)
                generate_doc(args, chirld_group_dict, compound_parser, path)


def recurse_group(group_dict, compound_parser, compound_refid, parent_compound_refid=None):
    """ 递归查找组及其组下节点并插入组字典中 """
    group_dict.insert(parent_compound_refid, compound_refid)
    compound_root = compound_parser.parse(compound_refid)

    if compound_root.compounddef:
        for compounddef in compound_root.compounddef:
            innergroup_list = compounddef.innergroup
            for innergroup in innergroup_list:
                recurse_group(group_dict, compound_parser, innergroup.refid, compound_refid)


def generate_group_dict(group_dict, compound_list, compound_parser):
    """ 生成文档组字典. 
    
    :param group_dict: 保存文档结构的字典.  
    :param compound_list: xml文件compound节点列表.  
    :param compound_parser: compound节点解析器.  
    """
    for compound in compound_list:
        if compound.kind == "group":
            recurse_group(group_dict, compound_parser, compound.refid)
            pass
        elif compound.kind == "struct":
            pass
        elif compound.kind == "dir":
            pass
        elif compound.kind == "union":
            pass
        elif compound.kind == "interface":
            pass
        elif compound.kind == "protocol":
            pass
        elif compound.kind == "category":
            pass
        elif compound.kind == "exception":
            pass
        elif compound.kind == "file":
            pass
        elif compound.kind == "namespace":
            pass
        elif compound.kind == "page":
            pass
        elif compound.kind == "example":
            pass        
        else:
            pass


def recurse_xml(args):
    """ 递归解析xml文件，生成markdown文档. """  
    doxygen_xml_dir = args.xmlpath
    doxygen_parser = DoxygenParserFactory(doxygen_xml_dir)
    
    # index 文件解析器
    index_parser = doxygen_parser.create_index_parser()   

    # 除index文件外的其他文件解析器      
    compound_parser = doxygen_parser.create_compound_parser()

    print("---------------------------start-----------------------------------")

    group_dict = GroupDict()
    doxyxml_root = index_parser.parse()
    compound_list = doxyxml_root.compound

    # 生成文档结构
    generate_group_dict(group_dict, compound_list, compound_parser)
    group_dict.print_()

    # 生成文档
    generate_doc(args, group_dict.group_dict, compound_parser, args.destdir)

    print("---------------------------end-----------------------------------")


def main():
    """Parse and check the command line arguments."""
    parser = argparse.ArgumentParser(
        description="""\
    Parse doxygen XML and generate markdown!
    Usage: python3 test.py [optional] <doxygen_xml_dir>""",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('-o', '--output-dir', action='store', dest='destdir',
                        help='Directory to place all output', required=True)
    parser.add_argument('-f', '--force', action='store_true', dest='force',
                        help='Overwrite existing files')
    parser.add_argument('-s', '--suffix', action='store', dest='suffix',
                        help='file suffix (default: md)', default='md')
    parser.add_argument('-q', '--quiet', action='store_true', dest='quiet',
                        help='suppress informational messages')
    parser.add_argument('--version', action='version',
                        version='xml2md %s' % __version__)
    parser.add_argument('xmlpath', type=str,
                        help='The directory contains index.xml')
    args = parser.parse_args()

    if args.suffix.startswith('.'):
        args.suffix = args.suffix[1:]

    if not os.path.isdir(args.xmlpath):
        print('%s is not a directory.' % args.xmlpath, file=sys.stderr)
        sys.exit(1)

    if 'index.xml' not in os.listdir(args.xmlpath):
        print('%s does not contain a index.xml' % args.xmlpath, file=sys.stderr)
        sys.exit(1)

    if not os.path.isdir(args.destdir):
        os.makedirs(args.destdir)
    
    args.xmlpath = os.path.abspath(args.xmlpath)
    print(args.destdir)

    # 递归xml文件
    recurse_xml(args)


if __name__ == "__main__":
    main()
