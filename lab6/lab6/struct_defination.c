typedef struct Type {
    enum {
        BASIC,
        ARRAY,
        OBJECT
    } kind;
    union {
        enum {
            STRING,
            NUMBER,
            BOOL,
            VNULL
        } basic;
        struct ArrayList *ARRAYLIST;
        struct ObjectList *OBJECTLIST;
    };
} Type;

typedef struct ArrayList {
    struct Type *ELEM_TYPE;
    struct ArrayList *next;
} ArrayList;

typedef struct ObjectList {
    struct Type *STRING_KEY_TYPE;
    struct Type *VALUE_TYPE;
    struct ObjectList *next;
} ObjectList;