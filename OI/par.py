

def avaliable(s):
    init_sum = sum(s)
    pars = [0 for i in range(len(s) + 1)]

    for i in range(len(s)):
        a = s[:i]
        b = s[i:]
        pars[i] += sum(a)%2
        pars[len(s)-i] += sum(b)%2
    return pars


def highest_common(pars_a, pars_b):
    min_len = min(len(pars_a), len(pars_b))
    pars_a = pars_a[:min_len][::-1]
    pars_b = pars_b[:min_len][::-1]

    for counter, (a, b) in enumerate(zip(pars_a, pars_b)):
        #if a & b:
        if a==b or a==2 or b==2:
            return min_len - counter - 1


if __name__ == '__main__':
    q = int(input())
    m, n = list(map(int, input().split()))
    a = list(map(int, input().split()))
    b = list(map(int, input().split()))
    av_a = avaliable(a)
    av_b = avaliable(b)
    print(highest_common(av_a, av_b))

