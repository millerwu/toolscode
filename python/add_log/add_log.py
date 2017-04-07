#/usr/bin python3

import os
import sys
import re

def get_all_file(path, ext, list):
    for x in os.listdir(path):
        p = os.path.join(path, x)
        if os.path.isdir(p):
            get_all_file(p, ext, list)
        elif os.path.splitext(x)[1]==ext:
            list.append(p)

def add_log(file, log_tag_in, log_tag_out):
    fread = open(file, 'r')
    code_after = []
    code_lines = fread.readlines()
    has_parenthesis = False
    had_return = False
    fun_enter = False
    stack_deep = 0
    for code_line in code_lines:
        if re.search(r'[(].*[)]', code_line) and not re.search(r'[;]', code_line):
            fun_enter = True
            code_after.append(code_line)
            if re.search(r'[{]', code_line):
                stack_deep += 1
                code_after.append(log_tag_in)
        elif re.search(r'[{]', code_line) and fun_enter:
            stack_deep += 1
            code_after.append(code_line)
            code_after.append(log_tag_in)
        elif re.search(r'return', code_line):
            fun_enter = False
            stack_deep = 0
            code_after.append(log_tag_out)
            code_after.append(code_line)
        elif re.search(r'}', code_line) and fun_enter:
            stack_deep = stack_deep - 1
            if stack_deep == 0:
                fun_enter = False
            code_after.append(log_tag_out)
            code_after.append(code_line)
        else:
            code_after.append(code_line)
    fread.close()
    fwrite = open(file, 'w')
    fwrite.writelines(code_after)
    fwrite.close()

def delete_log(file, log_tag):
    fread = open(file, 'r')
    code_after = []
    code_lines = fread.readlines()
    for code_line in code_lines:
        if re.search(log_tag, code_line):
            continue
        code_after.append(code_line)
    fread.close()
    fwrite = open(file, 'w')
    fwrite.writelines(code_after)
    fwrite.close()
    
if __name__ == "__main__":
    if len(sys.argv) < 4:
        path = "f:\\WFD\\Hitachi\\code\\toolscode\\python\\add_log\\"
        ext = '.cpp'
        add = 1
    else:
        path = sys.argv[1]
        ext = sys.argv[2]
        add = sys.argv[3]
    list = []
    get_all_file(path, ext, list)
    tag_in = 'printf("<-------------");\n'
    tag_out = 'printf("------------->");\n'
    delete_tag = '-------------'

    if int(add) == 1:
        print("add log_tag")
        for x in list:
            add_log(x, tag_in, tag_out)
    else:
        for x in list:
            delete_log(x, delete_tag)
    
    
