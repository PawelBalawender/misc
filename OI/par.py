

def avaliable(s):
    pars = [0 for i in range(len(s) + 1)]  # +1 to include full-len substring

    a = 0
    b = sum(s) % 2

    for i in range(len(s)):
        ind_a = i
        ind_b = len(s) - i
        pars[ind_a] += a
        pars[ind_b] += b
        if s[i] % 2:
            a = not a
            b = not b

    pars[0] *= 2
    pars[-1] *= 2

    cp = []
    for i in pars:
        if i == 0:
            cp += [0]
        elif i == 1:
            cp += [2]
        elif i == 2:
            cp += [1]
    pars=cp
    return pars


def highest_common(pars_a, pars_b):
    min_len = min(len(pars_a), len(pars_b))
    pars_a = pars_a[:min_len][::-1]
    pars_b = pars_b[:min_len][::-1]

    for counter, (a, b) in enumerate(zip(pars_a, pars_b)):
        if a==b or a==2 or b==2:
            return min_len - counter - 1


if __name__ == '__main__':
    q = int(input())
    m, n = list(map(int, input().split()))
    a = list(map(int, input().split()))
    b = list(map(int, input().split()))
    av_a = avaliable(a)
    av_b = avaliable(b)
    #print(av_a)
    print(highest_common(av_a, av_b))

