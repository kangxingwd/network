#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright Sangfor. All rights reserved

"""
Generate markdown module
~~~~~~~~~~~~~~~~~~~~~~~~

Generate markdown API, support most markdown grammar.
usage:

   >>> import markdown
   >>> content = markdown.link("Markdown官网" ,'https://markdown.cn')
   >>> print(content)
   [Markdown官网](https://markdown.cn)
   >>> content = markdown.title("markdown语法" , 1)
   >>> print(content)
   # markdown语法

The other markdown methods are supported - see `README.md`.
"""


def link(text, link, title = ""):
    """生成markdown链接.

    :param text: 需要转换的文本.  
    :param link: 链接地址或文件路径.  
    :param title: 链接显示的标题.  
    :returns: markdown格式链接文本.  
    :rtype: string  
    """
    return f'[{text}]({link} "{title}")'


def image(text, link):
    """生成markdown图片链接.  

    :param text: 替代文字.  
    :param link: 图片的链接或者文件路径.  
    :return: markdown格式的图片文本.  
    :rtype: string  
    """
    return f'![{text}]({link})'


def title(text, level = 1):
    """生成markdown标题.  

    :param text: 文本.  
    :param level: 标题等级 1-6.  
    :return: markdown标题文本.  
    :rtype: string   
    """
    if type(level) != int or level < 1 or level > 6:
        return text
    return f'{"#" * level} {text}\n'


def oblique(text, symbol="*"):
    """生成markdown斜体.  

    :param text: 文本.  
    :param symbol: markdown符号(默认是*)，可选的有：* 和 _.  
    :return: markdown斜体文本.  
    :rtype: string  
    """
    return f'{symbol}{text}{symbol}'


def bold(text, symbol="*"):
    """生成markdown加粗文本.  
    
    :param text: 文本.  
    :param symbol: markdown符号(默认是*)，可选的有：* 和 _.  
    :return: markdown加粗文本.  
    :rtype: string  
    """
    return f'{symbol * 2}{text}{symbol * 2}'

def oblique_bold(text, symbol="*"):
    """生成markdown斜体加粗文本.  

    :param text: 文本.  
    :param symbol: markdown符号(默认是*)，可选的有：* 和 _.  
    :return: markdown斜体加粗文本.  
    :rtype: string  
    """
    return f'{symbol * 3}{text}{symbol * 3}'


def email(text):
    """生成markdown Email文本.  

    :param text: 文本.  
    :return: markdown Email文本.  
    :rtype: string  
    """
    return f'<{text}>'


def highlight(text):
    """生成markdown高亮文本.  

    :param text: 文本.  
    :return: markdown高亮文本.  
    :rtype: string  
    """
    if str(text).find("`") != -1:
        return text
    return f'`{text}`'

def code(text, language=""):
    """生成markdown代码文本.  

    :param text: 文本.  
    :param language: 代码语言：cpp, html, css, javascript, php, python等.  
    :return: markdown代码文本.  
    :rtype: string  
    """
    return f'```{language}\n{text}\n```'

def block(text):
    """生成markdown代码区块文本.  

    :param text: 文本.  
    :return: markdown代码区块文本.  
    :rtype: string  
    """
    return f'> {text}\n'

def horizontal_rules(symbol="*"):
    """生成markdown分割线.  

    :param symbol: markdown符号(默认是*)，可选的有：* 和 _.  
    :return: markdown分割线文本.  
    :rtype: string  
    """
    return f'{symbol * 3}\n'

def trademarks():
    """生成版权标记.  

    :return: markdown版权标记.  
    :rtype: string  
    """

    return "&copy;"


def unordered_list(text_list, symbol="*"):
    """生成markdown无序列表.  

    :param text_list: 列表文本.  
    :param symbol: markdown列表符号(默认是*)，可选的有："*" 、"-" 和 "+" .  
    :return: markdown无序列表.  
    :rtype: string  
    """

    if len(text_list) == 0:
        return ""
    
    content = ""
    for text in text_list:
        content = f'{content}{symbol} {text}\n'
    
    return content


def ordered_list(text_list, num=1):
    """生成markdown有序列表.  

    :param text_list: 列表文本.  
    :param first: 序号(默认从1开始).  
    :return: markdown有序列表.  
    :rtype: string  
    """
    
    if (len(text_list) == 0 or type(num) != int or 
        (type(num) == int and num < 1)) :
        return ""
    
    content = ""
    for text in text_list:
        content = f'{content}{num} {text}\n'
        num += 1
    
    return content


def table(title_list, value_list):
    """生成markdown表格.  

    :param title_list: 标题列表.  
    :param value_list: 内容列表.   
    :return: markdown有序列表.  
    :rtype: string  
    """
    content = ""
    if len(title_list) == 0 and len(value_list) == 0:
        return content
    
    for title in title_list:
        content = f'{content}{title}|'
    
    content = f'{content}\n{"---------------------------------|" * len(title_list)}\n'
    
    for value in value_list:
        for val in value:
            content = f'{content}{val}|'
        content = f'{content}\n'
    return content


def table2(title_list, value_list, alignment = ""):
    """生成markdown表格.  

    :param title_list: 标题列表.  
    :param value_list: 内容列表.  
    :param alignment: 对其方式.  
    :return: markdown有序列表.  
    :rtype: string
    """


    return ""