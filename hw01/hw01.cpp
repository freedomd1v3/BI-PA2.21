#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#endif  /* __PROGTEST__ */

const size_t MAX_BITS_IN_OTHER_BYTES = 6;
const size_t BITS = 8;

/**
 * Read the next byte of UTF-8 character
 * @param utf8
 * @param id
 * @param WHICH_BYTE
 * @return true, if everything went ok;
 *      false otherwise
 */
bool
read_next_utf8_byte(std::ifstream & utf8, int32_t & id,
                    const int WHICH_BYTE) {
    char c;
    utf8.get(c);
    if (!utf8.good()) {
        return false;
    }

    // 0xc0 = (11000000)bin, 0x80 = (10000000)bin
    if ((c & 0xc0) != 0x80) {
        return false;
    }
 
    // We ignore first 2 bits, so we're starting with 5 (6th bit)
    for (int i = 5; i >= 0; i--) {
        if ((c >> i) & 1) {
            int current_bit = WHICH_BYTE * MAX_BITS_IN_OTHER_BYTES + i;
            id |= 1 << current_bit;
        }
    }

    return true;
} 

/**
 * Get UTF-8 character's ID
 * @param utf8
 * @param id
 * @return 1, if everything was ok;
 *      EOF, if couldn't read the first byte;
 *          0, if got error
 */
int
get_utf8_id(std::ifstream & utf8, int32_t & id) {
    char c;
    utf8.get(c);
    if (!utf8.good()) {
        return EOF;
    }

    // 1st stage: getting amount of bytes (maximum is 4 in UTF-8)
    const int MAX_BYTES = 4;
    unsigned short other_bytes = 0;
    // If first bit is 1, then we must have valid prefix
    if (c & 0x80) { // 0x80 = (10000000)bin
        for (int i = 6; i >= 0; i--) {
            if ((c >> i) & 1) {
                other_bytes++;
            }
            else {
                break;
            }

            // Every character is encoded in 4 bytes max
            if (!(MAX_BYTES > other_bytes)) {
                return 0;
            }
        }
        // We've got a beginning of prefix, but it's empty.
        // What the fuck is wrong with this world?
        if (!other_bytes) {
            return 0;
        }
    }

    // 2nd stage: getting the ID itself
    id = 0;
    // Working out with the current byte (others are gonna be read
    // with another function, if needed)
    int i = 6;          // We always ignore first bit
    i -= other_bytes;   // Ignore bits in a prefix
    for (; i >= 0; i--) {
        if ((c >> i) & 1) {
            size_t current_bit = other_bytes * MAX_BITS_IN_OTHER_BYTES + i;
            id |= 1 << current_bit;
        }
    }

    // Reading remaining bytes
    for (i = other_bytes; i > 0; i--) {
        if (!read_next_utf8_byte(utf8, id, i - 1)) {
            return 0;
        }
    }

    return 1;
}

/**
 * Find the max possible fibonacci number
 * @param ch_id
 * @param i
 * @return This number
 */
int
find_max_fibonacci(int ch_id, long & i) {
    int first = 1, second = 2;
    i = 0;

    while (ch_id >= second) {
        int next = first + second;
        first = second;
        second = next;
        i++;
    }
    return first;
}

/**
 * Encode the number with Fibonacci code
 * @param ch_id
 * @param highest_bit
 * @return Encoded number
 */
int32_t
encode_fibonacci(int32_t num, long & highest_bit) {
    int32_t encoded = 0;
    while (num) {
        long bit = 0;
        int next = find_max_fibonacci(num, bit);
        if (!highest_bit) {
            highest_bit = bit + 1;  // We will need to append '1' to the end
        }
        encoded |= 1 << bit;
        num -= next;
    }
    encoded |= 1 << highest_bit;
    return encoded;
}

