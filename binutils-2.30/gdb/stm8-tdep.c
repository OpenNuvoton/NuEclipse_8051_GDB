/* Target-dependent code for STM8, for GDB.
   Written by Ake Rehnman 2017-02-21,
   ake.rehnman (at) gmail dot com

   Copyright (C) 1996-2018 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdbtypes.h"
#include "inferior.h"
#include "symfile.h"
#include "arch-utils.h"
#include "regcache.h"
#include "dis-asm.h"
#include "objfiles.h"
#include "target-descriptions.h"
#include "dwarf2-frame.h"

enum stm8_regnum
{
	NU51_PSW_REGNUM,
	NU51_SP_REGNUM,
	NU51_B_REGNUM,
	NU51_ACC_REGNUM,
	NU51_DPL_REGNUM,
	NU51_DPH_REGNUM,
	NU51_R0_REGNUM,
	NU51_R1_REGNUM,
	NU51_R2_REGNUM,
	NU51_R3_REGNUM,
	NU51_R4_REGNUM,
	NU51_R5_REGNUM,
	NU51_R6_REGNUM,
	NU51_R7_REGNUM,
	NU51_PC_REGNUM,
	NU51_B0_REGNUM,
	NU51_B1_REGNUM,
	NU51_B2_REGNUM,
	NU51_B3_REGNUM,
	NU51_B4_REGNUM,
	NU51_B5_REGNUM,
	NU51_B6_REGNUM,
	NU51_B7_REGNUM,
	NU51_X8_REGNUM,
	NU51_X9_REGNUM,
	NU51_X10_REGNUM,
	NU51_X11_REGNUM,
	NU51_X12_REGNUM,
};

#define STM8_NUM_PSEUDOREGS 15

enum stm8_producer
{
  GCC_PRODUCER,
  SDCC_PRODUCER
};

static const char *stm8_register_names[] =
{
//  "pc", "a",   "x",  "y",  "sp", "cc"
	"psw", "sp", "b", "acc", "dpl", "dph", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "pc"
};

unsigned int stm8_debug = 0;

#define STM8_NUM_REGS ARRAY_SIZE (stm8_register_names)

struct gdbarch_tdep
{
  enum stm8_producer producer;
  /* Type for void.  */
  struct type *void_type;
  /* Type for a function returning void.  */
  struct type *func_void_type;
  /* Type for a pointer to a function.  Used for the type of PC.  */
  struct type *pc_type;
};

enum insn_return_kind {
  RETURN_RET,
  RETURN_RETF,
  RETURN_IRET
};

static int
stm8_convert_register_p (struct gdbarch *gdbarch, int regnum,
		struct type *type)
{
	struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

	if ((regnum == NU51_SP_REGNUM) && (TYPE_LENGTH(type) > 2))
	{
		return 1;
	}
	if ((regnum == NU51_DPL_REGNUM) && (TYPE_LENGTH(type) > 2))
	{
		return 1;
	}
	if ((regnum == NU51_DPH_REGNUM) && (TYPE_LENGTH(type) > 2))
	{
		return 1;
	}

	return 0;
}

/* Read a value of type TYPE from register REGNUM in frame FRAME, and
   return its contents in TO.  */

static int
stm8_register_to_value (struct frame_info *frame, int regnum,
		struct type *type, gdb_byte *to,
		int *optimizedp, int *unavailablep)
{
	/* Convert to TYPE.  */

	memset(to,0,TYPE_LENGTH(type));

	if (!get_frame_register_bytes (frame, regnum, 0, 2,
			to+TYPE_LENGTH(type)-2, optimizedp, unavailablep))
		return 0;

	*optimizedp = *unavailablep = 0;
	return 1;
}

static stm8_producer
stm8_get_producer()
{
  struct objfile *objfile;
  struct compunit_symtab *cust;

  if (current_program_space != NULL)
  {
    ALL_COMPUNITS(objfile, cust)
    {
      if (cust
        && COMPUNIT_PRODUCER (cust) != NULL
        && startswith (COMPUNIT_PRODUCER (cust), "SDCC"))
      {
        return SDCC_PRODUCER;
      }
    }
  }
  return GCC_PRODUCER;
}

static void
stm8_initialize_soft_register_info(struct gdbarch_tdep *tdep)
{
  tdep->producer = stm8_get_producer();
}

static const char *
stm8_register_name (struct gdbarch *gdbarch, int regnum)
{
	if (regnum >= 0 && regnum < STM8_NUM_REGS)
		return stm8_register_names[regnum];

	if (stm8_get_producer() == SDCC_PRODUCER)
	{
		switch (regnum)
		{
		case NU51_DPH_REGNUM:
			return "xh";
		case NU51_DPL_REGNUM:
			return "xl";
			//			case STM8_YH_REGNUM :
			//				return "yh";
			//			case STM8_YL_REGNUM :
			//				return "yl";
		}
	}
	return NULL;
}

