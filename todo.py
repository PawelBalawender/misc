#!/usr/bin/env python3
"""
todo:
TodoList class
argcomplete
"""
import argparse
import sys

todo_list = ['feed the dog']


def add(task):
    todo_list.append(task)


def rm(task):
    todo_list.remove(task)


def echo():
    for i, task in enumerate(todo_list):
        print('{num}. {task}'.format(num=i+1, task=task))
    

if __name__ == '__main__':
    cmds = {'echo':echo, 'add':add, 'rm':rm}

    parser = argparse.ArgumentParser(description="Menage your todo list")
    parser.add_argument('cmd', choices=cmds)
    parser.add_argument('words', nargs='*')

    # there is no way to set some default argument for 'choices' in argparse
    if len(sys.argv) == 1 or sys.argv[1] not in cmds.keys():
        sys.argv.insert(1, 'echo')
    
    args = parser.parse_args()
    
    print(vars(args))
