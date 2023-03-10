/* stm8-opc.c -- Table of opcodes for the STM8 processor.
   Copyright (C) 2007-2018 Free Software Foundation, Inc.
   Contributed by Ake Rehnman ake dot rehnman (at) gmail dot com

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <stdio.h>
#include "libiberty.h"
#include "symcat.h"
#include "opcode/stm8.h"

const struct stm8_opcodes_s stm8_opcodes[] =
{
//mcs51 opcode
{"nop", {}, 0x00},
{"ajmp", {ST8_SHORTMEM}, 0x01},
{"ljmp", {ST8_LONGMEM}, 0x02},
{"rr", {ST8_REG_A}, 0x03},
{"inc", {ST8_REG_A}, 0x04},
{"inc", {ST8_SHORTMEM}, 0x05},
{"inc", {ST8_REG_AT_R0}, 0x06},
{"inc", {ST8_REG_AT_R1}, 0x07},
{"inc", {ST8_REG_R0}, 0x08},
{"inc", {ST8_REG_R1}, 0x09},
{"inc", {ST8_REG_R2}, 0x0A},
{"inc", {ST8_REG_R3}, 0x0B},
{"inc", {ST8_REG_R4}, 0x0C},
{"inc", {ST8_REG_R5}, 0x0D},
{"inc", {ST8_REG_R6}, 0x0E},
{"inc", {ST8_REG_R7}, 0x0F},
{"jbc", {ST8_SHORTMEM, ST8_PCREL}, 0x10},
{"acall", {ST8_SHORTMEM}, 0x11},
{"lcall", {ST8_LONGMEM}, 0x12},
{"rrc", {ST8_REG_A}, 0x13},
{"dec", {ST8_REG_A}, 0x14},
{"dec", {ST8_SHORTMEM}, 0x15},
{"dec", {ST8_REG_AT_R0}, 0x16},
{"dec", {ST8_REG_AT_R1}, 0x17},
{"dec", {ST8_REG_R0}, 0x18},
{"dec", {ST8_REG_R1}, 0x19},
{"dec", {ST8_REG_R2}, 0x1A},
{"dec", {ST8_REG_R3}, 0x1B},
{"dec", {ST8_REG_R4}, 0x1C},
{"dec", {ST8_REG_R5}, 0x1D},
{"dec", {ST8_REG_R6}, 0x1E},
{"dec", {ST8_REG_R7}, 0x1F},
{"jb", {ST8_SHORTMEM, ST8_PCREL}, 0x20},
{"ajmp", {ST8_SHORTMEM}, 0x21},
{"ret", {}, 0x22},
{"rl", {ST8_REG_A}, 0x23},
{"add", {ST8_BYTE}, 0x24},
{"add", {ST8_SHORTMEM}, 0x25},
{"add", {ST8_REG_A, ST8_REG_AT_R0}, 0x26},
{"add", {ST8_REG_A, ST8_REG_AT_R1}, 0x27},
{"add", {ST8_REG_A, ST8_REG_R0}, 0x28},
{"add", {ST8_REG_A, ST8_REG_R1}, 0x29},
{"add", {ST8_REG_A, ST8_REG_R2}, 0x2A},
{"add", {ST8_REG_A, ST8_REG_R3}, 0x2B},
{"add", {ST8_REG_A, ST8_REG_R4}, 0x2C},
{"add", {ST8_REG_A, ST8_REG_R5}, 0x2D},
{"add", {ST8_REG_A, ST8_REG_R6}, 0x2E},
{"add", {ST8_REG_A, ST8_REG_R7}, 0x2F},
{"jnb", {ST8_SHORTMEM, ST8_PCREL}, 0x30},
{"acall", {ST8_SHORTMEM}, 0x31},
{"reti", {}, 0x32},
{"rlc", {ST8_REG_A}, 0x33},
{"addc", {ST8_REG_A, ST8_BYTE}, 0x34},
{"addc", {ST8_REG_A, ST8_SHORTMEM}, 0x35},
{"addc", {ST8_REG_A, ST8_REG_AT_R0}, 0x36},
{"addc", {ST8_REG_A, ST8_REG_AT_R1}, 0x37},
{"addc", {ST8_REG_A, ST8_REG_R0}, 0x38},
{"addc", {ST8_REG_A, ST8_REG_R1}, 0x39},
{"addc", {ST8_REG_A, ST8_REG_R2}, 0x3A},
{"addc", {ST8_REG_A, ST8_REG_R3}, 0x3B},
{"addc", {ST8_REG_A, ST8_REG_R4}, 0x3C},
{"addc", {ST8_REG_A, ST8_REG_R5}, 0x3D},
{"addc", {ST8_REG_A, ST8_REG_R6}, 0x3E},
{"addc", {ST8_REG_A, ST8_REG_R7}, 0x3F},
{"jc", {ST8_PCREL}, 0x40},
{"ajmp", {ST8_SHORTMEM}, 0x41},
{"orl", {ST8_SHORTMEM}, 0x42},
{"orl", {ST8_SHORTMEM, ST8_BYTE}, 0x43},
{"orl", {ST8_REG_A, ST8_BYTE}, 0x44},
{"orl", {ST8_REG_A, ST8_SHORTMEM}, 0x45},
{"orl", {ST8_REG_A, ST8_REG_AT_R0}, 0x46},
{"orl", {ST8_REG_A, ST8_REG_AT_R1}, 0x47},
{"orl", {ST8_REG_A, ST8_REG_R0}, 0x48},
{"orl", {ST8_REG_A, ST8_REG_R1}, 0x49},
{"orl", {ST8_REG_A, ST8_REG_R2}, 0x4A},
{"orl", {ST8_REG_A, ST8_REG_R3}, 0x4B},
{"orl", {ST8_REG_A, ST8_REG_R4}, 0x4C},
{"orl", {ST8_REG_A, ST8_REG_R5}, 0x4D},
{"orl", {ST8_REG_A, ST8_REG_R6}, 0x4E},
{"orl", {ST8_REG_A, ST8_REG_R7}, 0x4F},
{"jnc", {ST8_PCREL}, 0x50},
{"acall", {ST8_SHORTMEM}, 0x51},
{"anl", {ST8_SHORTMEM, ST8_REG_A}, 0x52},
{"anl", {ST8_SHORTMEM, ST8_BYTE}, 0x53},
{"anl", {ST8_REG_A, ST8_BYTE}, 0x54},
{"anl", {ST8_REG_A, ST8_SHORTMEM}, 0x55},
{"anl", {ST8_REG_A, ST8_REG_AT_R0}, 0x56},
{"anl", {ST8_REG_A, ST8_REG_AT_R1}, 0x57},
{"anl", {ST8_REG_A, ST8_REG_R0}, 0x58},
{"anl", {ST8_REG_A, ST8_REG_R1}, 0x59},
{"anl", {ST8_REG_A, ST8_REG_R2}, 0x5A},
{"anl", {ST8_REG_A, ST8_REG_R3}, 0x5B},
{"anl", {ST8_REG_A, ST8_REG_R4}, 0x5C},
{"anl", {ST8_REG_A, ST8_REG_R5}, 0x5D},
{"anl", {ST8_REG_A, ST8_REG_R6}, 0x5E},
{"anl", {ST8_REG_A, ST8_REG_R7}, 0x5F},
{"jz", {ST8_PCREL}, 0x60},
{"ajmp", {ST8_SHORTMEM}, 0x61},
{"xrl", {ST8_SHORTMEM, ST8_REG_A}, 0x62},
{"xrl", {ST8_SHORTMEM, ST8_BYTE}, 0x63},
{"xrl", {ST8_REG_A, ST8_BYTE}, 0x64},
{"xrl", {ST8_REG_A, ST8_SHORTMEM}, 0x65},
{"xrl", {ST8_REG_A, ST8_REG_AT_R0}, 0x66},
{"xrl", {ST8_REG_A, ST8_REG_AT_R1}, 0x67},
{"xrl", {ST8_REG_A, ST8_REG_R0}, 0x68},
{"xrl", {ST8_REG_A, ST8_REG_R1}, 0x69},
{"xrl", {ST8_REG_A, ST8_REG_R2}, 0x6A},
{"xrl", {ST8_REG_A, ST8_REG_R3}, 0x6B},
{"xrl", {ST8_REG_A, ST8_REG_R4}, 0x6C},
{"xrl", {ST8_REG_A, ST8_REG_R5}, 0x6D},
{"xrl", {ST8_REG_A, ST8_REG_R6}, 0x6E},
{"xrl", {ST8_REG_A, ST8_REG_R7}, 0x6F},
{"jnz", {ST8_PCREL}, 0x70},
{"acall", {ST8_SHORTMEM}, 0x71},
{"orl", {ST8_REG_C, ST8_SHORTMEM}, 0x72},
{"jmp", {ST8_REG_AT_A_DPTR}, 0x73},
{"mov", {ST8_REG_A, ST8_BYTE}, 0x74},
{"mov", {ST8_SHORTMEM, ST8_BYTE}, 0x75},
{"mov", {ST8_REG_AT_R0, ST8_BYTE}, 0x76},
{"mov", {ST8_REG_AT_R1, ST8_BYTE}, 0x77},
{"mov", {ST8_REG_R0, ST8_BYTE}, 0x78},
{"mov", {ST8_REG_R1, ST8_BYTE}, 0x79},
{"mov", {ST8_REG_R2, ST8_BYTE}, 0x7A},
{"mov", {ST8_REG_R3, ST8_BYTE}, 0x7B},
{"mov", {ST8_REG_R4, ST8_BYTE}, 0x7C},
{"mov", {ST8_REG_R5, ST8_BYTE}, 0x7D},
{"mov", {ST8_REG_R6, ST8_BYTE}, 0x7E},
{"mov", {ST8_REG_R7, ST8_BYTE}, 0x7F},
{"sjmp", {ST8_PCREL}, 0x80},
{"ajmp", {ST8_SHORTMEM}, 0x81},
{"anl", {ST8_REG_C, ST8_SHORTMEM}, 0x82},
{"movc", {ST8_REG_A, ST8_REG_AT_A_PC}, 0x83},
{"div", {ST8_REG_AB}, 0x84},
{"mov", {ST8_SHORTMEM, ST8_SHORTMEM}, 0x85},
{"mov", {ST8_SHORTMEM, ST8_REG_AT_R0}, 0x86},
{"mov", {ST8_SHORTMEM, ST8_REG_AT_R1}, 0x87},
{"mov", {ST8_SHORTMEM, ST8_REG_R0}, 0x88},
{"mov", {ST8_SHORTMEM, ST8_REG_R1}, 0x89},
{"mov", {ST8_SHORTMEM, ST8_REG_R2}, 0x8A},
{"mov", {ST8_SHORTMEM, ST8_REG_R3}, 0x8B},
{"mov", {ST8_SHORTMEM, ST8_REG_R4}, 0x8C},
{"mov", {ST8_SHORTMEM, ST8_REG_R5}, 0x8D},
{"mov", {ST8_SHORTMEM, ST8_REG_R6}, 0x8E},
{"mov", {ST8_SHORTMEM, ST8_REG_R7}, 0x8F},
{"mov", {ST8_REG_DPTR, ST8_WORD}, 0x90},
{"acall", {ST8_SHORTMEM}, 0x91},
{"mov", {ST8_SHORTMEM, ST8_REG_C}, 0x92},
{"movc", {ST8_REG_A, ST8_REG_AT_A_DPTR}, 0x93},
{"subb", {ST8_REG_A, ST8_BYTE}, 0x94},
{"subb", {ST8_REG_A, ST8_SHORTMEM}, 0x95},
{"subb", {ST8_REG_A, ST8_REG_AT_R0}, 0x96},
{"subb", {ST8_REG_A, ST8_REG_AT_R1}, 0x97},
{"subb", {ST8_REG_A, ST8_REG_R0}, 0x98},
{"subb", {ST8_REG_A, ST8_REG_R1}, 0x99},
{"subb", {ST8_REG_A, ST8_REG_R2}, 0x9A},
{"subb", {ST8_REG_A, ST8_REG_R3}, 0x9B},
{"subb", {ST8_REG_A, ST8_REG_R4}, 0x9C},
{"subb", {ST8_REG_A, ST8_REG_R5}, 0x9D},
{"subb", {ST8_REG_A, ST8_REG_R6}, 0x9E},
{"subb", {ST8_REG_A, ST8_REG_R7}, 0x9F},
{"orl", {ST8_REG_C, ST8_SHORTMEM}, 0xA0},
{"ajmp", {ST8_SHORTMEM}, 0xA1},
{"mov", {ST8_REG_C, ST8_SHORTMEM}, 0xA2},
{"inc", {ST8_REG_DPTR}, 0xA3},
{"mul", {ST8_REG_AB}, 0xA4},
//{"", {}, 0xA5},
{"mov", {ST8_REG_AT_R0, ST8_SHORTMEM}, 0xA6},
{"mov", {ST8_REG_AT_R1, ST8_SHORTMEM}, 0xA7},
{"mov", {ST8_REG_R0, ST8_SHORTMEM}, 0xA8},
{"mov", {ST8_REG_R1, ST8_SHORTMEM}, 0xA9},
{"mov", {ST8_REG_R2, ST8_SHORTMEM}, 0xAA},
{"mov", {ST8_REG_R3, ST8_SHORTMEM}, 0xAB},
{"mov", {ST8_REG_R4, ST8_SHORTMEM}, 0xAC},
{"mov", {ST8_REG_R5, ST8_SHORTMEM}, 0xAD},
{"mov", {ST8_REG_R6, ST8_SHORTMEM}, 0xAE},
{"mov", {ST8_REG_R7, ST8_SHORTMEM}, 0xAF},
{"anl", {ST8_REG_C, ST8_SHORTMEM}, 0xB0},
{"acall", {ST8_SHORTMEM}, 0xB1},
{"cpl", {ST8_SHORTMEM}, 0xB2},
{"cpl", {ST8_REG_C}, 0xB3},
{"cjne", {ST8_REG_A, ST8_BYTE, ST8_PCREL}, 0xB4},
{"cjne", {ST8_REG_A, ST8_SHORTMEM, ST8_PCREL}, 0xB5},
{"cjne", {ST8_REG_AT_R0, ST8_BYTE, ST8_PCREL}, 0xB6},
{"cjne", {ST8_REG_AT_R1, ST8_BYTE, ST8_PCREL}, 0xB7},
{"cjne", {ST8_REG_R0, ST8_BYTE, ST8_PCREL}, 0xB8},
{"cjne", {ST8_REG_R1, ST8_BYTE, ST8_PCREL}, 0xB9},
{"cjne", {ST8_REG_R2, ST8_BYTE, ST8_PCREL}, 0xBA},
{"cjne", {ST8_REG_R3, ST8_BYTE, ST8_PCREL}, 0xBB},
{"cjne", {ST8_REG_R4, ST8_BYTE, ST8_PCREL}, 0xBC},
{"cjne", {ST8_REG_R5, ST8_BYTE, ST8_PCREL}, 0xBD},
{"cjne", {ST8_REG_R6, ST8_BYTE, ST8_PCREL}, 0xBE},
{"cjne", {ST8_REG_R7, ST8_BYTE, ST8_PCREL}, 0xBF},
{"push", {ST8_SHORTMEM}, 0xC0},
{"ajmp", {ST8_SHORTMEM}, 0xC1},
{"clr", {ST8_SHORTMEM}, 0xC2},
{"clr", {ST8_REG_C}, 0xC3},
{"swap", {ST8_REG_A}, 0xC4},
{"xch", {ST8_REG_A, ST8_SHORTMEM}, 0xC5},
{"xch", {ST8_REG_A, ST8_REG_AT_R0}, 0xC6},
{"xch", {ST8_REG_A, ST8_REG_AT_R1}, 0xC7},
{"xch", {ST8_REG_A, ST8_REG_R0}, 0xC8},
{"xch", {ST8_REG_A, ST8_REG_R1}, 0xC9},
{"xch", {ST8_REG_A, ST8_REG_R2}, 0xCA},
{"xch", {ST8_REG_A, ST8_REG_R3}, 0xCB},
{"xch", {ST8_REG_A, ST8_REG_R4}, 0xCC},
{"xch", {ST8_REG_A, ST8_REG_R5}, 0xCD},
{"xch", {ST8_REG_A, ST8_REG_R6}, 0xCE},
{"xch", {ST8_REG_A, ST8_REG_R7}, 0xCF},
{"pop", {ST8_SHORTMEM}, 0xD0},
{"acall", {ST8_SHORTMEM}, 0xD1},
{"setb", {ST8_SHORTMEM}, 0xD2},
{"setb", {ST8_REG_C}, 0xD3},
{"da", {ST8_REG_A}, 0xD4},
{"djnz", {ST8_LONGMEM, ST8_PCREL}, 0xD5},
{"xchd", {ST8_REG_A, ST8_REG_AT_R0}, 0xD6},
{"xchd", {ST8_REG_A, ST8_REG_AT_R1}, 0xD7},
{"djnz", {ST8_REG_R0, ST8_PCREL}, 0xD8},
{"djnz", {ST8_REG_R1, ST8_PCREL}, 0xD9},
{"djnz", {ST8_REG_R2, ST8_PCREL}, 0xDA},
{"djnz", {ST8_REG_R3, ST8_PCREL}, 0xDB},
{"djnz", {ST8_REG_R4, ST8_PCREL}, 0xDC},
{"djnz", {ST8_REG_R5, ST8_PCREL}, 0xDD},
{"djnz", {ST8_REG_R6, ST8_PCREL}, 0xDE},
{"djnz", {ST8_REG_R7, ST8_PCREL}, 0xDF},
{"movx", {ST8_REG_A, ST8_REG_AT_DPTR}, 0xE0},
{"ajmp", {ST8_SHORTMEM}, 0xE1},
{"movx", {ST8_REG_A, ST8_REG_AT_R0}, 0xE2},
{"movx", {ST8_REG_A, ST8_REG_AT_R1}, 0xE3},
{"clr", {ST8_REG_A}, 0xE4},
{"mov", {ST8_REG_A, ST8_SHORTMEM}, 0xE5},
{"mov", {ST8_REG_A, ST8_REG_AT_R0}, 0xE6},
{"mov", {ST8_REG_A, ST8_REG_AT_R1}, 0xE7},
{"mov", {ST8_REG_A, ST8_REG_R0}, 0xE8},
{"mov", {ST8_REG_A, ST8_REG_R1}, 0xE9},
{"mov", {ST8_REG_A, ST8_REG_R2}, 0xEA},
{"mov", {ST8_REG_A, ST8_REG_R3}, 0xEB},
{"mov", {ST8_REG_A, ST8_REG_R4}, 0xEC},
{"mov", {ST8_REG_A, ST8_REG_R5}, 0xED},
{"mov", {ST8_REG_A, ST8_REG_R6}, 0xEE},
{"mov", {ST8_REG_A, ST8_REG_R7}, 0xEF},
{"movx", {ST8_REG_AT_DPTR, ST8_REG_A}, 0xF0},
{"acall", {ST8_SHORTMEM}, 0xF1},
{"movx", {ST8_REG_AT_R0, ST8_REG_A}, 0xF2},
{"movx", {ST8_REG_AT_R1, ST8_REG_A}, 0xF3},
{"cpl", {ST8_REG_A}, 0xF4},
{"mov", {ST8_SHORTMEM, ST8_REG_A}, 0xF5},
{"mov", {ST8_REG_AT_R0, ST8_REG_A}, 0xF6},
{"mov", {ST8_REG_AT_R1, ST8_REG_A}, 0xF7},
{"mov", {ST8_REG_R0, ST8_REG_A}, 0xF8},
{"mov", {ST8_REG_R1, ST8_REG_A}, 0xF9},
{"mov", {ST8_REG_R2, ST8_REG_A}, 0xFA},
{"mov", {ST8_REG_R3, ST8_REG_A}, 0xFB},
{"mov", {ST8_REG_R4, ST8_REG_A}, 0xFC},
{"mov", {ST8_REG_R5, ST8_REG_A}, 0xFD},
{"mov", {ST8_REG_R6, ST8_REG_A}, 0xFE},
{"mov", {ST8_REG_R7, ST8_REG_A}, 0xFF},

  {NULL, {ST8_END}, 0},
};

int stm8_num_opcode_operands(struct stm8_opcodes_s opcode)
{
	int i;
	for(i = 0; opcode.constraints[i]; i++);
	return i;
}

unsigned int stm8_opcode_size(unsigned int number)
{
	int i;
	for(i = sizeof(int); i > 0; i--) {
		if(number & 0xFF << (i-1)*8)
			return(i);
	}
	return(1);
}

int stm8_compute_insn_size(struct stm8_opcodes_s opcode) {
	int i, ret = 0;
	for(i = 0; opcode.constraints[i] != ST8_END; i++) {
		switch(opcode.constraints[i]) {
		case ST8_PCREL:
		case ST8_SHORTMEM:
		case ST8_BYTE:
		case ST8_SHORTPTRW_Y:
		case ST8_SHORTPTRW_X:
		case ST8_SHORTPTRW:
		case ST8_SHORTOFF_X:
		case ST8_SHORTOFF_Y:
		case ST8_SHORTOFF_SP:
			ret++;
			break;
		case ST8_LONGPTRE_Y:
		case ST8_LONGPTRE_X:
		case ST8_LONGPTRW_Y:
		case ST8_LONGPTRW_X:
		case ST8_LONGPTRE:
		case ST8_LONGPTRW:
		case ST8_LONGOFF_X:
		case ST8_LONGOFF_Y:
		case ST8_LONGMEM:
		case ST8_WORD:
			ret += 2;
			break;
		case ST8_EXTMEM:
		case ST8_EXTOFF_X:
		case ST8_EXTOFF_Y:
			ret += 3;
			break;
		case ST8_END:
		case ST8_BIT_0:
		case ST8_BIT_1:
		case ST8_BIT_2:
		case ST8_BIT_3:
		case ST8_BIT_4:
		case ST8_BIT_5:
		case ST8_BIT_6:
		case ST8_BIT_7:
		case ST8_REG_CC:
		case ST8_REG_A:
		case ST8_REG_R0:
		case ST8_REG_R1:
		case ST8_REG_R2:
		case ST8_REG_R3:
		case ST8_REG_R4:
		case ST8_REG_R5:
		case ST8_REG_R6:
		case ST8_REG_R7:
		case ST8_REG_DPTR:
		case ST8_REG_AB:
		case ST8_REG_C:
		case ST8_REG_AT_R0:
		case ST8_REG_AT_R1:
		case ST8_REG_AT_DPTR:
		case ST8_REG_AT_A_DPTR:
		case ST8_REG_AT_A_PC:
		case ST8_REG_X:
		case ST8_REG_Y:
		case ST8_REG_SP:
		case ST8_REG_XL:
		case ST8_REG_XH:
		case ST8_REG_YL:
		case ST8_REG_YH:
		case ST8_INDX:
		case ST8_INDY:
			break;
		}
	}
	ret += stm8_opcode_size(opcode.bin_opcode);
	return(ret);
}