static struct type *
stm8_register_type (struct gdbarch *gdbarch, int regnum)
{
	struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

	switch (regnum)
	{
	case NU51_PC_REGNUM:
		return builtin_type(gdbarch)->builtin_uint32;
		//	case STM8_PC_REGNUM:
		//		return builtin_type (gdbarch)->builtin_uint32;
		//	case STM8_SP_REGNUM:
		//		return builtin_type (gdbarch)->builtin_uint16;
		//	case STM8_X_REGNUM:
		//	case STM8_Y_REGNUM:
		//		return builtin_type (gdbarch)->builtin_uint16;
	default:
		return builtin_type (gdbarch)->builtin_uint8;
	}
}

static enum register_status
stm8_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
		int regnum, gdb_byte *buf)
{
	enum register_status status;
	struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
	enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
	int regsize = 2;
	gdb_byte tmp[4];

	switch (regnum)
	{
		//	case NU51_PC_REGNUM:
		//		status = regcache_raw_read(regcache, NU51_PC_REGNUM, tmp);
		//		if (status == REG_VALID)
		//		{
		//			buf[0] = tmp[0];
		//		}
		//		return status;
		//
		//	case NU51_SP_REGNUM:
		//		status = regcache_raw_read(regcache, NU51_SP_REGNUM, tmp);
		//		if (status == REG_VALID)
		//		{
		//			buf[0] = tmp[0];
		//		}
		//		return status;
		//
	case NU51_DPH_REGNUM:
		status = regcache_raw_read(regcache, NU51_DPH_REGNUM, tmp);
		if (status == REG_VALID)
		{
			buf[0] = tmp[0];
		}
		return status;

	case NU51_DPL_REGNUM:
		status = regcache_raw_read(regcache, NU51_DPL_REGNUM, tmp);
		if (status == REG_VALID)
		{
			buf[0] = tmp[1];
		}
		return status;

		//	case STM8_YH_REGNUM:
		//		status = regcache_raw_read(regcache, STM8_Y_REGNUM, tmp);
		//		if (status == REG_VALID)
		//		{
		//			buf[0] = tmp[0];
		//		}
		//		return status;
		//
		//	case STM8_YL_REGNUM:
		//		status = regcache_raw_read(regcache, STM8_Y_REGNUM, tmp);
		//		if (status == REG_VALID)
		//		{
		//			buf[0] = tmp[1];
		//		}
		//	 	return status;
		//
	default:
		internal_error (__FILE__, __LINE__, _("invalid regnum"));
		return REG_UNAVAILABLE;
	}
}

static void
stm8_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
		int regnum, const gdb_byte *buf)
{
	enum register_status status;
	enum bfd_endian byte_order = gdbarch_byte_order (target_gdbarch ());
	struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
	const int regsize = 2;
	gdb_byte tmp[4];

	switch (regnum)
	{

	case NU51_DPH_REGNUM:
		status = regcache_raw_read(regcache, NU51_DPH_REGNUM, tmp);
		if (status == REG_VALID)
		{
			tmp[0] = buf[0];
			regcache_raw_write(regcache, NU51_DPH_REGNUM, tmp);
		}
		return;

	case NU51_DPL_REGNUM:
		status = regcache_raw_read(regcache, NU51_DPL_REGNUM, tmp);
		if (status == REG_VALID)
		{
			tmp[1] = buf[0];
			regcache_raw_write(regcache, NU51_DPL_REGNUM, tmp);
		}
		return;

		//	case STM8_YH_REGNUM:
		//		status = regcache_raw_read(regcache, STM8_Y_REGNUM, tmp);
		//		if (status == REG_VALID)
		//		{
		//		  tmp[0] = buf[0];
		//		  regcache_raw_write (regcache, STM8_Y_REGNUM, tmp);
		//	  }
		//	  return;
		//
		//	case STM8_YL_REGNUM:
		//		status = regcache_raw_read(regcache, STM8_Y_REGNUM, tmp);
		//		if (status == REG_VALID)
		//		{
		//		  tmp[1] = buf[0];
		//		  regcache_raw_write (regcache, STM8_Y_REGNUM, tmp);
		//	  }
		//	  return;

	default:
		internal_error (__FILE__, __LINE__, _("invalid regnum"));
		return;
	}
}


struct stm8_frame_cache
{
	/* Base address.  */
	CORE_ADDR base;
	CORE_ADDR pc;

	/* Do we have a frame?  */
	int frameless_p;

	/* Frame size.  */
	int framesize;

	/* Frame size.  */
	int stackadj;

	/* Frame register.  */
	int fp_regnum;

	/* Return instruction */
	enum insn_return_kind return_kind;

	/* Offsets to saved registers.  */
	int register_offsets[STM8_NUM_REGS];

	/* Table of saved registers.  */
	struct trad_frame_saved_reg *saved_regs;
};

static const unsigned char *
stm8_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pcptr,
			    int *lenptr)
{
	static gdb_byte stm8_breakpoint[] = { 0x8b };

  *lenptr = sizeof (stm8_breakpoint);
  return stm8_breakpoint;
}

/* Implement the breakpoint_kind_from_pc gdbarch method.  */
static int
stm8_breakpoint_kind_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pcptr)
{
	CORE_ADDR pc = *pcptr;

	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_breakpoint_kind_from_pc called %8.8lx\n", (unsigned long)pc);

	return 1;
}