bool
utf8ToFibonacci(const char * inFile, const char * outFile) {
    std::ifstream utf8_file;
    utf8_file.open(inFile);
    if (!utf8_file.is_open()) {
        return false;
    }

    std::ofstream out;
    out.open(outFile);
    if (!out.is_open()) {
        return false;
    }

    unsigned char write = 0;
    size_t bits_written = 0;
    for (;;) {
        // We really EXPLICITLY want int32_t here
        int32_t ch_id;
        int get_utf8_id_status = get_utf8_id(utf8_file, ch_id);
        if (!get_utf8_id_status) {
            return false;
        }
        else if (get_utf8_id_status == EOF) {
            break;
        } 

        long bits_to_write = 0;
        int32_t encoded_ch_id = encode_fibonacci(ch_id + 1, bits_to_write);
        // +1, because we need to shift number to the right
        for (long i = 0; i <= bits_to_write; i++) {
            if ((encoded_ch_id >> i) & 1) {
                write |= 1 << bits_written;
            }
            if (++bits_written >= BITS) {
                out << write;
                if (!out.good()) {
                    return false;
                }
                write = 0;
                bits_written = 0;
            }
        }
    }
    if (write) {
        out << write;
        if (!out.good()) {
            return false;
        }
    }

    out.close();
    if (!out.good()) {
        return false;
    }
    return true;
}

/**
 * Find fibonacci element by its ID
 * @param ID
 * @return Fibonacci element with provided ID
 */
int
find_fib_elem(const size_t ID) {
    int first = 1, second = 2;
    for (size_t i = 0; i < ID; i++) {
        int next = first + second;
        first = second;
        second = next;
    }
    return first;
}

/**
 * Try to decode number encoded in Fibonacci code
 * @param fib_file
 * @param c
 * @param bits_read
 * @param decoded
 * @return true, if everything went ok;
 *      false, if got error
 */
int
decode_fibonacci(std::ifstream & fib_file, char & c, size_t & bits_read,
                 int32_t & decoded) {
    const bool IN = true, OUT = false;
    bool state = OUT, last_bit_was_one = false, read = false;
    size_t cur_elem = 0;
    for (;;) {
        // We haven't read any byte yet, or the last one was read completely
        if (!bits_read || bits_read >= BITS) {
            fib_file.get(c);
            if (fib_file.eof()) {
                if (state == IN) {
                    return 0;
                }
                return EOF;
            }
            bits_read = 0;
        }

        for (; bits_read < BITS; bits_read++) {
            if ((c >> bits_read) & 1) {
                if (!bits_read && last_bit_was_one) {
                    state = OUT;
                    bits_read++;
                    read = true;
                    break;
                }

                state = IN;
                decoded += find_fib_elem(cur_elem);

                if (decoded - 1 > 0x10ffff) {
                    return 0;   // Too big number
                }
                else if (bits_read + 1 < BITS && (c >> (bits_read + 1)) & 1) {
                    state = OUT;
                    bits_read += 2; // We want to skip 2 bits here
                    read = true;
                    break;
                } 

                else if (bits_read == BITS - 1) {
                    last_bit_was_one = true;
                }
            }
            cur_elem++;
            if (bits_read != BITS - 1) {
                last_bit_was_one = false;
            }
        }
        if (read) {
            break;
        }
    }

    decoded -= 1;
    return 1;
}

/**
 * Encode first UTF-8 byte
 * @param first_byte
 * @param BYTES_IN_TOTAL
 * @param TO_ENC
 */
void
encode_first_byte(unsigned char & first_byte, const int BYTES_IN_TOTAL,
                  const int32_t TO_ENC) {
    if (BYTES_IN_TOTAL > 1) {
        for (size_t i = 7; i > BITS - 1 - BYTES_IN_TOTAL; i--) {
            first_byte |= 1 << i;
        }
    }
    for (int i = BITS - BYTES_IN_TOTAL; i >= 0; i--) {
        if ((TO_ENC >> (i + (BYTES_IN_TOTAL - 1) * MAX_BITS_IN_OTHER_BYTES)) & 1) {
            first_byte |= 1 << i;
        }
    }
}

/**
 * Encode other UTF-8 byte
 * @param out
 * @param WHICH_BYTE
 * @param TO_ENC
 * @return Encoded byte
 */
unsigned char
encode_other_byte(std::ofstream & out, const int WHICH_BYTE,
                  const int32_t TO_ENC) {
    unsigned char other_byte = 0x80;
    for (int i = 5; i >= 0; i--) {
        if ((TO_ENC >> (i + (WHICH_BYTE - 1) * MAX_BITS_IN_OTHER_BYTES)) & 1) {
            other_byte |= 1 << i;
        }
    }
    return other_byte;
}

/**
 * Tries to encode characters to UTF-8. If for whatever reason
 * can't encode any character, returns false
 * @param out
 * @param num
 * @return true, if everything went fine;
 *      false otherwise
 */
