from Crypto.Util import number
from Crypto.Hash import SHA256

N = 1024


class ElGamalData:
    def __init__(self):
        self.p = 0
        self.g = 0
        self.x = 0
        self.y = 0

    def gen_key(self):
        self.p = number.getPrime(N)
        g0 = number.getRandomRange(2, self.p - 1)
        self.g = g0
        while number.GCD(self.g, self.p) != 1:
            self.g = number.getRandomRange(2, g0) if self.g + 1 == self.p else self.g + 1
        self.x = number.getRandomRange(2, self.p - 1)
        self.y = pow(self.g, self.x, self.p)

    def sign(self, m: bytes) -> (int, int):
        k0 = number.getRandomRange(1, self.p - 1)
        k = k0
        while number.GCD(k, self.p - 1) != 1:
            k = number.getRandomRange(1, k0) if k + 1 == self.p - 1 else k + 1
        h = SHA256.new()
        h.update(m)
        hm = int(h.hexdigest(), 16)
        r = pow(self.g, k, self.p)
        s = (number.inverse(k, self.p - 1) * (hm - self.x * r)) % (self.p - 1)
        return r, s

    def validate(self, m: bytes, pubkey: (int, int, int), h: (int, int)) -> bool:
        p, g, y = pubkey
        r, s = h
        yrrs = (pow(y, r, p) * pow(r, s, p)) % p
        h = SHA256.new()
        h.update(m)
        hm = int(h.hexdigest(), 16)
        ghm = pow(g, hm, p)
        return yrrs == ghm

    def pubkey(self) -> (int, int, int):
        return self.p, self.g, self.y


d = ElGamalData()
d.gen_key()

print('p =', d.p)
print('g =', d.g)
print('x =', d.x)
print('y =', d.y)

m = b'200110402'
h = d.sign(m)

print('r =', h[0])
print('s =', h[1])

print(d.validate(m, d.pubkey(), h))