static const gdb_byte *
stm8_sw_breakpoint_from_kind (struct gdbarch *gdbarch, int kind, int *size)
{
	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_sw_breakpoint_from_kind called\n");

	enum bfd_endian byte_order_for_code = gdbarch_byte_order_for_code (gdbarch);

	static gdb_byte stm8_breakpoint[] = { 0x8b };

	*size = 1;

	return stm8_breakpoint;
}

static int dwarf2_to_reg_map_sdcc[] =
	{
		NU51_R7_REGNUM,
		NU51_R6_REGNUM,
		NU51_R5_REGNUM,
		NU51_R4_REGNUM,
		NU51_R3_REGNUM,
		NU51_R2_REGNUM,
		NU51_R1_REGNUM,
		NU51_R0_REGNUM,
		NU51_B0_REGNUM,
		NU51_B1_REGNUM,
		NU51_B2_REGNUM,
		NU51_B3_REGNUM,
		NU51_B4_REGNUM,
		NU51_B5_REGNUM,
		NU51_B6_REGNUM,
		NU51_B7_REGNUM,
		NU51_X8_REGNUM,
		NU51_X9_REGNUM,
		NU51_X10_REGNUM,
		NU51_X11_REGNUM,
		NU51_X12_REGNUM,
		NU51_PSW_REGNUM,
		NU51_SP_REGNUM,
		NU51_DPL_REGNUM,
		NU51_DPH_REGNUM,
		NU51_B_REGNUM,
		NU51_ACC_REGNUM,
		NU51_PC_REGNUM,
		//  STM8_A_REGNUM,  //a
		//  STM8_XL_REGNUM,  //xl
		//  STM8_XH_REGNUM,  //xh
		//  STM8_YL_REGNUM,  //yl
		//  STM8_YH_REGNUM,  //yh
		//  STM8_CC_REGNUM,  //cc
		//  STM8_X_REGNUM,  //x
		//  STM8_Y_REGNUM,  //y
		//  STM8_SP_REGNUM,  //sp
		//  STM8_PC_REGNUM,  //pc
		-1};

static int dwarf2_to_reg_map_gcc[] =
{ 
	NU51_SP_REGNUM,
	NU51_ACC_REGNUM,
	NU51_DPL_REGNUM,
	NU51_DPH_REGNUM,
//  STM8_A_REGNUM,  //a
//  STM8_X_REGNUM,  //x
//  STM8_Y_REGNUM,  //y
//  STM8_SP_REGNUM, //sp
  -1
};

static int
stm8_dwarf2_reg_to_regnum (struct gdbarch *gdbarch, int reg)
{
	int ret = -1;

	struct gdbarch_tdep *tdep = gdbarch_tdep(gdbarch);
	static int *t;

	if (stm8_get_producer() == SDCC_PRODUCER)
	{
		t = dwarf2_to_reg_map_sdcc;
	}
	else
	{
		t = dwarf2_to_reg_map_gcc;
	}

	for (int i = 0; (t[i] >= 0) && (i < 32); i++)
	{
		if (i == reg)
		{
			ret = t[i];
			break;
		}
	}

  if ((stm8_debug>1) && (ret>=0))
	fprintf_unfiltered (gdb_stdlog, "stm8_dwarf2_reg_to_regnum called reg=%d ret=%d\n",reg,t[reg]);

  return ret;
}

static void
stm8_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_write_pc called\n");
	regcache_cooked_write_unsigned (regcache, NU51_PC_REGNUM, pc);
}

static CORE_ADDR
stm8_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
	gdb_byte buf[4];
	CORE_ADDR pc;

	struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
	frame_unwind_register (next_frame, NU51_PC_REGNUM, buf);
	if (frame_relative_level(next_frame) < 0)
		pc = extract_typed_address (buf, builtin_type (gdbarch)->builtin_func_ptr);
	else
		pc = extract_typed_address (buf, tdep->pc_type);
	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_unwind_pc called: pc=%8.8lx\n", (unsigned long)pc);
	return pc;
}

static CORE_ADDR
stm8_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
	CORE_ADDR sp;

	sp = frame_unwind_register_unsigned (next_frame, NU51_SP_REGNUM);
	sp = gdbarch_addr_bits_remove (gdbarch, sp);
	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_unwind_sp called: sp=%8.8lx\n", (unsigned long)sp);
	return sp;
}


/* Allocate and initialize a frame cache.  */

static struct stm8_frame_cache *
stm8_alloc_frame_cache (void)
{
	struct stm8_frame_cache *cache;

	cache = FRAME_OBSTACK_ZALLOC (struct stm8_frame_cache);

	//  /* Base address.  */
	cache->base = 0;
	cache->pc = 0;

	/* Frameless until proven otherwise.  */
	cache->frameless_p = 1;

	return cache;
}

