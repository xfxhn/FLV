
#ifndef MUX_SCRIPTTAG_H
#define MUX_SCRIPTTAG_H


#include <cstdint>
#include <fstream>

enum marker {
    number_marker,
    boolean_marker,
    string_marker = 2,
    object_marker,
    movieclip_marker,
    null_marker,
    undefined_marker,
    reference_marker,
    ecma_array_marker,
    object_end_marker,
    strict_array_marker,
    date_marker,
    long_string_marker,
    unsupported_marker,
    recordset_marker,
    xml_document_marker,
    typed_object_marker
};

struct Item {
    const char *name;
    struct {
        bool b;
        double n;
        const char *s;
    } u;
    marker type;
};


class ScriptTag {
public:
    static int write(std::ofstream &fs);

    static uint64_t reverse(uint8_t *arr, int n);
};


#endif //MUX_SCRIPTTAG_H
