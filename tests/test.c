int tv1, tv2 = 1, tv3 = -32;
//ptr i = &tv1;

func int tfunc1(int a1, int a2) {
    int rv;
    rv = a1;
    rv = 2;
    rv++;
    rv--;
    rv += 2;
    rv += a2;
    rv -= a1;
    rv -= 8;
    rv *= 32;
    rv *= a2;
    rv /= 6;
    return rv;
}

func int tfunc2() { return; }