/* Figure out what return type this function has.  */
static enum insn_return_kind
stm8_get_return_insn (CORE_ADDR pc)
{
	const char *name;
	CORE_ADDR func_addr, func_end, stop;

	if (find_pc_partial_function (pc, &name, &func_addr, &func_end))
	{
		gdb_byte buf[1];
		//func_end is pointing to last insn+1 ???
		if (!target_read_code (func_end-1, buf, sizeof (buf)))
		{
			switch (buf[0])
			{
			case 0x81:
				return RETURN_RET;
			case 0x87:
				return RETURN_RETF;
			case 0x80:
				return RETURN_IRET;
			}
		}
	}

	//defaut to RET
	if (stm8_debug)
		fprintf_unfiltered(gdb_stdlog, "WARNING: stm8_get_return_insn: No return instruction found in function %s start_addr = %8.8lx end_addr = %8.8lx\n", name, (unsigned long)func_addr, (unsigned long)func_end);
	return RETURN_RET;
}

/* Find the start of this function.  */
static CORE_ADDR
stm8_analyze_prologue (struct gdbarch *gdbarch, CORE_ADDR pc,
		CORE_ADDR current_pc,
		struct stm8_frame_cache *cache)
{
	const char *name;
	CORE_ADDR func_addr, func_end, stop;
	unsigned long insn;
	int done;
	gdb_byte buf[4];

	if (stm8_debug)
		fprintf_unfiltered(gdb_stdlog, "stm8_analyze_prologue called (pc=%8.8lx current_pc=%8.8lx)\n", (unsigned long)pc, (unsigned long)current_pc);

	/* Initialize info about frame.  */
	cache->framesize = 0;
	cache->frameless_p = 1;
	cache->stackadj = 0;

	/* Find the start of this function.  */
	find_pc_partial_function (pc, &name, &func_addr, &func_end);
	if (func_addr < pc)
		pc = func_addr;

	if (current_pc < pc)
		return current_pc;

	/* Start decoding the prologue.  We start by checking two special cases:

     1. We're about to return
     2. We're at the first insn of the prologue.

     If we're about to return, our frame has already been deallocated.
     If we are stopped at the first instruction of a prologue,
     then our frame has not yet been set up.  */

	/* Get the first insn from memory.  */
	if (target_read_code (pc, buf, sizeof (buf)))
		return current_pc;

	//RET?
	if (buf[0] == 0x81)
	{
		if (stm8_debug)
			fprintf_unfiltered (gdb_stdlog, "stm8_analyze_prologue: current_pc is RTS so we dont have a frame!\n");
		return pc;
	}

	/* Start at beginning of function and analyze until we get to the
	current pc, or the end of the function, whichever is first.  */
	stop = (current_pc < func_end ? current_pc : func_end);

	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_analyze_prologue: name=%s, func_addr=%s, stop=%s\n",
				name, paddress (gdbarch, func_addr),
				paddress (gdbarch, stop));

	/* scan the prologue */
	pc = func_addr;
	done = 0;
	while ((pc < stop) && !done)
	{
		if (target_read_code (pc, buf, sizeof (buf)))
			return current_pc;

		insn = buf[0];
		if (buf[0] == 0x90)
			insn = (insn << 8) + buf[1];

		switch (insn)
		{
		case 0x3b:
			//PUSH extmem
			cache->framesize += 1;
			pc += 3;
			break;
		case 0x9096:
			//LDW Y, SP
			pc += 2;
			break;
		case 0x90cf:
			//LDW extmem, Y
			pc += 4;
			cache->frameless_p = 0;
			break;
		case 0x52:
			//SUB SP,#xx?
			cache->framesize += buf[1];
			pc+=2;
			break;
		case 0x88:
			//PUSH A?
			cache->framesize += 1;
			pc++;
			break;
		case 0x89:
			//PUSH X?
			cache->framesize += 2;
			pc++;
			break;
		case 0x9089:
			//PUSH Y?
			cache->framesize += 2;
			pc+=2;
			break;
		default:
			done = 1;
		}

	}

	if (stm8_debug)
		if (cache->frameless_p)
			fprintf_unfiltered (gdb_stdlog, "stm8_analyze_prologue: this function is frameless! No pc adjustment was done.\n");

	return pc;
}

/* Return PC of first real instruction of the function starting at
   START_PC.  */

static CORE_ADDR
stm8_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR start_pc)
{
	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_skip_prologue called: start_pc=%8.8lx\n", (unsigned long)start_pc);

	struct symtab_and_line sal;
	CORE_ADDR func_start, func_end, ostart_pc;
	CORE_ADDR post_prologue_pc;
	struct stm8_frame_cache cache;

	/* This is the preferred method, find the end of the prologue by
     using the debugging information.  Debugging info does not always
     give the right answer since parameters are stored on stack after this.
     Always analyze the prologue.  */
	if (find_pc_partial_function (start_pc, NULL, &func_start, &func_end))
	{
		post_prologue_pc = skip_prologue_using_sal (gdbarch, func_start);

		if (stm8_debug)
			fprintf_unfiltered (gdb_stdlog, "stm8_skip_prologue: post_prologue_pc=%8.8lx\n",
				(unsigned long)post_prologue_pc);
		
		if (post_prologue_pc != 0)
			return std::max (start_pc, post_prologue_pc);
	}

	ostart_pc = stm8_analyze_prologue (gdbarch, func_start, 0xffffffffUL,
			&cache);

	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_skip_prologue: start_pc=%8.8lx ostart_pc=%8.8lx func_start=%8.8lx func_end=%8.8lx sal.end=%8.8lx\n", 
		(unsigned long)start_pc, (unsigned long)ostart_pc, 
		(unsigned long)func_start, (unsigned long)func_end, (unsigned long)sal.end);

	if (ostart_pc > start_pc)
		return ostart_pc;
	return start_pc;
}

