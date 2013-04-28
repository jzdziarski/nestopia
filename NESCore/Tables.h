/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                          Tables.h                       **/
/**                                                         **/
/** This file contains tables of used by 6502 emulation to  **/
/** compute NEGATIVE and ZERO flags. There are also timing  **/
/** tables for 6502 opcodes. This file is included from     **/
/** 6502.c.                                                 **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2007                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/*************************************************************/

static const byte Cycles[256] =
{
21,18, 0,24, 9, 9,15,15, 9, 6, 6, 6,12,12,18,18,
 6,15, 0,24,12,12,18,18, 6,12, 6,21,12,12,21,21,
18,18, 0,24, 9, 9,15,15,12, 6, 6, 6,12,12,18,18,
 6,15, 0,24,12,12,18,18, 6,12, 6,21,12,12,21,21,
18,18, 0,24, 9, 9,15,15, 9, 6, 6, 6, 9,12,18,18,
 6,15, 0,24,12,12,18,18, 6,12, 6,21,12,12,21,21,
18,18, 0,24, 9, 9,15,15,12, 6, 6, 6,15,12,18,18,
 6,15, 0,24,12,12,18,18, 6,12, 6,21,12,12,21,21,
 6,18, 6,18, 9, 9, 9, 9, 6, 6, 6, 6,12,12,12,12,
 6,18, 0,18,12,12,12,12, 6,15, 6,15,15,15,15,15,
 6,18, 6,18, 9, 9, 9, 9, 6, 6, 6, 6,12,12,12,12,
 6,15, 0,15,12,12,12,12, 6,12, 6,12,12,12,12,12,
 6,18, 6,24, 9, 9,15,15, 6, 6, 6, 6,12,12,18,18,
 6,15, 0,24,12,12,18,18, 6,12, 6,21,12,12,21,21,
 6,18, 6,24, 9, 9,15,15, 6, 6, 6, 6,12,12,18,18,
 6,15, 0,24,12,12,18,18, 6,12, 6,21,12,12,21,21
};

static const byte ZNTable[256] =
{
  Z_FLAG,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
  N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,N_FLAG,
};
