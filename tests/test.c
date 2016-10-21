int tv1, tv2 = 1, tv3 = -32;
//ptr i = &tv1;

func int tfunc1(int a1, int a2) {
    int rv, rtv, rv_ptr;

    rv = 32;
    rv += 2;
    rv_ptr = &rv;

    rtv = *rv_ptr;

    return rv;
}

func int tfunc2() { return; }