/* Normal frames.

  The STM8 frame is typically:

     args
     return lo addr
     return hi addr
     prev fp lo
     prev fp hi
     locals <- fp
sp-> empty
 */

static struct stm8_frame_cache *
stm8_frame_cache (struct frame_info *next_frame, void **this_cache)
{
	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_frame_cache called (next_frame=%p)\n", next_frame);

	struct stm8_frame_cache *cache;
	struct gdbarch *gdbarch = get_frame_arch (next_frame);
	CORE_ADDR start_pc, current_pc, current_sp;
	int retsize;

	if (*this_cache)
		return (struct stm8_frame_cache *) *this_cache;

	cache = stm8_alloc_frame_cache ();
	*this_cache = cache;

	cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

	start_pc = get_frame_func (next_frame);

	cache->return_kind = stm8_get_return_insn (start_pc);

	current_pc = get_frame_pc (next_frame);
	current_sp = get_frame_sp (next_frame);

	/* Analyze the function prologue.  */
	if (start_pc != 0)
		stm8_analyze_prologue (gdbarch, start_pc, current_pc,
				(struct stm8_frame_cache *) *this_cache);

	switch (cache->return_kind)
	{
	default:
	case RETURN_RET:
		retsize = 2;
		break;
	case RETURN_RETF:
		retsize = 3;
		break;
	case RETURN_IRET:
		retsize = 9;
		break;
	}

	cache->base = current_sp;
	if (cache->return_kind == RETURN_IRET)
		cache->saved_regs[NU51_PC_REGNUM].addr = cache->base + 1 + 6;
	else
		cache->saved_regs[NU51_PC_REGNUM].addr = cache->base + 1;
	trad_frame_set_value (cache->saved_regs,
			NU51_SP_REGNUM,
			cache->base+retsize);

	if (stm8_debug)
	{
		fprintf_unfiltered (gdb_stdlog, "stm8_frame_cache: (next_frame=%p) base=%4.4lx curr_pc=%4.4lx curr_sp=%4.4lx framesize=%4.4x stackadj=%4.4x retsize=%d\n",
				next_frame, (unsigned long)cache->base, (unsigned long)current_pc, (unsigned long)current_sp, cache->framesize, cache->stackadj, retsize);

		CORE_ADDR frame_pc;
		CORE_ADDR frame_sp;
		frame_pc = value_as_long(trad_frame_get_prev_register (next_frame, cache->saved_regs, STM8_PC_REGNUM));
		frame_sp =  value_as_long(trad_frame_get_prev_register (next_frame, cache->saved_regs, STM8_SP_REGNUM));

		frame_pc = frame_pc >> 16;
		fprintf_unfiltered (gdb_stdlog, "stm8_frame_cache: (next_frame=%p) pc=%8.8lx *pc=%8.8lx\n", next_frame, (unsigned long)cache->saved_regs[STM8_PC_REGNUM].addr, (unsigned long)frame_pc);
		fprintf_unfiltered (gdb_stdlog, "stm8_frame_cache: (next_frame=%p) sp=%8.8lx *sp=%8.8lx\n", next_frame, (unsigned long)cache->saved_regs[STM8_SP_REGNUM].addr, (unsigned long)frame_sp);
	}

	return (struct stm8_frame_cache *) (*this_cache);
}

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */
static void
stm8_frame_this_id (struct frame_info *next_frame, void **this_cache,
		struct frame_id *this_id)
{
	struct stm8_frame_cache *cache =
			stm8_frame_cache (next_frame, this_cache);
	CORE_ADDR func;
	CORE_ADDR base;

	func = get_frame_func (next_frame);
	base = cache->base;

	/* This marks the outermost frame.  */
	if (base == 0)
		return;

	(*this_id) = frame_id_build (base, func);

	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_frame_this_id: base=%8.8lx pc=%8.8lx\n", (unsigned long)base, (unsigned long)func);
}

static struct value *
stm8_frame_prev_register (struct frame_info *this_frame,
		void **this_cache, int regnum)
{
	struct value *value;
	struct stm8_frame_cache *info = stm8_frame_cache (this_frame,
			this_cache);

	value = trad_frame_get_prev_register (this_frame, info->saved_regs, regnum);

	if (stm8_debug)
		fprintf_unfiltered (gdb_stdlog, "stm8_frame_prev_register: regnum(%d)=%8.8lx\n", regnum, (unsigned long)value_as_long(value));

	return value;
}

