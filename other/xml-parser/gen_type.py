#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright Sangfor. All rights reserved

"""
doxygenxm2md type class
"""

class FuncParamType:

    def __init__(self, param_type="", param_name="", param_desc=""):
        if param_type:
            self.param_type = param_type
        else:
            self.param_type = ""
        
        if param_name:
            self.param_name = param_name
        else:
            self.param_name = ""
        
        if param_desc:
            self.param_desc = param_desc
        else:
            self.param_desc = ""


class FuncType:
    
    def __init__(self):
        self.name = ""
        self.definition = ""
        self.ret_type = ""
        self.ret_desc = ""
        self.argsstring = ""
        self.param_list = []
        self.briefdescription = ""
        self.detaileddescription = ""
        self.prot = "public"
        self.static = "no"
        self.const = "no"
        self.explicit = "no"
        self.inline = "no"
        self.virt = "non-virtual"
        self.location = None
        
    def export_md(self):


        for param in self.param_list:
            print(param.param_type)
        pass
    

