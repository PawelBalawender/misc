if __name__ == '__main__':
    a = input()
    b = input()
    s = ''
    ind = 0
    for i in a:
        x = b.find(i, ind)
        if x != -1:
            ind = x+1
            s += i
        if ind>=len(b):break
    print(s)