static void
stm8_store_return_value (struct type *type, struct regcache *regcache,
		const gdb_byte *valbuf)
{
	int len;

	len = TYPE_LENGTH (type);

	if (len == 1)
		// regcache_raw_write_part (regcache, STM8_A_REGNUM, 0, 1, valbuf);
		regcache_raw_write_part(regcache, NU51_ACC_REGNUM, 0, 1, valbuf);
	else if (len == 2)
	{
		// regcache_raw_write_part (regcache, STM8_X_REGNUM, 0, 2, valbuf);
		regcache_raw_write_part(regcache, NU51_DPL_REGNUM, 0, 1, valbuf);
		regcache_raw_write_part(regcache, NU51_DPH_REGNUM, 0, 1, valbuf);
	}
	//	else if (len == 4)
	//	{
	//		regcache_raw_write_part (regcache, STM8_X_REGNUM, 0, 2, valbuf+2);
	//		regcache_raw_write_part (regcache, STM8_Y_REGNUM, 0, 2, valbuf);
	//	}
	else
		error (_("return of value > 4 is not supported."));
}

/* Given a return value in `regcache' with a type `type',
   extract and copy its value into `valbuf'.  */

static void
stm8_extract_return_value (struct type *type, struct regcache *regcache,
		void *valbuf)
{
	gdb_byte buf[2];

	switch (TYPE_LENGTH (type))
	{
	case 1:
		// regcache_raw_read (regcache, STM8_A_REGNUM, buf);
		regcache_raw_read(regcache, NU51_ACC_REGNUM, buf);
		memcpy(valbuf, buf, 1);
		break;

	case 2:
		// regcache_raw_read (regcache, STM8_X_REGNUM, buf);
		regcache_raw_read(regcache, NU51_DPL_REGNUM, buf);
		memcpy((char *)valbuf + 1, buf, 1);
		regcache_raw_read(regcache, NU51_DPH_REGNUM, buf);
		memcpy(valbuf, buf, 1);
		break;

		//	case 4:
		//		regcache_raw_read (regcache, STM8_X_REGNUM, buf);
		//		memcpy ((char*) valbuf + 2, buf, 2);
		//		regcache_raw_read (regcache, STM8_Y_REGNUM, buf);
		//		memcpy (valbuf, buf, 2);
		//		break;

	default:
		error (_("bad size for return value"));
	}
}

static enum return_value_convention
stm8_return_value (struct gdbarch *gdbarch, struct value *function,
		struct type *valtype, struct regcache *regcache,
		gdb_byte *readbuf, const gdb_byte *writebuf)
{
	if (TYPE_CODE (valtype) == TYPE_CODE_STRUCT
			|| TYPE_CODE (valtype) == TYPE_CODE_UNION
			|| TYPE_CODE (valtype) == TYPE_CODE_ARRAY
			|| TYPE_LENGTH (valtype) > 4)
		return RETURN_VALUE_STRUCT_CONVENTION;
	else
	{
		if (readbuf != NULL)
			stm8_extract_return_value (valtype, regcache, readbuf);
		if (writebuf != NULL)
			stm8_store_return_value (valtype, regcache, writebuf);
		return RETURN_VALUE_REGISTER_CONVENTION;
	}
}

static const struct frame_unwind stm8_frame_unwind = {
		NORMAL_FRAME,
		default_frame_unwind_stop_reason,
		stm8_frame_this_id,
		stm8_frame_prev_register,
		NULL,
		default_frame_sniffer
};

//static CORE_ADDR
//stm8_frame_base_address (struct frame_info *next_frame,
//		void **this_cache)
//{
//	struct stm8_frame_cache *cache =
//			stm8_frame_cache (next_frame, this_cache);
//
//	if (stm8_debug)
//		fprintf_unfiltered (gdb_stdlog, "stm8_frame_base_address: fb=%8.8lx\n", (unsigned long)cache->base);
//
//	return cache->base;
//}

//static CORE_ADDR
//stm8_frame_args_address (struct frame_info *this_frame, void **this_cache)
//{
//	CORE_ADDR addr;
//	struct stm8_frame_cache *info
//	= stm8_frame_cache (this_frame, this_cache);
//
//	addr = info->base;
//	if (info->return_kind == RETURN_IRET)
//		addr += 12; //2 bytes fp + 9 bytes regs + 1
//	else if (info->return_kind == RETURN_RETF)
//		addr += 6; //2 bytes fp + 3 bytes pc + 1
//	else
//		addr += 5; //2 bytes fp + 2 bytes pc + 1
//
//	if (stm8_debug)
//		fprintf_unfiltered (gdb_stdlog, "stm8_frame_args_address: addr = %8.8lx\n", (unsigned long)addr);
//
//	return addr;
//}

//static const struct frame_base stm8_frame_base =
//{
//		&stm8_frame_unwind,
//		stm8_frame_base_address,
//		stm8_frame_base_address,
//		stm8_frame_args_address
//};
//
//static const struct frame_base *
//stm8_frame_base_sniffer (struct frame_info *this_frame)
//{
//	return &stm8_frame_base;
//}

