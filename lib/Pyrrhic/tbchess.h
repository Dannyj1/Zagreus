#pragma once

#include <cstdint>

#include "tbprobe.h"

typedef struct PyrrhicPosition {
    uint64_t white, black;
    uint64_t kings, queens, rooks;
    uint64_t bishops, knights, pawns;
    uint8_t rule50, ep;
    bool turn;
} PyrrhicPosition;

enum {
    PYRRHIC_BLACK = 0,
    PYRRHIC_WHITE = 1,
    PYRRHIC_PAWN = 1,
    PYRRHIC_KNIGHT = 2,
    PYRRHIC_BISHOP = 3,
    PYRRHIC_ROOK = 4,
    PYRRHIC_QUEEN = 5,
    PYRRHIC_KING = 6,

    PYRRHIC_WPAWN = 1,
    PYRRHIC_BPAWN = 9,
    PYRRHIC_WKNIGHT = 2,
    PYRRHIC_BKNIGHT = 10,
    PYRRHIC_WBISHOP = 3,
    PYRRHIC_BBISHOP = 11,
    PYRRHIC_WROOK = 4,
    PYRRHIC_BROOK = 12,
    PYRRHIC_WQUEEN = 5,
    PYRRHIC_BQUEEN = 13,
    PYRRHIC_WKING = 6,
    PYRRHIC_BKING = 14,

    PYRRHIC_PROMOTES_NONE = 0,
    PYRRHIC_PROMOTES_QUEEN = 1,
    PYRRHIC_PROMOTES_ROOK = 2,
    PYRRHIC_PROMOTES_BISHOP = 3,
    PYRRHIC_PROMOTES_KNIGHT = 4,
};

enum : uint64_t {
    PYRRHIC_PROMOSQS = 0XFF000000000000FFULL,

    PYRRHIC_PRIME_WKING = 00000000000000000000ULL,
    PYRRHIC_PRIME_WQUEEN = 11811845319353239651ULL,
    PYRRHIC_PRIME_WROOK = 10979190538029446137ULL,
    PYRRHIC_PRIME_WBISHOP = 12311744257139811149ULL,
    PYRRHIC_PRIME_WKNIGHT = 15202887380319082783ULL,
    PYRRHIC_PRIME_WPAWN = 17008651141875982339ULL,
    PYRRHIC_PRIME_BKING = 00000000000000000000ULL,
    PYRRHIC_PRIME_BQUEEN = 15484752644942473553ULL,
    PYRRHIC_PRIME_BROOK = 18264461213049635989ULL,
    PYRRHIC_PRIME_BBISHOP = 15394650811035483107ULL,
    PYRRHIC_PRIME_BKNIGHT = 13469005675588064321ULL,
    PYRRHIC_PRIME_BPAWN = 11695583624105689831ULL,
    PYRRHIC_PRIME_NONE = 00000000000000000000ULL,
};

const char pyrrhic_piece_to_char[] = " PNBRQK  pnbrqk";

unsigned pyrrhic_move_from(PyrrhicMove move);
unsigned pyrrhic_move_to(PyrrhicMove move);
unsigned pyrrhic_move_promotes(PyrrhicMove move);

int pyrrhic_colour_of_piece(uint8_t piece);
int pyrrhic_type_of_piece(uint8_t piece);

bool pyrrhic_test_bit(uint64_t bb, int sq);
void pyrrhic_enable_bit(uint64_t* b, int sq);
void pyrrhic_disable_bit(uint64_t* b, int sq);
bool pyrrhic_promo_square(int sq);
bool pyrrhic_pawn_start_square(int colour, int sq);

bool pyrrhic_do_move(PyrrhicPosition* pos, const PyrrhicPosition* pos0, PyrrhicMove move);
bool pyrrhic_legal_move(const PyrrhicPosition* pos, PyrrhicMove move);

uint64_t pyrrhic_pieces_by_type(const PyrrhicPosition* pos, int colour, int piece);
int pyrrhic_char_to_piece_type(char c);

uint64_t pyrrhic_calc_key(const PyrrhicPosition* pos, int mirror);
uint64_t pyrrhic_calc_key_from_pcs(int* pieces, int mirror);
uint64_t pyrrhic_calc_key_from_pieces(uint8_t* pieces, int length);

uint64_t pyrrhic_do_bb_move(uint64_t bb, unsigned from, unsigned to);
PyrrhicMove pyrrhic_make_move(unsigned promote, unsigned from, unsigned to);
PyrrhicMove* pyrrhic_add_move(PyrrhicMove* moves, int promotes, unsigned from, unsigned to);

PyrrhicMove* pyrrhic_gen_captures(const PyrrhicPosition* pos, PyrrhicMove* moves);
PyrrhicMove* pyrrhic_gen_moves(const PyrrhicPosition* pos, PyrrhicMove* moves);
PyrrhicMove* pyrrhic_gen_legal(const PyrrhicPosition* pos, PyrrhicMove* moves);

bool pyrrhic_is_pawn_move(const PyrrhicPosition* pos, PyrrhicMove move);
bool pyrrhic_is_en_passant(const PyrrhicPosition* pos, PyrrhicMove move);
bool pyrrhic_is_capture(const PyrrhicPosition* pos, PyrrhicMove move);
bool pyrrhic_is_legal(const PyrrhicPosition* pos);
bool pyrrhic_is_check(const PyrrhicPosition* pos);
bool pyrrhic_is_mate(const PyrrhicPosition* pos);
