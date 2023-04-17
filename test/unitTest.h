#ifndef UNITTEST_H
#define UNITTEST_H

#define assertEgal(fname, a, b) if(a!=b){\
        fprintf(stderr, "%s failed\n", fname);\
        ret = EXIT_FAILURE;\
        }


#endif