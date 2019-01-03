#!/usr/bin/env python3
with open('HelloWorld.b') as doc:
    src = doc.read()

cmds = '+-><.,[]'

doc = ''.join(i for i in src if i in cmds)

print(doc)
