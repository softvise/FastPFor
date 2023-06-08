/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

/*  Based on code by
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *    which was available under the Apache License, Version 2.0.
 */

#ifndef VSENCODING_H_
#define VSENCODING_H_

#include "codecs.h"

/***
 * D. Lemire: for my purposes, I just took the code I needed from
 * http://integerencoding.isti.cnr.it/ after adapting the API somewhat
 * and some minor cleaning up/optimization.
 *
 * I put it inside a namespace because there are many global variables.
 * My minor changes did not affect performances in my tests, in fact,
 * I noticed a small gain in decompression speed in one test.
 *
 * Note: This code is not what I would consider model C++ code.
 *
 * The code below follows :
 * F. Silvestri and R. Venturini, VSEncoding: efficient coding and fast
 * decoding of integer lists via dynamic programming, in CIKM, 2010, pp. 1219-1228.
 */

namespace vsencoding {

class BitsWriter {
private:
    uint32_t *data;
    uint32_t Fill;
    uint64_t buffer;
public:
    uint32_t written;

    BitsWriter(uint32_t *out);
    void bit_flush();

    void bit_writer(uint32_t value, uint32_t bits);
};

/*
 * If a list is larger that this parameter, then
 * it is split into subblocks that are compressed
 * separatelly. Compression is slightly worse, but
 * the decompression is more cache-friendly.
 */
 //#ifndef VSENCODING_BLOCKSZ
 //#define VSENCODING_BLOCKSZ      65536U
 //#endif

class VSEncoding {
private:
    /*
     * In some cases, there might be asymmetry between possible
     * lenghts of blocks if they are formed by zeros(posszLens)
     * or larger numbers(possLens).
     */
    bool aligned;
    uint32_t *possLens;
    uint32_t *posszLens;
    uint32_t poss_sz;
    uint32_t maxBlk;

public:
    VSEncoding(uint32_t *lens, uint32_t *zlens, uint32_t size, bool cflag);

    /*
     * Compute the optimal sub-lists from lists.
     *      len: The length of the sequence of lists
     *      fixCost: The fix cost in bits that we pay for  each block
     */
    uint32_t *compute_OptPartition(uint32_t *seq, uint32_t len,
            uint32_t fixCost, uint32_t &pSize);
};

class VSEncodingBlocks: public FastPFor::IntegerCODEC {
public:
    enum {
        TAIL_MERGIN = 2048,// was 128 but I want to be safe (D. Lemire)
        VSEBLOCKS_LOGLEN = 4,
        VSEBLOCKS_LOGLOG = 4,
        VSEBLOCKS_LOGDESC = (VSEBLOCKS_LOGLEN + VSEBLOCKS_LOGLOG),
        VSEBLOCKS_LENS_LEN = (1 << VSEBLOCKS_LOGLEN),
        VSEBLOCKS_LOGS_LEN = (1 << VSEBLOCKS_LOGLOG)
    };
    VSEncodingBlocks(const uint32_t mVSENCODING_BLOCKSZ = 65536U) :
        VSENCODING_BLOCKSZ(mVSENCODING_BLOCKSZ),
                __tmp(VSENCODING_BLOCKSZ * 4 + VSEncodingBlocks::TAIL_MERGIN) {
    }

    void encodeVS(uint32_t len, const uint32_t *in, uint32_t &size,
            uint32_t *out);

    const uint32_t * decodeVS(uint32_t len, const uint32_t *in, uint32_t *out,
            uint32_t *aux);
    std::string name() const {
        return "VSEncoding";
    }

    /*
     * It assumes that values start form 0.
     *  - *in: points to the first d-gap to be encoded
     *  - *out: points to the first int that will countain the compress
     * It returns the compress size in number of int
     *
     * Note: *out must be large enough to contain the compress.
     */
    void encodeArray(const uint32_t *in, const size_t len, uint32_t *out,
            size_t &nvalue);

    const uint32_t * decodeArray(const uint32_t *in, const size_t len,
            uint32_t *out, size_t &nvalue);
    uint32_t VSENCODING_BLOCKSZ;//     = 65536U

    std::vector<uint32_t> __tmp;// = new uint32_t[VSENCODING_BLOCKSZ * 2 + VSEncodingBlocks::TAIL_MERGIN];

};

} // namespace vsencoding

#endif /* VSENCODING_H_ */