bool
encode_utf8(std::ofstream & out, int32_t num) {
    // 0x10ffff = (100001111111111111111)bin
    if (num > 0x10ffff) {
        return false;
    }

    unsigned char first_byte = 0;
    int other_bytes = 0;    // 1 byte by default
    // 0xffff = (1111111111111111)bin
    if (num > 0xffff) {
        // 4 bytes
        other_bytes = 3;
    }

    // 0x7ff = (11111111111)bin
    else if (num > 0x7ff) {
        // 3 bytes
        other_bytes = 2;
    }

    // 0x7f = (1111111)bin
    else if (num > 0x7f) {
        // 2 bytes
        other_bytes = 1;
    }

    encode_first_byte(first_byte, other_bytes + 1, num);
    out << first_byte;
    if (!out.good()) {
        return false;
    }

    // Encoding other bytes
    for (int i = other_bytes; i > 0; i--) {
        unsigned char next_byte = encode_other_byte(out, i, num);
        out << next_byte;
        if (!out.good()) {
            return false;
        }
    }
    return true;
}
  
bool
fibonacciToUtf8(const char * inFile, const char * outFile) {
    std::ifstream fib_file (inFile);
    if (!fib_file.is_open()) {
        return false;
    }
    char c;
    size_t bits_read = 0;

    std::ofstream out;
    out.open(outFile);
    if (!out.is_open()) {
        return false;
    }

    for (;;) {
        int32_t num = 0;
        int decode_fibonacci_status = decode_fibonacci(fib_file, c, bits_read,
                                                       num);
        if (!decode_fibonacci_status) {
            return false;
        }
        else if (decode_fibonacci_status == EOF) {
            break;
        }

        if (!encode_utf8(out, num)) {
            // This will never happen though
            return false;
        }
    }

    out.close();
    if (!out.good()) {
        return false;
    }
    return true;
}
  
#ifndef __PROGTEST__
bool
identicalFiles(const char * file1, const char * file2) {
    std::ifstream f1, f2;
    f1.open(file1), f2.open(file2);
    for (;;) {
        char c1, c2;
        f1.get(c1), f2.get(c2);
        if (f1.good() != f2.good()) {
            return false;
        }
        else if (c1 != c2) {
            return false;
        }

        // We got either an end of file, or file reading failed.
        // There won't be any errors like the latter on ProgTest
        // (I'm 99% sure about this),
        // so I don't even want to spend time detecting them
        if (!f1.good()) {
            break;
        }
    }
    return true;
}

int
main(void) {
    // 1st stage: UTF-8 to Fibonacci
    assert(utf8ToFibonacci("example/src_0.utf8", "output.fib")
           &&identicalFiles("output.fib", "example/dst_0.fib"));
    assert(utf8ToFibonacci("example/src_1.utf8", "output.fib")
           && identicalFiles("output.fib", "example/dst_1.fib"));
    assert(utf8ToFibonacci("example/src_2.utf8", "output.fib")
           && identicalFiles("output.fib", "example/dst_2.fib"));
    assert(utf8ToFibonacci("example/src_3.utf8", "output.fib")
           && identicalFiles("output.fib", "example/dst_3.fib"));
    assert(utf8ToFibonacci("example/src_4.utf8", "output.fib")
           && identicalFiles("output.fib", "example/dst_4.fib"));
    assert(!utf8ToFibonacci("example/src_5.utf8", "output.fib"));

    // 2nd stage: Fibonacci to UTF-8
    assert(fibonacciToUtf8("example/src_6.fib", "output.utf8")
           && identicalFiles("output.utf8", "example/dst_6.utf8"));
    assert(fibonacciToUtf8("example/src_7.fib", "output.utf8")
           && identicalFiles("output.utf8", "example/dst_7.utf8"));
    assert(fibonacciToUtf8("example/src_8.fib", "output.utf8")
           && identicalFiles("output.utf8", "example/dst_8.utf8"));
    assert(fibonacciToUtf8("example/src_9.fib", "output.utf8")
           && identicalFiles("output.utf8", "example/dst_9.utf8"));
    assert(fibonacciToUtf8("example/src_10.fib", "output.utf8")
           && identicalFiles("output.utf8", "example/dst_10.utf8"));
    assert(!fibonacciToUtf8("example/src_11.fib", "output.utf8"));

    // 3rd stage: Special tests, failed on ProgTest before
    assert(!fibonacciToUtf8("example/src_12.fib", "output.utf8"));
    assert(!fibonacciToUtf8("example/src_13.fib", "output.utf8"));

    return 0;
}
#endif  /* __PROGTEST__ */