struct target_desc *tdesc_stm8;
static void
initialize_tdesc_stm8 (void)
{
	struct target_desc *result = allocate_target_description ();
	struct tdesc_feature *feature;

	feature = tdesc_create_feature(result, "org.gnu.gdb.nu8051.core");

	tdesc_create_reg(feature, "psw", 0, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "sp", 1, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "b", 2, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "acc", 3, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "dpl", 4, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "dph", 5, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r0", 6, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r1", 7, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r2", 8, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r3", 9, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r4", 10, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r5", 11, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r6", 12, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "r7", 13, 1, "general", 8, "uint8");
	tdesc_create_reg(feature, "pc", 14, 1, "general", 32, "uint32");

	//	feature = tdesc_create_feature (result, "org.gnu.gdb.stm8.core");
	//	tdesc_create_reg (feature, "pc", 0, 1, "general", 32, "uint32");
	//	tdesc_create_reg (feature, "a", 1, 1, "general", 8, "uint8");
	//	tdesc_create_reg (feature, "x", 2, 1, "general", 16, "uint16");
	//	tdesc_create_reg (feature, "y", 3, 1, "general", 16, "uint16");
	//	tdesc_create_reg (feature, "sp", 4, 1, "general", 16, "uint16");
	//	tdesc_create_reg (feature, "cc", 5, 1, "general", 8, "uint8");

	tdesc_stm8 = result;
}


/* Initialize the gdbarch structure for the STM8.  */

static struct gdbarch *
stm8_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
	struct gdbarch *gdbarch;
	struct gdbarch_tdep *tdep;
	struct gdbarch_list *best_arch;
	struct tdesc_arch_data *tdesc_data = NULL;
	const struct target_desc *tdesc = info.target_desc = 0; // override target desc if any

	/* If there is already a candidate, use it.  */
	arches = gdbarch_list_lookup_by_info(arches, &info);
	if (arches != NULL)
	{
		tdep = gdbarch_tdep(arches->gdbarch);
		stm8_initialize_soft_register_info(tdep);
		return arches->gdbarch;
	}
	if (tdesc == NULL)
		tdesc = tdesc_stm8;

	/* Check any target description for validity.  */
	if (tdesc_has_registers(tdesc))
	{
		const struct tdesc_feature *feature;
		int valid_p;
		int i;

		feature = tdesc_find_feature(tdesc,
									 "org.gnu.gdb.nu8051.core");
		if (feature == NULL)
			return NULL;
		tdesc_data = tdesc_data_alloc();

		valid_p = 1;
		for (int i = 0; i < STM8_NUM_REGS; i++)
			valid_p &= tdesc_numbered_register(feature, tdesc_data, i,
											   stm8_register_names[i]);
		if (!valid_p)
		{
			tdesc_data_cleanup(tdesc_data);
			return NULL;
		}
	}

	/* None found, create a new architecture from the information provided.  */
	tdep = XNEW(struct gdbarch_tdep);
	gdbarch = gdbarch_alloc(&info, tdep);

	/* Initialize soft registers */
	stm8_initialize_soft_register_info(tdep);

	set_gdbarch_num_regs(gdbarch, STM8_NUM_REGS);
	set_gdbarch_register_name(gdbarch, stm8_register_name);
	set_gdbarch_register_type(gdbarch, stm8_register_type);
	set_tdesc_pseudo_register_type(gdbarch, stm8_register_type);
	set_tdesc_pseudo_register_name(gdbarch, stm8_register_name);

	set_gdbarch_num_pseudo_regs(gdbarch, STM8_NUM_PSEUDOREGS);
	set_gdbarch_pseudo_register_read(gdbarch, stm8_pseudo_register_read);
	set_gdbarch_pseudo_register_write(gdbarch, stm8_pseudo_register_write);

	set_gdbarch_convert_register_p(gdbarch, stm8_convert_register_p);
	set_gdbarch_register_to_value(gdbarch, stm8_register_to_value);

	/* Register numbers of various important registers.  */
	// set_gdbarch_sp_regnum (gdbarch, STM8_SP_REGNUM);
	// set_gdbarch_pc_regnum (gdbarch, STM8_PC_REGNUM);
	// set_gdbarch_ps_regnum (gdbarch, STM8_CC_REGNUM);
	set_gdbarch_sp_regnum(gdbarch, NU51_SP_REGNUM);
	set_gdbarch_pc_regnum(gdbarch, NU51_PC_REGNUM);
	set_gdbarch_ps_regnum(gdbarch, NU51_ACC_REGNUM);

	//set_gdbarch_ptr_bit (gdbarch, 2 * TARGET_CHAR_BIT);
	//set_gdbarch_addr_bit (gdbarch, 3 * TARGET_CHAR_BIT);

  	/* Map Dwarf2 registers to GDB registers.  */
  	set_gdbarch_dwarf2_reg_to_regnum (gdbarch, stm8_dwarf2_reg_to_regnum);

  	/* Call dummy code.  */
  	set_gdbarch_call_dummy_location (gdbarch, ON_STACK);

  	set_gdbarch_return_value (gdbarch, stm8_return_value);

  	set_gdbarch_skip_prologue (gdbarch, stm8_skip_prologue);

  	/* Stack grows downward.  */
  	set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  	set_gdbarch_breakpoint_kind_from_pc (gdbarch, stm8_breakpoint_kind_from_pc);
  	set_gdbarch_sw_breakpoint_from_kind (gdbarch,  stm8_sw_breakpoint_from_kind);

  	set_gdbarch_breakpoint_from_pc (gdbarch, stm8_breakpoint_from_pc);

  	//set_gdbarch_frame_args_skip (gdbarch, 8);

  	set_gdbarch_print_insn (gdbarch, print_insn_stm8);

  	set_gdbarch_write_pc (gdbarch, stm8_write_pc);

  	set_gdbarch_unwind_pc (gdbarch, stm8_unwind_pc);
  	set_gdbarch_unwind_sp (gdbarch, stm8_unwind_sp);
  	////set_gdbarch_dummy_id (gdbarch, stm8_dummy_id);

  	////frame_base_set_default (gdbarch, &stm8_frame_base);

  	/* Hook in ABI-specific overrides, if they have been registered.  */
 	//gdbarch_init_osabi (info, gdbarch);

	//dwarf2 cfi unwinder
  	//dwarf2_frame_set_init_reg (gdbarch, stm8_dwarf2_frame_init_reg);

  	dwarf2_append_unwinders (gdbarch);
  	frame_unwind_append_unwinder (gdbarch, &stm8_frame_unwind);
  	frame_base_append_sniffer (gdbarch, dwarf2_frame_base_sniffer);
  	////frame_base_append_sniffer (gdbarch, stm8_frame_base_sniffer); 

	//  /* Create a type for PC.  We can't use builtin types here, as they may not
	//     be defined.  */
	tdep->void_type = arch_type(gdbarch, TYPE_CODE_VOID, TARGET_CHAR_BIT, "void");
	tdep->func_void_type = make_function_type(tdep->void_type, NULL);
	tdep->pc_type = arch_type(gdbarch, TYPE_CODE_PTR, 2 * TARGET_CHAR_BIT, NULL);
	TYPE_TARGET_TYPE(tdep->pc_type) = tdep->func_void_type;
	TYPE_UNSIGNED(tdep->pc_type) = 1;

	//
	//  set_gdbarch_short_bit (gdbarch, 2 * TARGET_CHAR_BIT);
	//  set_gdbarch_int_bit (gdbarch, 2 * TARGET_CHAR_BIT);
	//  set_gdbarch_long_bit (gdbarch, 4 * TARGET_CHAR_BIT);
	//  set_gdbarch_long_long_bit (gdbarch, 8 * TARGET_CHAR_BIT);
	//  set_gdbarch_ptr_bit (gdbarch, 2 * TARGET_CHAR_BIT);
	//  set_gdbarch_addr_bit (gdbarch, 32);
	//
	//  set_gdbarch_float_bit (gdbarch, 4 * TARGET_CHAR_BIT);
	//  set_gdbarch_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);
	//  set_gdbarch_long_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);
	//
	//  set_gdbarch_float_format (gdbarch, floatformats_ieee_single);
	//  set_gdbarch_double_format (gdbarch, floatformats_ieee_single);
	//  set_gdbarch_long_double_format (gdbarch, floatformats_ieee_single);
	//
	//
	//  set_gdbarch_push_dummy_call (gdbarch, stm8_push_dummy_call);
	//
	//  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, stm8_dwarf_reg_to_regnum);
	//
	//  set_gdbarch_address_to_pointer (gdbarch, stm8_address_to_pointer);
	//  set_gdbarch_pointer_to_address (gdbarch, stm8_pointer_to_address);
	//  set_gdbarch_integer_to_address (gdbarch, stm8_integer_to_address);
	//
	//  frame_base_set_default (gdbarch, &stm8_frame_base);
	//
	//  set_gdbarch_dummy_id (gdbarch, stm8_dummy_id);
	//
	//

	if (tdesc_data != NULL)
		tdesc_use_registers(gdbarch, tdesc, tdesc_data);
	if (stm8_debug)
		fprintf_unfiltered(gdb_stdlog, "stm8_gdbarch_init 2 called\n");
	return gdbarch;
}

static void
show_stm8_debug (struct ui_file *file, int from_tty,
		struct cmd_list_element *c, const char *value)
{
	fprintf_filtered (file, _("stm8 debugging is %s.\n"), value);
}

extern initialize_file_ftype _initialize_stm8_tdep; /* -Wmissing-prototypes */

void
_initialize_stm8_tdep (void)
{
	if (stm8_debug)
		fprintf_unfiltered(gdb_stdlog, "_initialize_stm8_tdep called\n");
	register_gdbarch_init (bfd_arch_stm8, stm8_gdbarch_init);
	initialize_tdesc_stm8 ();

	add_setshow_zuinteger_cmd ("stm8", class_maintenance, &stm8_debug,  _("\
Set stm8 debugging."), _("\
Show stm8 debugging."), _("\
When non-zero, stm8 debugging is enabled."),
NULL,
show_stm8_debug,
&setdebuglist, &showdebuglist);

}
